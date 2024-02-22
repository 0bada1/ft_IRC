// #include "includes/Server.hpp"
#include "includes/Server.hpp"

Server::Server(int port, std::string password)
{
    this->port_ = port;
    this->password_ = password;
    this->maxClients_ = FD_SETSIZE;
}

Server::~Server() {}

// Prototype server for now
int Server::startServer(int port, std::string password)
{

    (void)password;

    // Create a socket
    if ((Server::serverSocket_ = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    // Set server address
    Server::serverAddress_.sin_family = AF_INET;
    Server::serverAddress_.sin_addr.s_addr = INADDR_ANY;
    Server::serverAddress_.sin_port = htons(port);

    // Bind the socket
    if (bind(Server::serverSocket_, (struct sockaddr *)&Server::serverAddress_, sizeof(Server::serverAddress_)) < 0)
    {
        std::cerr << "Error binding socket" << std::endl;
        return -1;
    }

    // Listen for connections
    if (listen(Server::serverSocket_, 3) < 0)
    {
        std::cerr << "Error listening for connections" << std::endl;
        return -1;
    }

    // gethostname(Server::char_hostname_, MAX_HOST_NAME);
    // Server::hostName_ = Server::char_hostname_;
    // std::cout << "Server started on port " << port << ": " << gethostname(Server::hostname, MAX_HOST_NAME) std::endl;
    std::cout << "Server started on port " << port << std::endl;
    std::cout << "Waiting for connections..." << std::endl;
    return 0;
}

// To be completed by Mutasem
// int Server::runServer()
// {

// }