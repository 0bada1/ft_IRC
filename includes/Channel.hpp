
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream> // already included in Server.hpp
#include <sstream>
#include <map>
#include <vector> // already included in Server.hpp
#include "User.hpp"
#include "Server.hpp"

// Colors
#define GREEN_LIGHT "\033[1;38:5:150m"

// Channel modes
// #define INVITE_ONLY_CHANNEL 50
// #define OPEN_CHANNEL 51
// #define 

// Channel error messages  (RFC 2812 section 5.2) _M = MESSAGE
#define ERR_NOSUCHNICK_M 		" :No such nick/channel\n" // 401				Used to indicate the nickname parameter supplied to a command is currently unused.
#define ERR_NOSUCHCHANNEL_M 	" :No such channel\n" // 403					Used to indicate the server name given currently does not exist.
#define ERR_TOOMANYCHANNELS_M 	" :You have joined too many channels\n" // 405	Sent to a user when they have joined the maximum number of allowed channels and they try to join another channel.
#define ERR_USERNOTINCHANNEL_M	" :They aren't on that channel\n" // 441		Returned by the server to indicate that the target user of the command is not on the given channel.
#define ERR_NOTONCHANNEL_M		" :You're not on that channel\n" // 442			Returned by the server to indicate that the target user of the command is not on the given channel.
#define ERR_USERONCHANNEL_M		" :is already on channel" // 443				Returned when a client tries to invite a user to a channel they are already on.
#define ERR_NOTREGISTERED_M		" :You have not registered\n" // 451			Returned by the server to indicate that the client MUST be registered before the server will allow it to be parsed in detail.
#define ERR_NEEDMOREPARAMS_M	" :Not enough parameters\n" // 461				Returned by the server by numerous commands to indicate to the client that it didn't supply enough parameters.
#define ERR_CHANNELISFULL_M 	" :Channel is full\n" // 471					Sent to a user who is trying to join a channel which is set +l and is already full.
#define ERR_UNKNOWNMODE_M		" :is unknown mode char to me for " // 472		Used to indicate that a given mode char is unknown.
#define ERR_INVITEONLYCHAN_M 	" :Cannot join channel (+i)\n" // 473				Sent to a user trying to join a channel which is invite-only (+i) without an invitation.
#define ERR_BANNEDFROMCHAN_M 	" :Cannot join channel (+b)\n" // 474			Sent to a user trying to join a channel where the user is banned.
#define ERR_BADCHANNELKEY_M 	" :Cannot join channel (+k)" // 475				Sent to a user trying to join a channel with a key when the channel has been set +k (or mode +k).
#define ERR_CHANOPRIVSNEEDED_M	" :You're not channel operator" // 482			Any command requiring 'chanop' privileges (such as MODE messages) MUST return this error if the client making the attempt is not a chanop on the specified channel.

// Channel error codes (RFC 2812 section 5.2) _C = CODE
#define ERR_NOSUCHNICK_C 		401	// " :No such nick/channel\n"				Used to indicate the nickname parameter supplied to a command is currently unused.
#define ERR_NOSUCHCHANNEL_C 	403	// " :No such channel\n"					Used to indicate the server name given currently does not exist.
#define ERR_TOOMANYCHANNELS_C 	405	// " :You have joined too many channels\n"	Sent to a user when they have joined the maximum number of allowed channels and they try to join another channel.
#define ERR_USERNOTINCHANNEL_C	441	// " :They aren't on that channel\n"		Returned by the server to indicate that the target user of the command is not on the given channel.
#define ERR_NOTONCHANNEL_C		442 // " :You're not on that channel\n"			Returned by the server to indicate that the target user of the command is not on the given channel.
#define ERR_USERONCHANNEL_C		443 // " :is already on channel"				Returned when a client tries to invite a user to a channel they are already on.
#define ERR_NOTREGISTERED_C		451	// " :You have not registered\n"			Returned by the server to indicate that the client MUST be registered before the server will allow it to be parsed in detail.
#define ERR_NEEDMOREPARAMS_C	461 // " :Not enough parameters\n"				Returned by the server by numerous commands to indicate to the client that it didn't supply enough parameters.
#define ERR_CHANNELISFULL_C 	471	// " :Channel is full\n"					Sent to a user who is trying to join a channel which is set +l and is already full.
#define ERR_UNKNOWNMODE_C		472	// " :is unknown mode char to me for "		Used to indicate that a given mode char is unknown.
#define ERR_INVITEONLYCHAN_C 	473	// " :Cannot join channel (+i)\n"			Sent to a user trying to join a channel which is invite-only (+i) without an invitation.
#define ERR_BANNEDFROMCHAN_C 	474	// " :Cannot join channel (+b)\n"			Sent to a user trying to join a channel where the user is banned.
#define ERR_BADCHANNELKEY_C 	475	// " :Cannot join channel (+k)"				Sent to a user trying to join a channel with a key when the channel has been set +k (or mode +k).
#define ERR_CHANOPRIVSNEEDED_C	482	// " :You're not channel operator"			Any command requiring 'chanop' privileges (such as MODE messages) MUST return this error if the client making the attempt is not a chanop on the specified channel.

// Other RFC messages (RFC 2812 section 5.2)
#define  RPL_NOTOPIC_M			" :No topic is set\n" // 331					When sending a TOPIC message to determine the channel topic, one of two replies is sent.  If the topic is set, RPL_TOPIC is sent back else RPL_NOTOPIC.

// Other RFC codes (RFC 2812 section 5.2)
#define  RPL_NOTOPIC_C			331 //" :No topic is set\n"						When sending a TOPIC message to determine the channel topic, one of two replies is sent.  If the topic is set, RPL_TOPIC is sent back else RPL_NOTOPIC.

// #define NOT_OPERATOR_ERROR	"Error: You're not an operator of the channel\n"
// #define NO_USER_ERROR 		"Error: No such username\n"
// #define NO_CHANNEL_ERROR 	"Error: No such channel\n"
// #define INVITE_ONLY_CHANNEL	"Error: This channel is invite only\n"
// // #define 

using namespace std;
class User;

class Channel
{
	public:
		// CONSTRUCTORS
		Channel(string channel_name, bool channelExists);
		~Channel();

		// OPERATOR OVERLOADS
		bool operator==(const Channel& other) const;

		// ITERATORS
		vector<User>::iterator	user_it;
		vector<User>::iterator	operator_it;
		vector<User>::iterator	invite_it;

		// METHODS
		// int	create_channel(string channel_name);
		// int		set_Channel_Modes(Channel channel, int mode, bool mode_state);
		int						announce_channel(string message);
		bool					channel_exists(Channel channel);
		bool					channel_exists(string channel);
		int						find_user(vector<User> users, User user); // overload to search by string (DONE)
		int						find_user(vector<User> users, string user);
		vector<User>::iterator	&user_index(int fd);
		vector<User>::iterator	&operator_index(int fd);
		vector<User>::iterator	&invite_index(int fd);
		// size_t					user_index(int fd);
		// size_t					operator_index(int fd);
		// size_t					invite_index(int fd);
		void					addUser(const User &user);
		void					addOperator(const User &user);
		void					addInvite(const User &user);
		void					addBan(const User &user);
		void					removeUser(const User &user);
		void					removeOperator(const User &user);
		void					removeInvite(const User &user);
		void					removeBan(const User &user);
		void					removeChannel(string channel);

		// GETTERS
		int				get_max_users() const;
		char			get_type() const;
		string			get_channel_name() const;
		string			get_channel_topic() const;
		string			get_password() const;
		vector<User>	&get_users();
		vector<User>	&get_operator_list();
		vector<User>	&get_invite_list();
		vector<User>	&get_ban_list();
		map<char, bool>	&get_mode();

		// SETTERS
		void	set_channel_topic(string topic);
		void	set_channel_password(string password);
		void	set_max_users(int max_users);
		void	set_type(char type);
		void	set_channel_name(string channel_name);
		void	set_mode(char mode, bool mode_state);
		void	set_topic(string topic);

	private:
		unsigned int		max_users_;			// Max users allowed in channel, initially unlimited
		char				type_;				// '#' for public, '&' for private
		string				channel_name_;		// Channel name
		string				channel_topic_;		// Channel topic, initially empty
		string				password_;			// Password for channel, initially empty
		map<char, bool>		mode_;				// 0/false for off, 1/true for on
		vector<User>		users_;			// List of users in channel
		vector<User>		operator_list_;		// List of operators in channel
		vector<User>		invite_list_;		// List of users invited to channel
		vector<User>		ban_list_;			// List of banned users
};

# endif // CHANNEL_HPP
