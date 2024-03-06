#include "includes/User.hpp"

using namespace std;

// CONSTRUCTORS
User::User()
{

}

User::User(string username, string nickname)
{
    this->username_ = username;
    this->nickname_ = nickname;
    this->isRegistered_ = false;
    Server::users_.push_back(*this);
}

User::~User() {}

// OVERLOAD
bool    User::operator==(User const &rhs)
{
    if (this->nickname_ == rhs.getNickname() && this->username_ == rhs.getUsername())
        return true;
    return false;
}


// GETTERS
vector<Channel> User::getChannels() const { return this->channels_; }
string          User::getUsername() const { return this->username_; }
string          User::getNickname() const { return this->nickname_; }
int             User::getFd() const { return this->fd_; }
bool            User::isRegistered() const { return this->isRegistered_; }

// SETTERS
void    User::setUsername(string username) { this->username_ = username; }
void    User::setNickname(string nickname) { this->nickname_ = nickname; }
void    User::setRegistered(bool isRegistered) { this->isRegistered_ = isRegistered; }
void    User::setFd(int fd) { this->fd_ = fd; }



// METHODS
/** @brief Check if a user exists
 * 
 * @param nickname User to be checked
*/
bool	User::user_exists(User nickname)
{
	for (int i = 0; i < Server::users_.size(); i++)
	{
		if (Server::users_[i] == nickname)
			return true;
	}
	return false;
}

/** @brief Check if a user exists
 * 
 * @param nickname string to be checked
 * @return true if channel exists, false otherwise
*/
bool	User::user_exists(string nickname)
{
	for (int i = 0; i < Server::users_.size(); i++)
	{
		if (Server::users_[i].nickname_ == nickname)
			return true;
	}
	return false;
}
