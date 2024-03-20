#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include "Server.hpp"

using namespace std;

class User;
class Channel;

namespace Utils {

	// ITERATORS
	// extern vector<Channel>::iterator it_channel;

	// Check if a user exists 
	User	find_User(const User &nickname);
	User	find_User(const string &nickname);
	bool	channel_exists(Channel channel);
	bool	channel_exists(string channel);
	Channel	&find_Channel(string channel);
	int		find_Channel(Channel channel);
	bool	username_exists(string username);
	bool	nickname_exists(string nickname);
	void	sendErrorMessage(int fd, const std::string& message, const int key);
	template <typename T>
	int binarySearch(std::vector<T> users, T nickname, unsigned int size);
    // std::string to_string(int value);
	vector<string> split(const string str);
	// std::string trim(std::string &str);
	void signalHandler(int signum);
	User &find(int fd);
	// void closeThis(User &user);
	void	removeClient(User &user);
	bool	profanitiesBot(vector<string> messages);
	string	concatenateStrings(const vector<string>& vec);

	void	printChannelList();
	void	printUserList();
}

# endif // UTILS_HPP
