#include "includes/Channel.hpp"
#include "includes/Utils.hpp"
#include "includes/User.hpp"

using namespace std;

// CONSTRUCOTS

/**
 * @brief Construct a new Channel:: Channel object
 * 
 * @param channel_name Channel name if provided, otherwise use ""
 * @param channelExists If a user tries to join a channel that does not exist, enter false so that the channel is not added to Server::channels_ and an error is returned
 */
Channel::Channel(string channel_name, bool channelExists)
{
	(void)channelExists;
	this->max_users_ = -1; // Max users are initialially unlimited, so it is set to -1
	this->channel_name_ = channel_name;
	this->channel_topic_ = "";
	this->password_ = "";
	this->type_ = channel_name[0]; // '#' for public, '&' for private
	this->mode_['i'] = false; // Invite only
	this->mode_['t'] = false; // Topic
	this->mode_['k'] = false; // Password
	this->mode_['o'] = false; // Operator privelege
	this->mode_['l'] = false; // User limit
	// if (channelExists == true)
	// 	Server::channels_.push_back(*this);
}

Channel::~Channel()
{
	// Delete ban_list
	// Delete invite_list
	// Delete operator_list

}

// GETTERS
int		Channel::get_max_users() const { return this->max_users_; }
char	Channel::get_type() const { return this->type_; }

string	Channel::get_channel_name() const { return this->channel_name_; }
string	Channel::get_channel_topic() const { return this->channel_topic_; }
string	Channel::get_password() const { return this->password_; }

vector<User>	&Channel::get_users() { return this->users_; }
vector<User>	&Channel::get_operator_list() { return this->operator_list_; }
vector<User>	&Channel::get_invite_list() { return this->invite_list_; }
vector<User>	&Channel::get_ban_list() { return this->ban_list_; }
map<char, bool>	&Channel::get_mode() { return this->mode_; }

// SETTERS
void	Channel::set_channel_topic(string topic) { this->channel_topic_ = topic; }
void	Channel::set_channel_password(string password) { this->password_ = password; }
void	Channel::set_max_users(int max_users) { this->max_users_ = max_users; }
void	Channel::set_type(char type) { this->type_ = type; }
void	Channel::set_channel_name(string channel_name) { this->channel_name_ = channel_name; }
void	Channel::set_mode(char mode, bool mode_state) { this->mode_[mode] = mode_state; }
void	Channel::set_topic(string topic) { this->channel_topic_ = topic; }

// OVERLOADS
bool Channel::operator==(const Channel& other) const { return (this->channel_name_ == other.get_channel_name()); }

// METHODS
/**
 * @brief Announce a message to all clients in a channel
 * 
 * @param user User announcing to channel
 * @param message Message to be sent to channel
 */
int	Channel::announce_channel(string message)
{
	for (size_t i = 0; i < this->users_.size(); i++)
		send(this->users_[i].getFd(), message.c_str(), message.length(), 0);
	return 0;
}

/** @brief Check if a channel exists
 * 
 * @param channel Channel to be checked
*/
bool	Channel::channel_exists(Channel channel)
{
	for (size_t i = 0; i < Server::channels_.size(); i++)
	{
		if (Server::channels_[i] == channel)
			return true;
	}
	return false;
}

/** @brief Check if a channel exists
 * 
 * @param channel string to be checked
 * @return true if channel exists, false otherwise
*/
bool	Channel::channel_exists(string channel)
{
	for (size_t i = 0; i < Server::channels_.size(); i++)
	{
		if (Server::channels_[i].channel_name_ == channel)
			return true;
	}
	return false;
}

/** @brief Find a user in current channel
 * 	@param users List of users in the channel
 * 	@param user User to be found
 */
int		Channel::find_user(vector<User> users, User user)
{
	for (size_t i = 0; i < users.size(); i++)
	{
		if (users[i] == user)
			return i;
	}
	return -1;
}

/** @brief Find a user in current channel
 * 
 * 	@param users List of users in the channel
 * 	@param user User(string) to be found
 * 	@return int
 */
int		Channel::find_user(vector<User> users, string user)
{
	for (size_t i = 0; i < users.size(); i++)
	{
		if (users[i].getNickname() == user)
			return i;
	}
	return -1;
}

/**
 * @brief 
 * 
 * @param fd 
 * @return size_t 
 */
// size_t	Channel::user_index(int fd)
// {
// 	size_t i = 0;
// 	for (; i < this->get_users().size(); i++)
// 	{
// 		if (this->get_users()[i].getFd() == fd)
// 			return i;
// 	}
// 	return -1;
// }

// /**
//  * @brief 
//  * 
//  * @param fd 
//  * @return size_t
//  */
// size_t	Channel::operator_index(int fd)
// {
// 	size_t i = 0;
// 	for (; i < this->get_operator_list().size(); i++)
// 	{
// 		if (this->get_operator_list()[i].getFd() == fd)
// 			return i;
// 	}
// 	return -1;
// }

// /**
//  * @brief 
//  * 
//  * @param fd 
//  * @return size_t
//  */
// size_t	Channel::invite_index(int fd)
// {
// 	size_t i = 0;
// 	for (; i < this->get_invite_list().size(); i++)
// 	{
// 		if (this->get_invite_list()[i].getFd() == fd)
// 			return i;
// 	}
// 	return -1;
// }

std::vector<User>::iterator &Channel::user_index(int fd)
{
	if (this->users_.size() > 0) {

		for (this->user_it = this->users_.begin(); this->user_it != this->users_.end(); user_it++)
		{
			if (user_it->getFd() == fd)
				return (user_it);
		}
	}
	return (user_it);
}

std::vector<User>::iterator &Channel::operator_index(int fd)
{
	for (this->operator_it = this->operator_list_.begin(); this->operator_it != this->operator_list_.end(); operator_it++)
	{
		if (this->operator_it->getFd() == fd)
			return (operator_it);
	}
	return (operator_it);
}

std::vector<User>::iterator &Channel::invite_index(int fd)
{
	for (this->invite_it = this->invite_list_.begin(); this->invite_it != this->invite_list_.end(); invite_it++)
	{
		if (this->invite_it->getFd() == fd)
			return (invite_it);
	}
	return (invite_it);
}

/** @brief Add a user to a channel
 * 
 * @param user User to be added
*/
void	Channel::addUser(const User &user) { this->users_.push_back(user); }

/** @brief Add an operator to a channel
 * 
 * @param user User to be added
*/
void	Channel::addOperator(const User &user) { this->operator_list_.push_back(user); }

/** @brief Add an invite to a channel
 * 
 * @param user User to be added
*/
void	Channel::addInvite(const User &user) { this->invite_list_.push_back(user); }

/** @brief Add a ban to a user from the channel
 * 
 * @param user User to be added
*/
void	Channel::addBan(const User &user) { this->ban_list_.push_back(user); }

/** @brief Remove a user from a channel
 * 
 * @param user User to be removed
*/
void	Channel::removeUser(const User &user) { this->users_.erase(this->users_.begin() + find_user(this->users_, user)); }

/** @brief Remove an operator from a channel
 * 
 * @param user User to be removed
*/
void	Channel::removeOperator(const User &user) { this->operator_list_.erase(this->operator_list_.begin() + find_user(this->operator_list_, user)); }

/**
 * @brief Remove an invitee from a channel's invite list
 * 
 * @param user User to be removed
 */
void	Channel::removeInvite(const User &user) { this->invite_list_.erase(this->invite_list_.begin() + find_user(this->invite_list_, user)); }

/** @brief Remove a banned user from a channel's banned list
 * 
 * @param user User to be banned
*/
void	Channel::removeBan(const User &user) { this->ban_list_.erase(this->ban_list_.begin() + find_user(this->ban_list_, user)); }

/**
 * @brief Remove a channel from Server::channels_ | Normally called when no users are left on channel
 * 
 * @param channel Channel(string) to be removed from Server::channels_
 */
void	Channel::removeChannel(string channel)
{
	for (size_t i = 0; i < Server::channels_.size(); i++)
		if (Server::channels_[i].channel_name_ == channel)
			Server::channels_.erase(Server::channels_.begin() + i);
	for (size_t i = 0; i < Server::users_.size(); i++)
		for (size_t j = 0; j < Server::users_[i].getChannels().size(); j++)
			if (Server::users_[i].getChannels()[j].get_channel_name() == channel)
				Server::users_[i].removeChannel(Server::users_[i].getChannels()[j]);
}
