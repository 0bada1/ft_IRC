#include "includes/User.hpp"
#include "includes/Channel.hpp"

using namespace std;

// CONSTRUCTORS
User::User() {}

User::User(int fd) : fd_(fd), isRegistered_(false), username_(""), nickname_("") {}

User::User(string username, string nickname)
{
    this->username_ = username;
    this->nickname_ = nickname;
    this->isRegistered_ = false;
    Server::users_.push_back(*this);
	Server::_fds.push_back(fd);
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
string			User::getInput() const { return this->input_; }
string          User::getUsername() const { return this->username_; }
string          User::getNickname() const { return this->nickname_; }
int             User::getFd() const { return this->fd_; }
bool            User::isRegistered() const { return this->isRegistered_; }

// SETTERS
void    User::setUsername(string username) { this->username_ = username; }
void    User::setNickname(string nickname) { this->nickname_ = nickname; }
void    User::setRegistered(bool isRegistered) { this->isRegistered_ = isRegistered; }
void    User::setFd(int fd) { this->fd_ = fd; }
void    User::addChannel(Channel channel) { this->channels_.push_back(channel); }
void	User::removeChannel(Channel channel)
{
	for (size_t i = 0; i < this->channels_.size(); i++)
	{
		if (this->channels_[i] == channel)
		{
			this->channels_.erase(this->channels_.begin() + i);
			break;
		}
	}
}

// METHODS
/** @brief Check if a user exists
 * 
 * @param nickname User to be checked
*/
bool	User::user_exists(User nickname)
{
	for (size_t i = 0; i < Server::users_.size(); i++)
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
	for (size_t i = 0; i < Server::users_.size(); i++)
	{
		if (Server::users_[i].nickname_ == nickname)
			return true;
	}
	return false;
}

/**
 * @brief 
 * 
 * @param cmd 
 * @param user 
 */
void	User::execute(string cmd, User *user)
{
	vector<string> splitmsg = Utils::split(cmd);

	if(this->isRegistered_ == false) {
		int user_flag = 0, nick_flag = 0, pass_flag = 0;
		for(size_t i = 0; splitmsg.size() > 0 && i < splitmsg.size(); i++)
		{
			if(splitmsg.at(i) == "USER" && user_flag == 1)
			{
				Utils::sendErrorMessage(user->getFd(), "Unknown command\n", 421);
				return ;
			}
			if(splitmsg.at(i) == "NICK" && nick_flag == 1)
			{
				Utils::sendErrorMessage(user->getFd(), "Unknown command\n", 421);
				return ;
			}
			if(splitmsg.at(i) == "PASS" && pass_flag == 1)
			{
				Utils::sendErrorMessage(user->getFd(), "Unknown command\n", 421);
				return ;
			}
			if(splitmsg.at(i) == "USER")
				user_flag = 1;
			if(splitmsg.at(i) == "NICK")
				nick_flag = 1;
			if(splitmsg.at(i) == "PASS")
				pass_flag = 1;
		}
	}

	if (!user_options(user, splitmsg))
		return ;
	if(!authorise(user, cmd))
	{
		if(splitmsg.size() > 0 && splitmsg.at(0) != "CAP"){
			if(pass_issue != 1 && alr_reg != 1)
			{
				string S = ERR_NOTREGISTERED_M;
				S.append(" You have not registered\n");
				send(user->getFd(), S.c_str(), strlen(S.c_str()), 0);
			}
			else
			{
				Utils::closeThis(*user);
				return ;
			}
		}
	}

	if(splitmsg.size() > 0 && splitmsg.at(0) == "NICK" && change_flag == false)
		change_user(user, splitmsg);

	if ((splitmsg.size() > 1 && splitmsg.at(0) == "CAP"))
	{
		if(splitmsg.size() >= 3 && splitmsg.at(1) == "LS" && splitmsg.at(2) == "302")
		{
			string S = "CAP * ACK :multi-prefix\r\n";
			send(user->getFd(), S.c_str(), strlen(S.c_str()), 0);
		}
		else if (splitmsg.size() >= 2 && splitmsg.at(1) == "LS")
		{
			string S = "CAP * ACK :multi-prefix\r\n";
			send(user->getFd(), S.c_str(), strlen(S.c_str()), 0);
		}
		else if (splitmsg.size() >= 3 && splitmsg.at(1) == "REQ" && splitmsg.at(2) == "multi-prefix")
		{
			string S = "CAP * ACK :multi-prefix\n";
			send(user->getFd(), S.c_str(), strlen(S.c_str()), 0);
		}
	}

	if(this->isRegistered_ == true)
	{
		user_cmds(user, splitmsg);
		change_flag = false;
	}

	return ;
}
