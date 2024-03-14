#include "includes/Server.hpp"
#include "includes/User.hpp"
#include "includes/Commands.hpp"

// int checkInput(int argc, char *argv[])
// {
//     if (argc != 3)
//     {
//         std::cerr << "Usage: " << argv[0] << " <port number> <password>" << std::endl;
//         return -1;
//     }
//     return 0;
// }

// int main(int argc, char *argv[])
// {
//     if (checkInput(argc, argv) < 0)
//         return -1;

//     Server server(std::atoi(argv[1]), argv[2]);
    
//     server.startServer(atoi(argv[1]), std::string(argv[2]));
//     return 0;
// }


#include "./includes/Server.hpp"

int main(int argc, char *argv[])
{
	if (argc != 3) {
		std::cerr << "Usage: ./ircserv [port] [PASS]" << std::endl;
		return (1);
	}

	int port_num = std::atoi(argv[1]);
	std::string	port(argv[1]), password(argv[2]);
	if (port.empty() || password.empty() || port_num > MAX_PORT \
		|| port.length() > 5 || port.find_first_not_of("0123456789") != std::string::npos)
	{
		std::cerr << "Error: invalid arguments !" << std::endl;
		return 1;
	}

	signal( SIGINT, Utils::signalHandler );
    signal( SIGQUIT, Utils::signalHandler );
	
	try
	{
		Server::_port = port_num;
		Server::_password = password;
		Server::openSocket();
		Server::run();
	} catch(const std::exception& e) {
		std::cerr << RED << "Exception: " << e.what() << RESET << '\n';
	}
	
	return 0;
}
