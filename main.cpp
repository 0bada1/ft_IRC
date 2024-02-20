#include "Server.hpp"

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
    return 0;
}