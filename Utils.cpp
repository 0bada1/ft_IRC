#include "includes/Utils.hpp"
#include "includes/User.hpp"

using namespace std;

template <typename T>
int Utils::binarySearch(vector<T> users, T nickname, unsigned int size)
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

/**
 * @brief Sends an error message to the client as follows\n
 * <key> ERROR: <message>
 * 
 * @param fd 
 * @param message 
 * @param key 
 * @return int 
 */
void	Utils::sendErrorMessage(int fd, const string& message, const int key) {
	string errorMsg = key + " ERROR: " + message;
	send(fd, errorMsg.c_str(), strlen(errorMsg.c_str()), 0);
}

User	Utils::find_User(string nickname)
{
	for (size_t i = 0; i < Server::users_.size(); i++)
	{
		if (Server::users_[i].getNickname() == nickname)
			return Server::users_[i];
	}
	return User();
}

User	Utils::find_User(User nickname)
{
	for (size_t i = 0; i < Server::users_.size(); i++)
	{
		if (Server::users_[i] == nickname)
			return Server::users_[i];
	}
	return User();
}

Channel	Utils::find_Channel(string channel)
{
	for (size_t i = 0; i < Server::channels_.size(); i++)
	{
		if (Server::channels_[i].get_channel_name() == channel)
			return Server::channels_[i];
	}
	return Channel("", false);
}

int	Utils::find_Channel(Channel channel)
{
	for (size_t i = 0; i < Server::channels_.size(); i++)
	{
		if (Server::channels_[i] == channel)
			return i;
	}
	return -1;
}

bool	Utils::username_exists(string username)
{
	for (size_t i = 0; i < Server::users_.size(); i++)
	{
		if (Server::users_[i].getUsername() == username)
			return true;
	}
	return false;
}

bool	Utils::nickname_exists(string nickname)
{
	for (size_t i = 0; i < Server::users_.size(); i++)
	{
		if (Server::users_[i].getNickname() == nickname)
			return true;
	}
	return false;
}

/** @author Mutasem mmajid-m */
#include "includes/Server.hpp"
#include "includes/Commands.hpp"

User &Utils::find(int fd) {
	for(vector<User>::iterator it = Server::users_.begin(); it != Server::users_.end(); ++it) {
        if (it->_fd == fd) {
			return *it;
        }
    }
	throw Server::ServerException("Utils::find: User not found");
}

void Utils::signalHandler(int signum) {

    cout << RED << "Interrupt signal (" << signum << ") received." << RESET << "\n";

    for(vector<int>::iterator it = Server::_fds.begin(); it != Server::_fds.end(); ++it) {
            close(*it);
    }
	shutdown(Server::serverSocket, SHUT_RDWR);
    close(Server::serverSocket);
	Server::_fds.clear();
	Server::users_.clear();
	Server::channels_.clear();
    exit(signum);
}

// void Utils::closeThis(User &user)
// {
// 	close(user._fd);
// 	vector<User>::iterator it_u;
// 	vector<User>::iterator it_o;
// 	vector<User>::iterator it_i;

//     Server::_fds.erase(find(Server::_fds.begin(), Server::_fds.end(), user._fd));	
//     Server::users_.erase(find(Server::users_.begin(), Server::users_.end(), user));
// 	for (vector<Channel>::iterator it = Server::channels_.begin(); it != Server::channels_.end(); it++)
// 	{
// 		it_u = it->user_in_chan(Server::sd);
// 		it_o = it->op_in_chan(Server::sd);
// 		if (it_u != it->users.end())
// 			it->users.erase(it_u);
// 		if (it_o != it->operators.end())
// 		{
// 			it->operators.erase(it_o);
// 			it_o = it->users.begin();
// 			if (it_o != it->users.end() && it->operators.size() == 0)
// 				it->operators.push_back(*it_o);
// 		}
// 		it_i = it->inv_in_chan(Server::sd);
// 		if (it_i != it->invites.end())
// 			it->invites.erase(it_i);
// 	}
// }


// string Utils::to_string(int value) {
//     stringstream ss;
// 	ss << value;
// 	string s = ss.str();
// 	return s;
// }
// vector<string> Utils::split(const string str) {
// 	vector<string> vector;
// 	istringstream iss(str);
// 	string cmd;
// 	while (iss >> skipws >> cmd)
// 		vector.push_back(cmd);
// 	return vector;
// }
// string Utils::trim(string &str) {
//     size_t left = str.find_first_not_of('\n');
//     size_t right = str.find_last_not_of('\n');
//     if (left == string::npos || right == string::npos)
//         return "";
//     return str.substr(left, right - left + 1);
// }

// void sendErrorMessage(int fd, const string& message, const string& key) {
//     string errorMsg = key + " ERROR: " + message;
//     send(fd, errorMsg.c_str(), strlen(errorMsg.c_str()), 0);
// }

// void handleJoinCommand(const vector<string>& splitmsg, Command& cmd, User* user) {
//     if (splitmsg.size() == 2) {
//         cmd.join(splitmsg.at(1), "", *user);
//     } else if (splitmsg.size() == 3){
// 		cmd.join(splitmsg.at(1), splitmsg.at(2), *user);
// 	}
// 	else {
// 		return ;
//     }
// }

// void handleKickCommand(const vector<string>& splitmsg, Command& cmd, User* user) {
//     if (splitmsg.size() >= 3) {
//         cmd.kick(splitmsg.at(1), splitmsg.at(2), splitmsg, *user);
//     } else {
//         sendErrorMessage(user->_fd, "KICK command requires 3 or 4 arguments\n", TOO_MANY_ARGS);
//     }
// }

// void handlePrivMsgCommand(const vector<string>& splitmsg, Command& cmd, User* user) {
// 	if (splitmsg.size() >= 3) {
// 		cmd.privmsg(splitmsg.at(1), splitmsg, *user); // second argument will be the split message for mutiple words
// 	} else if (splitmsg.size() == 2) {
// 		// no such nickname, if nickname doesn't exist
// 		sendErrorMessage(user->_fd, "PRIVMSG command requires atleast 3 arguments\n", PRIVMSG_EMPTY);
// 	} else if(splitmsg.size() == 1) {
// 		sendErrorMessage(user->_fd, "PRIVMSG command requires atleast 3 arguments\n", ERR_NOSUCHNICK);
// 	} else { // if PRIVMSG nickname exist and msg dosent exist
// 		string S = ERR_NEEDMOREPARAMS;
// 		S.append(" :Not enough parameters\r\n");
// 		send(user->_fd, S.c_str(), strlen(S.c_str()), 0);
// 		return;
// 	}
// }

// void handleNoticeCommand(const vector<string>& splitmsg, Command& cmd, User* user) {
// 	if (splitmsg.size() >= 3) {
// 		cmd.notice(splitmsg.at(1), splitmsg, *user); // second argument will be the split message for mutiple words
// 	}
// }

// void	handleInviteCommand(const vector<string>& splitmsg, Command& cmd, User* user)
// {
// 	if (splitmsg.size() == 3) {
// 		cmd.invite(splitmsg.at(1), splitmsg.at(2), *user);
// 	} else {
// 		sendErrorMessage(user->_fd, "INVITE command requires 3 arguments\n", TOO_MANY_ARGS);
// 	}
// }

// void	handleTopicCommand(const vector<string>& splitmsg, Command& cmd, User* user)
// {
// 	if (splitmsg.size() == 3) {
// 		cmd.topic(splitmsg.at(1), splitmsg.at(2), *user);
// 	}else if (splitmsg.size() == 2){ 
// 		cmd.topic(splitmsg.at(1), "", *user);
// 	}else {
// 		sendErrorMessage(user->_fd, "INVITE command requires 1 or 2 arguments\n", TOO_MANY_ARGS);
// 	}
// }

// void handleWhoisCommand(const vector<string>& splitmsg, Command& cmd, User* user) {
// 	(void)cmd;
// 	if (splitmsg.size() == 2) {
// 		string nick = "name : " + splitmsg.at(1) + "\r\n";
// 		send(user->_fd, nick.c_str(), strlen(nick.c_str()), 0);
// 	} else {
// 		sendErrorMessage(user->_fd, "WHOIS command requires 2 arguments\n", TOO_MANY_ARGS);
// 	}
// }

// void handleModeCommand(const vector<string>& splitmsg, Command& cmd, User* user)
// {
// 	if (splitmsg.size() == 3){
// 		cmd.mode(splitmsg.at(1), splitmsg.at(2), *user, "");
// 	}
// 	else if(splitmsg.size() == 4){
// 		cmd.mode(splitmsg.at(1), splitmsg.at(2), *user, splitmsg.at(3));
// 	}
// 	else {
// 		sendErrorMessage(user->_fd, "MODE command requires 2 or 3 arguments\n", TOO_MANY_ARGS);
// 	}
		
// }

// void handlePartCommand(const vector<string>& splitmsg, Command& cmd, User* user)
// {
// 	if (splitmsg.size() == 2){
// 		cmd.part(splitmsg.at(1), *user);
// 	}
// 	else {
// 		sendErrorMessage(user->_fd, "PART command requires 1 argument\n", TOO_MANY_ARGS);
// 	}
// }

User &Utils::find(int fd) {
	for(vector<User>::iterator it = Server::users_.begin(); it != Server::users_.end(); ++it) {
        if (it->getFd() == fd) {
			return *it;
        }
    }
	throw Server::ServerException("Utils::find: User not found");
}

void Utils::signalHandler(int signum) {

    cout << RED << "Interrupt signal (" << signum << ") received." << RESET << "\n";

    for(vector<int>::iterator it = Server::_fds.begin(); it != Server::_fds.end(); ++it) {
            close(*it);
    }
	shutdown(Server::serverSocket, SHUT_RDWR);
    close(Server::serverSocket);
	Server::_fds.clear();
	Server::users_.clear();
	Server::channels_.clear();
    exit(signum);
}
