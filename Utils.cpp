#include "includes/Utils.hpp"
#include "includes/Users.hpp"

int	Utils::binarySearch(std::vector<std::string> users, std::string nickname, unsigned int size)
{
	int low = 0;
	int high = size - 1;
	int mid;

	while (high >= low)
	{
		mid = (high + low) / 2;
		if (users[mid] == nickname)
			return mid;
		else if (users[mid] < nickname)
			low = mid + 1;
		else if (users[mid] > nickname)
			high = mid - 1;
	}
	return -1;
}

int	Utils::user_exists(std::string nickname)
{
	if (binarySearch(Users::nickname_, nickname, Users::userList_.size()) != -1)
		return 0;
	return -1;
}

User &Utils::find(int fd) {
	for(std::vector<User>::iterator it = Server::_users.begin(); it != Server::_users.end(); ++it) {
        if (it->_fd == fd) {
			return *it;
        }
    }
	throw Server::ServerException("Utils::find: User not found");
}

void Utils::signalHandler(int signum) {

    std::cout << RED << "Interrupt signal (" << signum << ") received." << RESET << "\n";

    for(std::vector<int>::iterator it = Server::_fds.begin(); it != Server::_fds.end(); ++it) {
            close(*it);
    }
	shutdown(Server::serverSocket, SHUT_RDWR);
    close(Server::serverSocket);
	Server::_fds.clear();
	Server::_users.clear();
	Server::_channels.clear();
    exit(signum);
}
