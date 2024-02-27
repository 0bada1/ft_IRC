#include "includes/Channel.hpp"
#include "Utils.hpp"

using namespace std;

// CONSTRUCOTS
/* Channel::Channel()
{
	this->max_users_ = -1; // Max users are initialially unlimited, so it is set to -1
	this->channel_name_ = "";
	this->channel_topic_ = "";
	this->password_ = "";
	this->type_ = '#';
	this->mode_['i'] = false;
	this->mode_['t'] = false;
	this->mode_['k'] = false;
	this->mode_['o'] = false;
	this->mode_['l'] = false;
} */

Channel::Channel(string channel_name, char type)
{
	this->max_users_ = -1; // Max users are initialially unlimited, so it is set to -1
	this->channel_name_ = channel_name;
	this->channel_topic_ = "";
	this->password_ = "";
	this->type_ = type; // '#' for public, '&' for private
	this->mode_['i'] = false;
	this->mode_['t'] = false;
	this->mode_['k'] = false;
	this->mode_['o'] = false;
	this->mode_['l'] = false;
}

Channel::~Channel()
{
	// Delete ban_list
	// Delete invite_list
	// Delete operator_list

}

// GETTERS
int	Channel::get_max_users() const { return this->max_users_; }
int	Channel::get_type() const { return this->type_; }

string	Channel::get_channel_name() const { return this->channel_name_; }
string	Channel::get_channel_topic() const { return this->channel_topic_; }
string	Channel::get_password() const { return this->password_; }

vector<User>	Channel::get_users() const { return this->users_; }
vector<User>	Channel::get_operator_list() const { return this->operator_list_; }
vector<User>	Channel::get_invite_list() const { return this->invite_list_; }
vector<User>	Channel::get_ban_list() const { return this->ban_list_; }

// METHODS

// int Channel::create_channel(string channel_name)
// {
// 	if (channel_name == "" || channel_name[0] != '#' || channel_name[0] != '&')
// 	{
// 		cerr << "Error 404: JOIN #<channel>" << endl;
// 		return -1;
// 	}

// 	cout << "Channel " << channel_name << " has been created" << endl;
// 	return 0;
// }

// OVERLOADS
bool Channel::operator==(const Channel& other) const { return (this->channel_name_ == other.get_channel_name()); }

// METHODS
/** @brief Check if a channel exists
 * 
 * @param channel Channel to be checked
*/
bool	Channel::channel_exists(Channel channel)
{
	for (int i = 0; i < Server::_channels.size(); i++)
	{
		if (Server::_channels[i] == channel)
			return true;
	}
	return false;
}

/** @brief Check if a channel exists
 * 
 * @param channel string to be checked
*/
bool	Channel::channel_exists(string channel)
{
	for (int i = 0; i < Server::_channels.size(); i++)
	{
		if (Server::_channels[i].channel_name_ == channel)
			return true;
	}
	return false;
}

// Search for a user in a channel
int		Channel::find_user(vector<User> users, User user)
{
	for (int i = 0; i < users.size(); i++)
	{
		if (users[i] == user)
			return i;
	}
	return -1;
}

int		Channel::find_user(vector<User> users, string user)
{
	for (int i = 0; i < users.size(); i++)
	{
		if (users[i].getNickname() == user)
			return i;
	}
	return -1;
}

/** @brief Remove a user from a channel
 * 
 * @param user User to be removed
*/
int	Channel::removeUser(string user)
{
	this->users_.erase(this->users_.begin() + find_user(this->users_, user)); // find_user() needs string overload
}

/** @brief Sets the mode of the channel
 * 
 * @param channel channel to be set
 * @param mode mode to be set for the channel (i, t, k, o, l)
 * @param mode_state state of the mode (true or false)
*/
int Channel::set_Channel_Modes(Channel channel, char mode, bool mode_state)
{
	if (mode != 'i' || mode != 't' || mode != 'k' || mode != 'o' || mode != 'l')
	{
		cerr << "Error 404: " << mode << "does not exist" << endl; // Wrong error code, update it
		return 404; // Wrong error code
	}

	if (channel_exists(channel))
	{
		channel.mode_[mode] = mode_state;
	}
	else
	{
		cerr << "Error 404: Channel " << channel.get_channel_name() << " does not exist" << endl; // Wrong error code, update it
		return 404; // Wrong error code
	}
	cout << "Channel " << channel << "modes have been set to " << modes << endl;
	return 0;
}
