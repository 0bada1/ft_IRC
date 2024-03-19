#include "includes/User.hpp"
#include "includes/Channel.hpp"
#include "includes/Parse.hpp"

using namespace std;

// CONSTRUCTORS
User::User() {}


// User::User(int fd) : _fd(fd), isAuth(false), isOperator(false), nickname_(""), username_(""), _cmd(2)
// {
// 	isRegistered_ = false;
// 	change_flag = false;
// 	pass_issue = 0;
// 	alr_reg = 0;
// 	input_ = "";
// }

User::User(int fd) : nick_flag(0), user_flag(0), pass_flag(0), isAuth(false), _cmd(2), alr_reg(0), fd_(fd), isRegistered_(false), username_(""), nickname_("") {}

User::User(int fd, string username, string nickname) : nick_flag(0), user_flag(0), pass_flag(0), _cmd(2), fd_(fd)
{
    this->username_ = username;
    this->nickname_ = nickname;
    this->isRegistered_ = false;
	this->isAuth= false;
	this->alr_reg = 0;
    // Server::users_.push_back(*this);
	// Server::_fds.push_back(fd);
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
void	User::setInput(string input){ this->input_ = input; }
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
void	User::closeFd() { close(this->fd_); }

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

bool	User::parse_cmd(std::string str)
{
	std::vector<std::string> vector =  Utils::split(str);

	for(std::vector<std::string>::iterator it = vector.begin(); it != vector.end();)
	{
		if(*it == "USER")
		{
			if(*it == vector.back())
				return false;
			else
				_cmd[0] = *(++it);
		}
		if(*it == "NICK")
		{
			if(*it == vector.back())
				return false;
			else {
				// std::cout << "------> " << *(it + 1) << std::endl;
				_cmd[1] = *(++it);
			}
		}
		if(*it == "PASS")
		{
			if(*it == vector.back())
				return false;
			else
				pass = *(++it);
			if (*(it) != vector.back())
			{
				if(*(it + 1) != "NICK" && *(it + 1) != "USER")
				{
					std::string S = "464";
					S.append(" : Password incorrect");
					send(this->fd_, S.c_str(), strlen(S.c_str()), 0);
					pass_issue = 1;
					return false;
				}
			}
		}
		else
			++it;
	}

	return true;
}

int User::authorise(User *user, std::string cmd)
{
	if(user->isAuth == true)
		return 2;
	if (parse_cmd(cmd))
	{
		if(!isRegistered_)
		{
			for(std::vector<User>::iterator it = Server::users_.begin(); it != Server::users_.end(); ++it)
			{
				if(_cmd[0] != "" && _cmd[1] != "" && pass != ""){
					if (_cmd.size() > 1 && it->isRegistered_  && (it->nickname_ == _cmd[1]|| it->username_ == _cmd[0]) && !isAuth)
					{
						std::string S = ERR_ALREADYREGISTRED_M;
						S.append(" User already registered\n");
						send(user->fd_, S.c_str(), strlen(S.c_str()), 0);
						alr_reg = 1;
						return 0;
					}
				}
			}
		}
		user->nickname_ = _cmd[1];
		user->username_ = _cmd[0];
		if(user->nickname_ != "" && user->username_ != "" && pass == Server::getPassword() && !isRegistered_)
		{
			std::string wlcmMsg = ":irc 001 " + user->nickname_ + " :Welcome to FT_IRC, " + user->username_ + "@" + Server::_hostName + "\n"
								  ":irc 002 " + user->nickname_ + " :Host is " + Server::_hostName + ", running version 1.0\n"
								  ":irc 003 " + user->nickname_ + " :Created in 42 labs at July\n";
			send(user->fd_, wlcmMsg.c_str(), strlen(wlcmMsg.c_str()), 0);
			if(pass == Server::getPassword()){
				user->isAuth = true;
				isRegistered_ = true;
			}
		}

		if(pass != "")
		{
			if(this->pass != Server::getPassword())
				{
					std::string S = "464";
					S.append(" : Password incorrect");
					send(this->fd_, S.c_str(), strlen(S.c_str()), 0);
					pass_issue = 1;
					return false;
				}
		}
		change_flag	= true;
		return 1;
	}
	return 0;
}

bool	User::user_options(User *user, std::vector<std::string> splitmsg) // CHANGE
{
	if (splitmsg.size() > 0 && (splitmsg.at(0) == "quit"))
	{
		Utils::removeClient(*user);
		return false;
	}
	else if (splitmsg.size() > 0 && splitmsg.at(0) == "PASS") {
			 if (user->isAuth == true) {
				send(user->fd_, "462 :You may not reregister\r\n", 30, 0);
    	}
	}
	return true;
}

void User::change_user(User *user, std::vector<std::string> splitmsg) // CHANGE
{
	std::string old_name;
	if(!this->isAuth)
		return ;

	 if (splitmsg.size() < 2) {
		std::string S = "431";
		S.append(" :No nickname given\r\n");
        send(this->fd_, S.c_str(), strlen(S.c_str()), 0);
        return;
    }
	if (splitmsg.size() == 2 && splitmsg.at(0) == "NICK")
	{
		for(std::vector<User>::iterator it = Server::users_.begin(); it != Server::users_.end(); ++it)
		{
			if (it->nickname_ == splitmsg.at(1))
			{
				std::string S = "433";
				S.append(" : Nickname is already in use\n");
				send(user->fd_, S.c_str(), strlen(S.c_str()), 0);
				return ;
			}
		}
		old_name = user->nickname_;
		user->nickname_ = splitmsg.at(1);
		for (std::vector<Channel>::iterator it = Server::channels_.begin(); it != Server::channels_.end(); it++)
		{
			for (it_u = it->get_users().begin(); it_u != it->get_users().end(); it_u++)
				if (it_u->nickname_ == old_name)
					it_u->nickname_ = splitmsg.at(1);
			for (it_u = it->get_operator_list().begin(); it_u != it->get_operator_list().end(); it_u++)
				if (it_u->nickname_ == old_name)
					it_u->nickname_ = splitmsg.at(1);
			for (it_i = it->get_invite_list().begin(); it_i != it->get_invite_list().end(); it_i++)
				if (it_i->nickname_ == old_name)
					it_i->nickname_ = splitmsg.at(1);
		}
	}
	else
	{
		std::string S = ERR_NEEDMOREPARAMS_M;
		S.append(" :Not enough parameters\r\n");
		send(this->fd_, S.c_str(), strlen(S.c_str()), 0);
		return;
	}
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
				Utils::removeClient(*user);
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

	if(this->isAuth == true)
	{
		parse(*user, cmd);
		change_flag = false;
	}

	return ;
}
