#ifndef SERVER_HPP
#define SERVER_HPP

#include "User.hpp"
#include "Channel.hpp"
#include "Utils.hpp"
#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <fcntl.h>
#include <cerrno>
#include <iomanip>
#include <signal.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>

#define MAX_PORT UINT16_MAX
#define MAX_BUFFER 1024
#define MAX_HOST_NAME 512
#define CYAN "\033[36m"
#define YELLOW "\033[33m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define RESET "\033[0m"

// Server error codes (RFC 2812 section 5.2)
#define NO_SUCHSERVER			" :No such server" // 402
#define ERR_NOPRIVILEGES 		":Permission Denied- You're not an IRC operator" // 481	// Any command requiring operator privileges to operate must return this error to indicate the attempt was unsuccessful.

class User;
class Channel;
class Server
{
private:
	Server(void);

public:
	static const int MAX_CLIENTS = FD_SETSIZE;
	static const int BUFFER_SIZE = MAX_BUFFER;
	static std::string _password;
	static std::string _hostName;
	static std::string bufferStr;
	static char client_buffer[MAX_BUFFER];
	static char c_hostName[MAX_HOST_NAME];
	static int serverSocket;
	static int max_fds;
	static int current_fd;
	static int received_bytes;
	static int _port;
	static int newSocket;
	static int curIndex;
	static int addrlen;
	static struct sockaddr_in address;
	static fd_set readfds;
	class ServerException : public std::exception
	{
		private:
			std::string _msg;
		public:
			ServerException(std::string msg) : _msg(msg) {}
			virtual ~ServerException() throw() {}
			virtual const char *what() const throw() { return _msg.c_str(); }
	};
	static std::vector<int> _fds;
	static std::vector<User> users_;
	static std::vector<Channel> channels_;
	static void openSocket(void);
	static void run(void);
	static void acceptConnection(void);
	static void handleClientMessages(void);
	static void showUsers(void);
	static void showChannels(void);
	static std::string getPassword(void);
};

#endif
