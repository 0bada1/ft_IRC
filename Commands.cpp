/**
 * @file Commands.cpp
 * @authors Obada Outabachi, AbdulAziz, Mutasem Majid
 * @brief 
 * @version 1.0
 * @date 2024-03-10
 * 
 * @copyright Copyright (c) 2024
 */

#include "includes/Commands.hpp"
#include "includes/Channel.hpp"
#include "includes/Utils.hpp"

using namespace std;

Commands::Commands() {}

Commands::~Commands() {}

/**
 * @brief Join or create a channel
 * 
 * @details
 * Case 1: Channel doesn't exist. Create a new channel and add the user to it as an operator\n
 * Case 2: Channel exists. Add the user to the channel\n
 * Case 3: User is already in the channel. Return an error message\n
 * Case 4: User is banned from the channel. Return an error message\n
 * Case 5: User is invited to the channel. Add the user to the channel\n
 * Case 6: User is not invited to the channel. Return an error message\n
 * 
 * @param channel Channel to be joined. Pass Channel(<"channel_name">, false)
 * @param user User who is joining the channel
 * @param key Password if provided. Pass "" if no password was provided
 */
int Commands::join(Channel &channel, User &user, string key)
{
    if (channel.get_channel_name() == "" || (channel.get_channel_name()[0] != '#' && channel.get_channel_name()[0] != '&'))
    {
        Utils::sendErrorMessage(user.getFd(), channel.get_channel_name() + ERR_NEEDMOREPARAMS_M, ERR_NEEDMOREPARAMS_C);
        return -1;
    }
	cout << user.getNickname() << endl;

    if (user.isRegistered() == true) // User is registered
    {    
        if (channel.channel_exists(channel) == true) // Channel exists
        {
            if (channel.find_user(channel.get_users(), user) < 0) // User is not in channel
            {
                if (channel.find_user(channel.get_ban_list(), user) < 0) // User is not banned from channel
                {
                    if ((channel.find_user(channel.get_invite_list(), user) >= 0 && channel.get_mode()['i'] == true) || channel.get_mode()['i'] == false) // (User is invited to channel and channel is invite only) or (channel is not invite only)
                    {
                        if ((channel.get_mode()['k'] == true && key == channel.get_password()) || (!channel.get_mode()['k'] == true && key == "")) // (Channel is password protected and password is correct) or (channel is not password protected and there is no key)
                        {
                            if ((channel.get_mode()['l'] == true && channel.get_users().size() + 1 <= static_cast<size_t>(channel.get_max_users())) || (channel.get_mode()['l'] == false)) // (Channel has user limit but is not full with new member) or (channel has no user limit)
                            {
                                if (user.getChannels().size() + 1 <= 10) // Max channels have not been joined
                                {
									cout << user.getNickname() << " has joined " << channel.get_channel_name() << endl;
                                    channel.addUser(user); // Add user to channel
                                    if (channel.get_users().size() == 1) // If only user in channel
                                        channel.addOperator(user); // Make user an operator
                                    user.addChannel(channel); // Add channel to user's list of channels
                                    displayChannelIntro(user);
                                    (channel.find_user(channel.get_invite_list(), user) >= 0) ? channel.removeInvite(user) : (void)user; // Remove user from invite list
                                }
                                else { Utils::sendErrorMessage(user.getFd(), ERR_TOOMANYCHANNELS_M, ERR_TOOMANYCHANNELS_C); return ERR_TOOMANYCHANNELS_C;} // Max channels have been joined
                            }
                            else { Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + ERR_CHANNELISFULL_M).c_str(), ERR_CHANNELISFULL_C); return ERR_CHANNELISFULL_C; } // Channel is full
                        }
                        else if (channel.get_mode()['k'] == false && key != "") // Channel is not password protected but a password was provided
                        { Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + " :Channel has no key required (-k)\n").c_str(), ERR_BADCHANNELKEY_C); return ERR_BADCHANNELKEY_C; } // There is no password but a password was provided
                        else { Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + ERR_BADCHANNELKEY_M).c_str(), ERR_BADCHANNELKEY_C); return ERR_BADCHANNELKEY_C; } // Password is incorrect
                    }
                    else { Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + ERR_INVITEONLYCHAN_M).c_str(), ERR_INVITEONLYCHAN_C); return ERR_INVITEONLYCHAN_C; } // Channel is invite only
                }
                else { Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + ERR_BANNEDFROMCHAN_M).c_str(), ERR_BANNEDFROMCHAN_C); return ERR_BANNEDFROMCHAN_C; } // User is banned from channel
            }
            else { Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + ERR_USERONCHANNEL_M).c_str(), ERR_USERONCHANNEL_C); return ERR_USERONCHANNEL_C; } // User is already in channel
        }
        else // Channel doesn't exist | Create new channel
        {
            Channel new_channel(channel.get_channel_name(), true);
			if (key != "")
			{
				new_channel.set_mode('k', true);
				new_channel.set_channel_password(key);
			}
            new_channel.addUser(user);
            new_channel.addOperator(user);
            user.addChannel(new_channel);
			Server::channels_.push_back(new_channel);
            displayChannelIntro(user);
        }
    }
    else { Utils::sendErrorMessage(user.getFd(), ERR_NOTREGISTERED_M, ERR_NOTREGISTERED_C); return ERR_NOTREGISTERED_C; } // User is not registered

    return 0;
}

/** @brief Kicks a user from a channel. The user kicking the other user must be an operator.\n
 * If the user is not an operator, the function will return an error message #ERR_CHANOPRIVSNEEDED\n
 * User will be removed from Channel::users_ & Channel::operator_list_ & User::channels_
 * @bug can kick self. Shouldn't be possible (FIXED)
 * 
 *  @example /kick #general evaluator | /kick #general evaluator "Spamming"
 *  @param channel Channel from which the user will be kicked | Pass Channel("<channel_name">, false)
 *  @param user User who is kicking the other user
 *  @param kicked_user User(string) to be kicked 
 *  @param reason Reason for the kick | (set to "" if not provided) (default: "No reason given")
**/
int Commands::kick(Channel &channel, User &user, string kicked_user, string reason)
{
    if (channel.get_channel_name() == "" || (channel.get_channel_name()[0] != '#' && channel.get_channel_name()[0] != '&'))
    {
        Utils::sendErrorMessage(user.getFd(), channel.get_channel_name() + ERR_NOSUCHCHANNEL_M, ERR_NOSUCHCHANNEL_C);
        return -1;
    }

    if (user.isRegistered() == true) // User is registered
    {
        if (channel.channel_exists(channel) == true) // Channel exists
        {
            if (channel.find_user(channel.get_users(), user) >= 0) // User is in channel
            {
                if (channel.find_user(channel.get_operator_list(), user) >= 0) // User is an operator
                {
                    if (channel.find_user(channel.get_users(), kicked_user) >= 0) // Kicked user is in channel
                    {
						if (!(Utils::find_User(kicked_user) == user)) // User is not kicking self
						{
							Utils::find_User(kicked_user).removeChannel(channel); // Remove channel from user's list of channels
							(channel.find_user(channel.get_operator_list(), Utils::find_User(kicked_user)) >= 0) ? channel.removeOperator(Utils::find_User(kicked_user)) : (void)kicked_user; // Remove user from operator list if they're operator
							(channel.find_user(channel.get_invite_list(), Utils::find_User(kicked_user)) >= 0) ? channel.removeInvite(Utils::find_User(kicked_user)) : (void)kicked_user; // Remove user from invite list if they're in invite list
							channel.removeUser(Utils::find_User(kicked_user)); // Remove user from channel
							string message = "You have been kicked from " + channel.get_channel_name() + " Reason: " + reason + "\n";
							send(Utils::find_User(kicked_user).getFd(), message.c_str(), strlen(message.c_str()), 0);
							
							message = (user.getNickname() + " has kicked " + Utils::find_User(kicked_user).getNickname() + " from " + channel.get_channel_name());
							channel.announce_channel(message.c_str());
							
							if (reason != "")
							{
								message = " Reason: " + reason + "\n";
								channel.announce_channel(message.c_str());
							}
							else
								channel.announce_channel(" Reason: No reason given\n");
						}
						else { Utils::sendErrorMessage(user.getFd(), " :You cannot kick yourself", ERR_USERNOTINCHANNEL_C); return ERR_USERNOTINCHANNEL_C; } // User is kicking self
                    }
                    else { Utils::sendErrorMessage(user.getFd(), (Utils::find_User(kicked_user).getNickname() + " " + channel.get_channel_name() + ERR_USERNOTINCHANNEL_M).c_str(), ERR_USERNOTINCHANNEL_C); return ERR_USERNOTINCHANNEL_C; } // Kicked user not in channel
                }   
                else { Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + ERR_CHANOPRIVSNEEDED_M).c_str(), ERR_CHANOPRIVSNEEDED_C); return ERR_CHANOPRIVSNEEDED_C; } // User is not an operator
            }
            else { Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + ERR_NOTONCHANNEL_M).c_str(), ERR_NOTONCHANNEL_C); return ERR_NOTONCHANNEL_C; } // User is not in channel
        }
        else { Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + ERR_NOSUCHCHANNEL_M).c_str(), ERR_NOSUCHCHANNEL_C); return ERR_NOSUCHCHANNEL_C; } // Channel doesn't exist
    }
    else { Utils::sendErrorMessage(user.getFd(), ERR_NOTREGISTERED_M, ERR_NOTREGISTERED_C); return ERR_NOTREGISTERED_C; } // User is not registered

    return 0;
}

/** @brief Invites a user to a channel. Invites are valid once and are removed after the user joins the channel\n
 * 
 *  @example /invite evaluator #general
 *  @note    Recheck else conditions to make sure they are correct. Wrote code while sleepy :)
 * 
 *  @param channel Channel to which the user will be invited | Pass Channel("<channel_name">, false)
 *  @param user User who is inviting the other user
 *  @param invited_user User to be invited
 */
int Commands::invite(Channel &channel, User &user, string invited_user)
{
    if (channel.get_channel_name() == "" || (channel.get_channel_name()[0] != '#' && channel.get_channel_name()[0] != '&'))
    {
        Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + ERR_NOSUCHCHANNEL_M).c_str(), ERR_NOSUCHCHANNEL_C); // Rewrite error message
        return -1;
    }

    if (user.isRegistered() == true) // User is registered
    {
        if (channel.channel_exists(channel) == true) // Channel exists
        {
            if (channel.find_user(channel.get_users(), user) >= 0) // User is in channel
            {
                if ((channel.find_user(channel.get_operator_list(), user) >= 0) || channel.get_mode()['i'] == false) // (User is an operator) or (channel is not invite only)
                {
                    if (channel.find_user(channel.get_users(), invited_user) < 0) // Invited user is not in channel
                    {
                        if (channel.find_user(channel.get_ban_list(), invited_user) < 0) // User is not banned from channel
                        {
                            string  message = "You have been invited to " + channel.get_channel_name() + " by " + user.getNickname() + "\n";
                            send(Utils::find_User(invited_user).getFd(), message.c_str(), strlen(message.c_str()), 0); // Invited user recieves message
                            message = invited_user + " has been invited to " + channel.get_channel_name() + "\n";
							channel.announce_channel(message);
                        }
                        else { Utils::sendErrorMessage(user.getFd(), (invited_user + " " + channel.get_channel_name() + ERR_BANNEDFROMCHAN_M).c_str(), ERR_BANNEDFROMCHAN_C); return ERR_BANNEDFROMCHAN_C; } // User is banned from channel
                    }
                    else { Utils::sendErrorMessage(user.getFd(), (invited_user + " " +  channel.get_channel_name() + ERR_USERONCHANNEL_M).c_str(), ERR_USERONCHANNEL_C); return ERR_USERONCHANNEL_C; } // Invited user is already in channel
                }
                else { Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + ERR_CHANOPRIVSNEEDED_M).c_str(), ERR_CHANOPRIVSNEEDED_C); return ERR_CHANOPRIVSNEEDED_C; } // User is not an operator (+o)
            }
            else { Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + ERR_NOTONCHANNEL_M).c_str(), ERR_NOTONCHANNEL_C); return ERR_NOTONCHANNEL_C; } // User is not in channel
        }
        else { Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + ERR_NOSUCHCHANNEL_M).c_str(), ERR_NOSUCHCHANNEL_C); return ERR_NOSUCHCHANNEL_C; } // Channel doesn't exist
    }
    else { Utils::sendErrorMessage(user.getFd(), ERR_NOTREGISTERED_M, ERR_NOTREGISTERED_C); return ERR_NOTREGISTERED_C; } // User is not registered

    return 0;
}

/**
 * @brief Sends a message to a channel
 * 
 * @param channel Channel for the message to be sent | Pass Channel("<channel_name">, false)
 * @param user User who is sending the message
 * @param message Message to be sent as a vector of strings
 */
int Commands::privmsg(Channel &channel, User &user, vector<string> message)
{
    if (channel.channel_exists(channel) == false)
    {
        Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + ERR_NOSUCHCHANNEL_M).c_str(), ERR_NOSUCHCHANNEL_C);
        return ERR_NOSUCHCHANNEL_C;
    }
	cout << "I AM HERE" << endl;
	for (size_t i = 0; i < channel.get_users().size(); i++)
		cout << "PRIVMSG: " << channel.get_users()[i].getNickname() << endl;

    string  msg;
    if (user.isRegistered() == true)
    {
        if (channel.channel_exists(channel) == true) // Channel exists
        {
            if (channel.find_user(channel.get_users(), user) >= 0) // User is in channel
            {
				for (size_t i = 0; i < channel.get_users().size(); i++)
                {
                    msg = user.getNickname() + ": ";
                    send(channel.get_users()[i].getFd(), msg.c_str(), strlen(msg.c_str()), 0); // Send sender's nickname to reciever
                    for (size_t j = 0; j < message.size(); j++) // loop over all strings in message
                    {
                        msg = message[j] + " ";
                        send(channel.get_users()[i].getFd(), msg.c_str(), strlen(msg.c_str()), 0); // Send message to reciever
                    }
                    send(channel.get_users()[i].getFd(), "\n", 2, 0); // Send new line to reciever
                }
            }
            else { Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + ERR_NOTONCHANNEL_M).c_str(), ERR_NOTONCHANNEL_C); return ERR_NOTONCHANNEL_C; } // User is not in channel
        }
        else { Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + ERR_NOSUCHCHANNEL_M).c_str(), ERR_NOSUCHCHANNEL_C); return ERR_NOSUCHCHANNEL_C; } // Channel doesn't exist
    }
    else { Utils::sendErrorMessage(user.getFd(), ERR_NOTREGISTERED_M, ERR_NOTREGISTERED_C); return ERR_NOTREGISTERED_C; } // User is not registered

    return 0;
}

/**
 * @brief Sends a message to a user
 * 
 * @param receiver User(string) who will recieve the message
 * @param user User who is sending the message
 * @param message Message to be sent as a vector of strings separated by spaces, so no spaces in any element
 */
int Commands::privmsg(string receiver, User &user, vector<string> message)
{
    if (user.isRegistered() == true)
    {
        if (Utils::nickname_exists(receiver) == true) // User exists
        {
            if (Utils::find_User(receiver).getFd() != user.getFd()) // User is not sending a message to themselves
            {
                string  msg = user.getNickname() + ": ";
                send(Utils::find_User(receiver).getFd(), msg.c_str(), strlen(msg.c_str()), 0); // Send sender's nickname to reciever
                for (size_t i = 0; i < message.size(); i++) // loop over all strings in message
                {
                    msg = message[i] + " ";
                    send(Utils::find_User(receiver).getFd(), msg.c_str(), strlen(msg.c_str()), 0); // Send message to reciever
                }
                send(Utils::find_User(receiver).getFd(), "\n", 2, 0); // Send new line to reciever
            }
            else { Utils::sendErrorMessage(user.getFd(), "You cannot message yourself\n", -1); return -1; } // User is sending a message to themselves
        }
        else { Utils::sendErrorMessage(user.getFd(), (receiver + ERR_NOSUCHNICK_M).c_str(), ERR_NOSUCHNICK_C); return ERR_NOSUCHNICK_C; } // User doesn't exist
    }
    else { Utils::sendErrorMessage(user.getFd(), ERR_NOTREGISTERED_M, ERR_NOTREGISTERED_C); return ERR_NOTREGISTERED_C; } // User is not registered

    return 0;
}

/**
 * @brief Changes the topic of a channel. Topic will be displayed when a user joins the channel or when the /topic command is used
 * 
 * @example /topic #general IRC | /topic #general
 * @bug	User is not operator and has provided topic. Wrong error message
 * 
 * @param channel Channel to which the topic will be changed | Pass Channel("<channel_name">, false)
 * @param user User who is changing the topic
 * @param topic New topic | If no topic is provided, pass "" as the argument
 */
int Commands::topic(Channel &channel, User &user, string topic)
{
    if (channel.get_channel_name() == "" || (channel.get_channel_name()[0] != '#' && channel.get_channel_name()[0] != '&'))
    {
        send(user.getFd(), "Usage: /topic #<channel> <topic>\n", 34, 0); // Rewrite error message
        return -1;
    }

    if (user.isRegistered() == true) // User is registered
    {
        if (channel.channel_exists(channel) == true) // Channel exists
        {
            if (channel.find_user(channel.get_users(), user) >= 0) // User is in channel
            {
                if ((channel.find_user(channel.get_operator_list(), user) >= 0) || (channel.get_mode()['t'] == false && topic == "")) // (Channel is topic privileged and user is an operator) or (channel is not topic privileged, but a topic was not provided)
                {
                    if (topic != "" && topic.size() <= 50 && topic != channel.get_channel_topic() && channel.find_user(channel.get_operator_list(), user.getNickname()) >= 0) // New topic is not too long and is different from the current topic
                    {
                        channel.set_topic(topic);
                        channel.announce_channel(user.getNickname() + " has changed the topic to: " + topic + "\n"); // Announce to channel
                    }
                    else if (topic == "") // User wants to see the current topic
                    {
                        if (channel.get_channel_topic() != "")
                            send(user.getFd(), (channel.get_channel_name() + " topic: " + channel.get_channel_topic() + "\n").c_str(), channel.get_channel_name().size() + channel.get_channel_topic().size() + 11, 0); // Send current topic to user
                        else { Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + RPL_NOTOPIC_M).c_str(), RPL_NOTOPIC_C); return RPL_NOTOPIC_C; } // No topic set (RPL_NOTOPIC
                    }
                    else { Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + RPL_NOTOPIC_M).c_str(), RPL_NOTOPIC_C); return RPL_NOTOPIC_C; }
                }
                else { Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + ERR_CHANOPRIVSNEEDED_M).c_str(), ERR_CHANOPRIVSNEEDED_C); return ERR_CHANOPRIVSNEEDED_C; } // User is not an operator
            }
            else { Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + ERR_NOTONCHANNEL_M).c_str(), ERR_NOTONCHANNEL_C); return ERR_NOTONCHANNEL_C; } // User is not in channel          
        }
        else { Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + ERR_NOSUCHCHANNEL_M).c_str(), ERR_NOSUCHCHANNEL_C); return ERR_NOSUCHCHANNEL_C; } // Channel doesn't exist
    }
    else { Utils::sendErrorMessage(user.getFd(), ERR_NOTREGISTERED_M, ERR_NOTREGISTERED_C); return ERR_NOTREGISTERED_C; } // User is not registered

    return 0;
}

/**
 * @brief Changes the mode of a channel.\n
 * Available modes are: i, t, k, o, l
 * 
 * @example /mode #general +i
 * 
 * @param channel Channel to which the mode will be changed | Pass Channel("<channel_name">, false)
 * @param user User who is changing the mode
 * @param mode New mode (i, t, k, o, l) | If user inputs unavailable mode, pass it anyways
 * @param state State of the mode (+ or -) | If user inputs invalid state, pass it anyways
 * @param argument Argument for the mode | set to "" if not provided
 */
int Commands::mode(Channel &channel, User &user, char mode, char state, string argument)
{
    if (channel.get_channel_name() == "" || (channel.get_channel_name()[0] != '#' && channel.get_channel_name()[0] != '&'))
    {
        Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + ERR_NOSUCHCHANNEL_M).c_str(), ERR_NOSUCHCHANNEL_C);
        return -1;
    }
    else if (state != '+' && state != '-')
    {
		stringstream	ss;
		ss << state;
        string  message = ss.str();
		message = message + ERR_UNKNOWNMODE_M;
        Utils::sendErrorMessage(user.getFd(), message, ERR_UNKNOWNMODE_C);
        return -1;
    }
    else if (mode != 'i' && mode != 't' && mode != 'k' && mode != 'o' && mode != 'l')
    {
		stringstream	ss;
		ss << mode;
        string  message = ss.str();
		message = message + ERR_UNKNOWNMODE_M;
        Utils::sendErrorMessage(user.getFd(), message, ERR_UNKNOWNMODE_C);
        return -1;
    }

    if (user.isRegistered() == true) // User is registered
    {
        if (channel.channel_exists(channel) == true) // Channel exists
        {
            if (channel.find_user(channel.get_users(), user) >= 0) // User is in channel
            {
                if (channel.find_user(channel.get_operator_list(), user) >= 0) // User is an operator
                {
                    if (channel.get_mode()[mode] != state) // Channel state is different from what is already set
                    {
                        bool    mode_state = (state == '+') ? true : false;
                        if (mode == 'i') // Invite only channel
                        {
                            channel.set_mode(mode, mode_state); // Set channel mode
                            (mode_state == true) ? channel.announce_channel(user.getNickname() + " has changed " + channel.get_channel_name() + " to invite only (+i)\n") : channel.announce_channel(user.getNickname() + " has removed " + channel.get_channel_name() + " from invite only (-i)\n"); // Announce to channel
                        }
                        else if (mode == 't') // Topic privilege for channel
                        {
                            channel.set_mode(mode, mode_state); // Set channel mode
                            (mode_state == true) ? channel.announce_channel(user.getNickname() + " has changed " + channel.get_channel_name() + "'s topic privilege (+t)\n") : channel.announce_channel(user.getNickname() + " has removed " + channel.get_channel_name() + "'s topic privilege (-t)\n"); // Announce to channel
                        }
                        else if (mode == 'k') // Channel password
                        {
                            if (argument == "" && mode_state == true) { Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + ERR_NEEDMOREPARAMS_M).c_str(), ERR_NEEDMOREPARAMS_C); return ERR_NEEDMOREPARAMS_C; } // No password provided and mode_state is set to + 
                            (mode_state == true) ? channel.set_channel_password(argument) : channel.set_channel_password("");
                            (mode_state == true) ? send(user.getFd(), ("Channel " + channel.get_channel_name() + " password has been changed\n").c_str(), 37 + channel.get_channel_name().size(), 0) : send(user.getFd(), ("Channel " + channel.get_channel_name() + " password has been removed\n").c_str(), 37 + channel.get_channel_name().size(), 0); // User recieves message
                            channel.set_mode(mode, mode_state); // Set channel mode
                            (mode_state == true) ? channel.announce_channel(user.getNickname() + " has changed " + channel.get_channel_name() + "'s password\n") : channel.announce_channel(user.getNickname() + " has removed " + channel.get_channel_name() + "'s password\n"); // Announce to channel 
                        }
                        else if (mode == 'o') // Operator privilege for channel
                        {
                            if (argument == "" && mode_state == true) { Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + ERR_NEEDMOREPARAMS_M).c_str(), ERR_NEEDMOREPARAMS_C); return ERR_NEEDMOREPARAMS_C; } // No user provided and mode_state is set to +
                            int    operator_index = channel.find_user(channel.get_users(), argument);
                            if (operator_index < 0) { Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + ERR_NOSUCHNICK_M).c_str(), ERR_NOSUCHNICK_C); return ERR_NOSUCHNICK_C; } // User(argument) doesn't exist in channel
                            if (mode_state == true && channel.find_user(channel.get_operator_list(), argument) >= 0) { Utils::sendErrorMessage(user.getFd(), (argument + " is already an operator in " + channel.get_channel_name() + "\n").c_str(), -1); return -1; } // User is already an operator
                            (mode_state == true) ? channel.addOperator(channel.get_users()[operator_index]) : channel.removeOperator(channel.get_users()[operator_index]);
                            channel.set_mode(mode, mode_state); // Set channel mode
                            (mode_state == true) ? channel.announce_channel(user.getNickname() + " has given operator privileges to " + argument + " in " + channel.get_channel_name() + "\n") : channel.announce_channel(user.getNickname() + " has removed operator privileges from " + argument + " in " + channel.get_channel_name() + "\n"); // Announce to channel
                        }
                        else if (mode == 'l') // User limit for channel
                        {
                            if (argument == "" && mode_state == true) { Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + ERR_NEEDMOREPARAMS_M).c_str(), ERR_NEEDMOREPARAMS_C); return ERR_NEEDMOREPARAMS_C; } // No limit provided and mode_state is set to +
                            for (size_t i = 0; i < argument.size(); i++) // Check if argument is numeric (0-9)
                                if (!std::isdigit(argument[i])) { Utils::sendErrorMessage(user.getFd(), (argument + ERR_NEEDMOREPARAMS_M).c_str(), ERR_NEEDMOREPARAMS_C); return ERR_NEEDMOREPARAMS_C; } // Limit is not numeric
                            if (atoi(argument.c_str()) > 9999) { Utils::sendErrorMessage(user.getFd(), (argument + ERR_NEEDMOREPARAMS_M).c_str(), ERR_NEEDMOREPARAMS_C); return ERR_NEEDMOREPARAMS_C; } // Limit is too high
                            int   limit = atoi(argument.c_str());
                            (mode_state == true) ? channel.set_max_users(limit) : (void)argument; // Set user limit to argument or continue
                            channel.set_mode(mode, mode_state); // Set channel mode
                            (mode_state == true) ? channel.announce_channel(user.getNickname() + " has set " + channel.get_channel_name() + "'s user limit to " + argument + "\n") : channel.announce_channel(user.getNickname() + " has removed " + channel.get_channel_name() + "'s user limit\n"); // Announce to channel
                        }
                    }
                    else { return -1; } // Mode is already set | Ignore command
                }
                else { Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + ERR_CHANOPRIVSNEEDED_M).c_str(), ERR_CHANOPRIVSNEEDED_C); return ERR_CHANOPRIVSNEEDED_C; } // User is not an operator
            }
            else { Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + ERR_NOTONCHANNEL_M).c_str(), ERR_NOTONCHANNEL_C); return ERR_NOTONCHANNEL_C; } // User is not in channel
        }
        else { Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + ERR_NOSUCHCHANNEL_M).c_str(), ERR_NOSUCHCHANNEL_C); return ERR_NOSUCHCHANNEL_C; } // Channel doesn't exist
    }
    else { Utils::sendErrorMessage(user.getFd(), ERR_NOTREGISTERED_M, ERR_NOTREGISTERED_C); return ERR_NOTREGISTERED_C; } // User is not registered

    return 0;
}

/**
 * @brief Sends a notice to all users in the server | If you do not find the user, call the overloaded function instead
 * 
 * @param user User sending the notice
 * @param message Message to be sent as a vector of strings
 */
int Commands::notice(User &user, vector<string> message)
{
    if (message.size() == 0)
    {
        Utils::sendErrorMessage(user.getFd(), "Usage: /notice <user> <message>\n", -1);
        return -1;
    }

    string  msg;
    if (user.isRegistered() == true)
    {
        for (vector<User>::iterator it = Server::users_.begin(); it != Server::users_.end(); it++) // loop over all users in server and send message
        {
            msg = user.getNickname() + ": ";
            send(it->getFd(), msg.c_str(), strlen(msg.c_str()), 0); // Send sender's nickname to reciever
            for (size_t i = 0; i < message.size(); i++) // loop over all strings in message
            {
                msg = message[i] + " ";
                send(it->getFd(), msg.c_str(), strlen(msg.c_str()), 0); // Send message to reciever
            }
            send(it->getFd(), "\n", 2, 0); // Send new line to reciever
        }
    }
    else { Utils::sendErrorMessage(user.getFd(), ERR_NOTREGISTERED_M, ERR_NOTREGISTERED_C); return ERR_NOTREGISTERED_C; } // User is not registered

    return 0;
}

/**
 * @brief Sends a notice to a user
 * 
 * @param receiver User who will recieve the notice
 * @param user User sending the notice
 * @param message Message to be sent as a vector of strings separated by spaces, so no spaces in any element
 */
int Commands::notice(string receiver, User &user, vector<string> message)
{
    if (message.size() == 0)
    {
        Utils::sendErrorMessage(user.getFd(), "Usage: /notice <user> <message>\n", -1);
        return -1;
    }

    if (user.isRegistered() == true)
    {
        if (Utils::nickname_exists(receiver) == true) // User exists
        {
            if (Utils::find_User(receiver).getFd() != user.getFd()) // User is not sending a notice to themselves
            {
                string msg = user.getNickname() + ": ";
                send(Utils::find_User(receiver).getFd(), msg.c_str(), strlen(msg.c_str()), 0); // Send sender's nickname to reciever
                for (size_t i = 0; i < message.size(); i++) // loop over all strings in message
                {
                    msg = message[i] + " ";
                    send(Utils::find_User(receiver).getFd(), msg.c_str(), strlen(msg.c_str()), 0); // Send message to reciever
                }
                send(Utils::find_User(receiver).getFd(), "\n", 2, 0); // Send new line to reciever
            }
            else { Utils::sendErrorMessage(user.getFd(), "You cannot send a notice to yourself\n", -1); return -1; } // User is sending a notice to themselves
        }
        else { Utils::sendErrorMessage(user.getFd(), (receiver + ERR_NOSUCHNICK_M).c_str(), ERR_NOSUCHNICK_C); return ERR_NOSUCHNICK_C; } // User doesn't exist
    }
    else { Utils::sendErrorMessage(user.getFd(), ERR_NOTREGISTERED_M, ERR_NOTREGISTERED_C); return ERR_NOTREGISTERED_C; } // User is not registered

    return 0;
}

/**
 * @brief Leaves a channel. If the user is the last one, the channel will be deleted | If the user is the last operator, the first channel member will become an operator
 * @example /part #general
 * 
 * @param channel Channel to be left | If the channel doesn't exist, pass Channel("<channel_name">, false)
 * @param user User who is leaving the channel
 */
int Commands::part(Channel &channel, User &user)
{
    if (channel.get_channel_name() == "" || (channel.get_channel_name()[0] != '#' && channel.get_channel_name()[0] != '&'))
    {
        send(user.getFd(), "Usage: /part #<channel>\n", 24, 0); // Rewrite error message
        return -1;
    }

    if (user.isRegistered() == true) // User is registered
    {
        if (channel.channel_exists(channel) == true) // Channel exists
        {
            if (channel.find_user(channel.get_users(), user) >= 0) // User is in channel
            {
                channel.removeUser(user);
                (channel.find_user(channel.get_operator_list(), user) >= 0) ? channel.removeOperator(user) : (void)user; // Remove user from operator list if they are an operator
                (channel.find_user(channel.get_invite_list(), user) >= 0) ? channel.removeInvite(user) : (void)user; // Remove user from invite list if they are invited
                user.removeChannel(channel); // Remove channel from user's list of channels
                channel.announce_channel(user.getNickname() + " has left " + channel.get_channel_name() + "\n"); // Announce to channel
                send(user.getFd(), ("You have left " + channel.get_channel_name() + "\n").c_str(), 17 + channel.get_channel_name().size(), 0); // User recieves message

                if (channel.get_users().size() == 0) // If last user in channel
                    channel.removeChannel(channel.get_channel_name()); // Remove channel
                else if (channel.get_operator_list().size() == 0) // If last operator in channel
                {
                    channel.addOperator(channel.get_users()[0]); // First channel member becomes an operator
                    send(channel.get_users()[0].getFd(), ("You are now an operator in " + channel.get_channel_name() + "\n").c_str(), 31 + channel.get_channel_name().size(), 0); // First channel member recieves message
                }
            }
            else { Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + ERR_NOTONCHANNEL_M).c_str(), ERR_NOTONCHANNEL_C); return ERR_NOTONCHANNEL_C; } // User is not in channel
        }
        else { Utils::sendErrorMessage(user.getFd(), (channel.get_channel_name() + ERR_NOSUCHCHANNEL_M).c_str(), ERR_NOSUCHCHANNEL_C); return ERR_NOSUCHCHANNEL_C; } // Channel doesn't exist
    }
    else { Utils::sendErrorMessage(user.getFd(), ERR_NOTREGISTERED_M, ERR_NOTREGISTERED_C); return ERR_NOTREGISTERED_C; } // User is not registered
    return 0;
}

/**
 * @brief Sets a nickname for a user
 * If the nickname already exists, an error is thrown
 * 
 * @param newNick 
 * @param user 
 * @return int
 */
int Commands::nick(string newNick, User &user)
{
    if (Utils::nickname_exists(newNick) == true)
    {
        Utils::sendErrorMessage(user.getFd(), ERR_ALREADYREGISTRED_M, ERR_ALREADYREGISTRED_C);
        return -1;
    }

	user.nick_flag = 1;
    user.setNickname(newNick);
    return 0;
}

/**
 * @brief Sets a username for a user\n
 * If the username already exists, an error is thrown
 * 
 * @param newUser 
 * @param user 
 * @return int 
 */
int Commands::user(string newUser, User &user)
{
    if (Utils::username_exists(newUser) == true)
    {
        Utils::sendErrorMessage(user.getFd(), ERR_ALREADYREGISTRED_M, ERR_ALREADYREGISTRED_C);
        return -1;
    }

	user.user_flag = 1;
    user.setUsername(newUser);
    return 0;
}

/**
 * @brief Server password
 * 
 * @param user User entering pass
 * @param pass Server password as a string
 * @return int 0 on correct pass | If pass incorrect, disconnect client
 */
int	Commands::pass(User &user, string pass)
{
	string	message = "Password incorrect\n";
	if (pass == Server::_password)
		user.pass_flag = 1;
	else { Utils::sendErrorMessage(user.getFd(), message.c_str(), 462); return -1; }
	
	return 0;
}

// COMMAND HELPERS

int Commands::displayChannelIntro(User user)
{
    string  message = "List of Commands                                             Usage\n"
		"PRIVMSG - message user(s) in the channel           PRIVMSG <receiver>{,<receiver>} <text to be sent>\n"
		"MODE (o) - change the mode of the channel          MODE <channel> <+/-mode> {argument}\n"
		"TOPIC (o) - change the topic of the channel        TOPIC <channel> <topic>\n"
		"INVITE (o) - invite another user to the channel    INVITE <nickname> <channel>\n"
		"KICK (o) - eject a client from a channel           KICK <channel> <user> [<comment>] \n\n";

    send(user.getFd(), message.c_str(), strlen(message.c_str()), 0);
    return 0;
}
