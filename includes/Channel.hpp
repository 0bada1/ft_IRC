#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Server.hpp"
#include "User.hpp"
#include <map>

// Channel modes
// #define INVITE_ONLY_CHANNEL 50
// #define OPEN_CHANNEL 51
// #define 

// Channel error messages  (RFC 2812 section 5.2) _M = MESSAGE
#define ERR_NOSUCHNICK_M 		" :No such nick/channel\n" // 401				Used to indicate the nickname parameter supplied to a command is currently unused.
#define ERR_NOSUCHCHANNEL_M 	" :No such channel\n" // 403					Used to indicate the server name given currently does not exist.
#define ERR_CANNOTSENDTOCHAN_M	" :Cannot send to channel\n" // 404				Sent to a user who is either (a) not on a channel which is mode +n or (b) not a chanop (or mode +v) on a channel which has mode +m set or where the user is banned and is trying to send a PRIVMSG message to that channel.
#define ERR_TOOMANYCHANNELS_M 	" :You have joined too many channels\n" // 405	Sent to a user when they have joined the maximum number of allowed channels and they try to join another channel.
#define ERR_USERNOTINCHANNEL_M	" :They aren't on that channel\n" // 441		Returned by the server to indicate that the target user of the command is not on the given channel.
#define ERR_NOTONCHANNEL_M		" :You're not on that channel\n" // 442			Returned by the server to indicate that the target user of the command is not on the given channel.
#define ERR_CHANNELISFULL_M 	" :Channel is full\n" // 471					Sent to a user who is trying to join a channel which is set +l and is already full.
#define ERR_UNKNOWNMODE_M		" :is unknown mode char to me for " // 472		Used to indicate that a given mode char is unknown.
#define ERR_INVITEONLYCHAN_M 	" :Channel is invite only\n" // 473				Sent to a user trying to join a channel which is invite-only (+i) without an invitation.
#define ERR_BANNEDFROMCHAN_M 	" :You are banned from this channel\n" // 474	Sent to a user trying to join a channel where the user is banned.
#define ERR_BADCHANNELKEY_M 	" :Cannot join channel (+k)" // 475				Sent to a user trying to join a channel with a key when the channel has been set +k (or mode +k).
#define ERR_CHANOPRIVSNEEDED_M	" :You're not channel operator" // 482			Any command requiring 'chanop' privileges (such as MODE messages) MUST return this error if the client making the attempt is not a chanop on the specified channel.

// Channel error codes (RFC 2812 section 5.2) _C = CODE
#define ERR_NOSUCHNICK_C 		401	// " :No such nick/channel\n"				Used to indicate the nickname parameter supplied to a command is currently unused.
#define ERR_NOSUCHCHANNEL_C 	403	// " :No such channel\n"					Used to indicate the server name given currently does not exist.
#define ERR_CANNOTSENDTOCHAN_C	404	// " :Cannot send to channel\n"				Sent to a user who is either (a) not on a channel which is mode +n or (b) not a chanop (or mode +v) on a channel which has mode +m set or where the user is banned and is trying to send a PRIVMSG message to that channel.
#define ERR_TOOMANYCHANNELS_C 	405	// " :You have joined too many channels\n"	Sent to a user when they have joined the maximum number of allowed channels and they try to join another channel.
#define ERR_USERNOTINCHANNEL_C	441	// " :They aren't on that channel\n"		Returned by the server to indicate that the target user of the command is not on the given channel.
#define ERR_NOTONCHANNEL_C		442 // " :You're not on that channel\n"			Returned by the server to indicate that the target user of the command is not on the given channel.
#define ERR_CHANNELISFULL_C 	471	// " :Channel is full\n"					Sent to a user who is trying to join a channel which is set +l and is already full.
#define ERR_UNKNOWNMODE_C		472	// " :is unknown mode char to me for "		Used to indicate that a given mode char is unknown.
#define ERR_INVITEONLYCHAN_C 	473	// " :Channel is invite only\n"				Sent to a user trying to join a channel which is invite-only (+i) without an invitation.
#define ERR_BANNEDFROMCHAN_C 	474	// " :You are banned from this channel\n"	Sent to a user trying to join a channel where the user is banned.
#define ERR_BADCHANNELKEY_C 	475	// " :Cannot join channel (+k)"				Sent to a user trying to join a channel with a key when the channel has been set +k (or mode +k).
#define ERR_CHANOPRIVSNEEDED_C	482	// " :You're not channel operator"			Any command requiring 'chanop' privileges (such as MODE messages) MUST return this error if the client making the attempt is not a chanop on the specified channel.

// #define NOT_OPERATOR_ERROR	"Error: You're not an operator of the channel\n"
// #define NO_USER_ERROR 		"Error: No such username\n"
// #define NO_CHANNEL_ERROR 	"Error: No such channel\n"
// #define INVITE_ONLY_CHANNEL	"Error: This channel is invite only\n"
// // #define 

using namespace std;
class Channel
{
	public:
		// CONSTRUCTORS
		// Channel();
		Channel(string channel_name, char type);
		~Channel();

		// OVERLOADS
		bool operator==(const Channel& other) const;

		// METHODS
		// int	create_channel(string channel_name);
		int		set_Channel_Modes(Channel channel, int mode, bool mode_state);
		bool	channel_exists(Channel channel);
		bool	channel_exists(string channel);
		int		find_user(vector<User> users, User user); // overload to search by string
		int		removeUser(string user);

		// GETTERS
		int				get_max_users() const;
		char			get_type() const;
		string			get_channel_name() const;
		string			get_channel_topic() const;
		string			get_password() const;
		vector<User>	get_users() const;
		vector<User>	get_operator_list() const;
		vector<User>	get_invite_list() const;
		vector<User>	get_ban_list() const;

	private:
		unsigned int		max_users_;			// Max users allowed in channel, initially unlimited
		char				type_;				// '#' for public, '&' for private
		string				channel_name_;		// Channel name
		string				channel_topic_;		// Channel topic, initially empty
		string				password_;			// Password for channel, initially empty
		map<char, int>		mode_;				// 0/false for off, 1/true for on
		vector<User>		users_;				// List of users in channel
		vector<User>		operator_list_;		// List of operators in channel
		vector<User>		invite_list_;		// List of users invited to channel
		vector<User>		ban_list_;			// List of banned users
};

# endif // CHANNEL_HPP
