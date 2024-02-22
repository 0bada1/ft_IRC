#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/types.h>
#include <fcntl.h>
#include <cerrno>
#include <iomanip>
#include <signal.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>
#include <exception>

#define MAX_HOST_NAME 512

using namespace std;
class Server
{
    public:
        // CONSTRUCTORS
        Server(int port, string password);
        ~Server();

        // METHODS
        int startServer(int port, string password);
        // int runServer(); // To be completed by Mutasem

        // GETTERS
        int getServerSocket() const;
        int getPort() const;


        private:
            static int     serverSocket_;
            static int     port_;
            static string  password_;
            static struct  sockaddr_in serverAddress_;
            static int     maxClients_;
            static string  hostName_;
            static char    char_hostName_[MAX_HOST_NAME];

};

# endif // SERVER_HPP