#include "./includes/Server.hpp"

void Server::openSocket() {
    if ( (Server::serverSocket = socket( AF_INET, SOCK_STREAM, 0) ) == -1 ) {
        throw ServerException( "Failed to create socket" );
    }

    int opt = 1;
    if ( setsockopt(Server::serverSocket, SOL_SOCKET, SO_REUSEADDR, ( char * )&opt, sizeof( int )) < 0 ) {
        throw ServerException( "setsockopt failed" );
    }

    Server::address.sin_family = AF_INET;
    Server::address.sin_addr.s_addr = INADDR_ANY;
    Server::address.sin_port = htons(_port);

    if ( bind(Server::serverSocket, ( struct sockaddr * )&Server::address, sizeof( Server::address )) < 0 ) {
        throw ServerException( "Bind failed" );
    }

    if ( listen(Server::serverSocket, MAX_CLIENTS) < 0 ) {
        throw ServerException( "Listen failed" );
    }

    addrlen = sizeof( Server::address );

    gethostname( c_hostName, MAX_HOST_NAME );
    Server::_hostName = c_hostName;
    std::cout << GREEN_LIGHT << "IRC Server started on port " << Server::_port << " : " << _hostName << std::endl;
    std::cout << "Waiting for incoming connections..." << RESET << std::endl;
}

/**
 * @brief Run the server | Accept incoming connections and handle client messages
 * 
 */
void Server::run( void ) {

    size_t i = 0;
    
    while (true) 
	{
        FD_ZERO( &Server::readfds );
        FD_SET( Server::serverSocket, &Server::readfds );
        Server::max_fds = serverSocket;

        for ( i = 0; i < Server::_fds.size(); i++ ) {
        
            Server::current_fd = Server::_fds[i];
            if ( Server::current_fd >= MAX_CLIENTS - 1 ) {
                for(std::vector<int>::iterator it = Server::_fds.begin(); it != Server::_fds.end(); ++it) {
                        close(*it);
                }
                shutdown(Server::serverSocket, SHUT_RDWR); 
                close(Server::serverSocket);
                Server::_fds.clear();
                Server::users_.clear();
                Server::channels_.clear();
                throw ServerException( "Max clients reached" );
            }
            if (Server::_fds.at(i) > 0)
                FD_SET( Server::current_fd, &Server::readfds );

            if ( Server::current_fd > Server::max_fds )
                Server::max_fds = Server::current_fd;
        }

        int active_fd = select( Server::max_fds + 1, &Server::readfds, NULL, NULL, NULL );
        if ( ( active_fd < 0 ) && ( errno != EINTR ) ) {
            throw ServerException( "Select error" );
        }

        if ( FD_ISSET( Server::serverSocket, &Server::readfds ) ) {
            acceptConnection();
        }
        handleClientMessages();
    }
}

void Server::acceptConnection() {
    if ( ( Server::newSocket = accept( Server::serverSocket, ( struct sockaddr * )&Server::address, ( socklen_t * )&Server::addrlen ) ) < 0 ) { 
        throw ServerException( "Accept failed" );
    }

	User(Sever::newSocket);
    // Server::_fds.push_back(Server::newSocket); already pushed in User constructor
    // Server::users_.push_back(User( Server::newSocket)); already pushed in User constructor
    std::cout << GREEN << "New connection, " << "IP is : " << inet_ntoa(Server::address.sin_addr) << 
        ", port : " << Server::_port << RESET << std::endl;
    if ( fcntl( Server::newSocket, F_SETFL, O_NONBLOCK ) < 0 ) {
        throw ServerException( "Failed to set client socket to non-blocking mode" );
    }
}

void Server::handleClientMessages() {

    int i = 0;
	std::vector<User>::iterator it_u;
	std::vector<User>::iterator it_o;
	std::vector<User>::iterator it_i;
    for ( i = 0; i < static_cast<int>( Server::_fds.size() ); i++ ) {
        Server::current_fd = Server::_fds.at(i);

        if ( FD_ISSET(Server::current_fd, &Server::readfds) ) {
		
            if ( (Server::received_bytes = recv(Server::current_fd, Server::client_buffer, BUFFER_SIZE, 0)) <= 0 ) {

                std::cout << RED << "Host disconnected | IP: " << inet_ntoa(Server::address.sin_addr) <<
                     " | port " << Server::_port << RESET << std::endl;
                FD_CLR(Server::current_fd, &Server::readfds);
                close(Server::current_fd);

                Server::_fds.erase(std::find(Server::_fds.begin(), Server::_fds.end(), Server::current_fd));
                Server::users_.erase(std::find(Server::users_.begin(), Server::users_.end(), Utils::find(Server::current_fd)));
                for (std::vector<Channel>::iterator it = Server::channels_.begin(); it != Server::channels_.end(); it++)
				{
					it_u = it->user_index(Server::current_fd);
					it_o = it->operator_index(Server::current_fd);
					if (it_u != it->get_users().end())
						it->get_users().erase(it_u);
					if (it_o != it->get_operator_list().end())
					{
						it->get_operator_list().erase(it_o);
						it_o = it->get_users().begin();
						if (it_o != it->get_users().end() && it->get_operator_list().size() == 0)
							it->get_operator_list().push_back(*it_o);
					}
					it_i = it->invite_index(Server::current_fd);
					if (it_i != it->get_invite_list().end())
						it->get_invite_list().erase(it_i);
				}

            } else {
                Server::received_bytes < BUFFER_SIZE ? Server::client_buffer[Server::received_bytes] = '\0' : Server::client_buffer[BUFFER_SIZE - 1] = '\0';
                for( std::vector<User>::iterator it = Server::users_.begin(); it != Server::users_.end(); ++it ) {
                    if ( it->getFd() == Server::current_fd ) {
                        it->getInput() += Server::client_buffer;
                        std::string userInput( Server::client_buffer );
                        curIndex = i;
                        if ( !userInput.empty() ) {
                            it->execute( userInput, &( *it ) );
                            break ; 
                        }
                        return ;
                    }
                }
            }
        }
    }
}

std::string Server::getPassword(void) {
    return Server::_password;
}
// Initialize static member variables
std::string Server::_password = ""; // Default password is an empty string
std::string Server::bufferStr = ""; // Default buffer string is an empty string
std::string Server::_hostName = ""; // Default host name is an empty string
char Server::client_buffer[BUFFER_SIZE] = {0}; // Initialize buffer array to zeros
char Server::c_hostName[MAX_HOST_NAME] = {0}; // Initialize host name buffer to zeros
int Server::serverSocket = -1; // Initialize the server socket to an invalid value
int Server::max_fds = -1; // Initialize max file descriptors to an invalid value
int Server::current_fd = -1; // Initialize current file descriptor to an invalid value
int Server::received_bytes = -1; // Initialize received_bytes to an invalid value
int Server::_port = -1; // Initialize the port to an invalid value
int Server::newSocket = -1; // Initialize newSocket to an invalid value
int Server::curIndex = -1; // Initialize curIndex to an invalid value
struct sockaddr_in Server::address; // Declare the address struct
fd_set Server::readfds; // Declare the file descriptor set for reading
std::vector<int> Server::_fds; // Initialize the vector of file descriptors
std::vector<User> Server::users_; // Initialize the vector of users
std::vector<Channel> Server::channels_; // Initialize the vector of channels
int Server::addrlen = sizeof(struct sockaddr_in); // Initialize addrlen with the size of the sockaddr_in structure
