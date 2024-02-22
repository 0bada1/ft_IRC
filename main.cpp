#include "includes/Server.hpp"
#include "includes/Users.hpp"
#include "includes/Commands.hpp"

int checkInput(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <port number> <password>" << std::endl;
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (checkInput(argc, argv) < 0)
        return -1;

    Server server(std::atoi(argv[1]), argv[2]);
    
    server.startServer(atoi(argv[1]), std::string(argv[2]));
    return 0;
}