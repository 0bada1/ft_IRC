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
 * @param channel Channel to be joined. If the channel doesn't exist, pass Channel(<"channel_name">, false)
 * @param user User who is joining the channel
 * @param key Password if provided. Pass "" if no password was provided
 */
int Commands::join(Channel channel, User user, string key)
{
    if (channel.get_channel_name() == "" || channel.get_channel_name()[0] != '#' || channel.get_channel_name()[0] != '&')
    {
        cerr << "Error 404: JOIN #<channel>" << endl;
        return -1;
    }

    if (user.isRegistered() == true) // User is registered
    {    
        if (Channel::channel_exists(channel) == true) // Channel exists
        {
            if (Channel::find_user(channel.get_users(), user) == false) // User is not in channel
            {
                if (Channel::find_user(channel.get_ban_list(), user) == false) // User is not banned from channel
                {
                    if ((Channel::find_user(channel.get_invite_list(), user) == true && channel.get_mode()['i'] == true) || channel.get_mode()['i'] == false) // (User is invited to channel and channel is invite only) or (channel is not invite only)
                    {
                        if (channel.get_mode()['k'] == true && key == channel.get_password() || channel.get_mode()['k'] == false && key == "") // (Channel is password protected and password is correct) or (channel is not password protected and there is no key)
                        {
                            if (channel.get_mode()['l'] == true && channel.get_users().size() + 1 <= channel.get_max_users() || channel.get_mode() == false) // (Channel has user limit but is not full with new member) or (channel has no user limit)
                            {
                                if (user.getChannels().size() + 1 <= 10) // Max channels have not been joined
                                {
                                    channel.addUser(user);
                                    if (channel.get_users().size() == 1) // Only user in channel
                                        channel.addOperator(user); // Make user an operator
                                    user.channels_.push_back(channel);
                                    displayChannelIntro(user);
                                }
                                else { Utils::sendErrorMessage(user.getFd(), ERR_TOOMANYCHANNELS_M, ERR_TOOMANYCHANNELS_C); cerr << ERR_TOOMANYCHANNELS_M; return ERR_TOOMANYCHANNELS_C;} // Max channels have been joined
                            }
                            else { Utils::sendErrorMessage(user.getFd(), channel.get_channel_name() + ERR_CHANNELISFULL_M, ERR_CHANNELISFULL_C); cerr << channel.get_channel_name() << ERR_CHANNELISFULL_M; return ERR_CHANNELISFULL_C; } // Channel is full
                        }
                        else if (channel.get_mode()['k'] == false && key != "") // Channel is not password protected but a password was provided
                        { Utils::sendErrorMessage(user.getFd(), channel.get_channel_name() + " :Channel has no key required (-k)\n", ERR_BADCHANNELKEY_C); cerr << ERR_BADCHANNELKEY_C << " ERROR" << " :Channel has no key required (-k)\n"; return ERR_BADCHANNELKEY_C; } // There is no password but a password was provided
                        else { Utils::sendErrorMessage(user.getFd(), channel.get_channel_name() + ERR_BADCHANNELKEY_M, ERR_BADCHANNELKEY_C); cerr << channel.get_channel_name() << ERR_BADCHANNELKEY_M; return ERR_BADCHANNELKEY_C; } // Password is incorrect
                    }
                    else { Utils::sendErrorMessage(user.getFd(), channel.get_channel_name() + ERR_INVITEONLYCHAN_M, ERR_INVITEONLYCHAN_C); cerr << channel.get_channel_name() << ERR_INVITEONLYCHAN_M; return ERR_INVITEONLYCHAN_C; } // Channel is invite only
                }
                else { Utils::sendErrorMessage(user.getFd(), channel.get_channel_name() + ERR_BANNEDFROMCHAN_M, ERR_BANNEDFROMCHAN_C); cerr << channel.get_channel_name() << ERR_BANNEDFROMCHAN_M; return ERR_BANNEDFROMCHAN_C; } // User is banned from channel
            }
            else { Utils::sendErrorMessage(user.getFd(), channel.get_channel_name() + ERR_USERONCHANNEL_M, ERR_USERONCHANNEL_C); cerr << channel.get_channel_name() << ERR_USERONCHANNEL_M; return ERR_USERONCHANNEL_C; } // User is already in channel
        }
        else // Channel doesn't exist | Create new channel <--- Can delete this else condition if parser passes a Channel constructor
        {
            Channel new_channel(channel, true);
            new_channel.addUser(user);
            new_channel.addOperator(user);
            user.channels_.push_back(new_channel);
            displayChannelIntro(user);
        }
    }
    else { Utils::sendErrorMessage(user.getFd(), ERR_NOTREGISTERED_M, ERR_NOTREGISTERED_C); cerr << ERR_NOTREGISTERED_M; return ERR_NOTREGISTERED_C; } // User is not registered

    return 0;
}

/** @brief Kicks a user from a channel. The user kicking the other user must be an operator.\n
 * If the user is not an operator, the function will return an error message #ERR_CHANOPRIVSNEEDED\n
 * User will be removed from Channel::users_ & Channel::operator_list_ & User::channels_
 * 
 *  @example /kick #general evaluator "No reason given"
 *  @todo   Use binary search to find the channel, user, user to be kicked, and the operator for efficiency\n
 *  Use send instead of cout
 *  @param channel Channel from which the user will be kicked | If the channel doesn't exist, pass Channel("<channel_name">, false)
 *  @param user User who is kicking the other user
 *  @param kicked_user User to be kicked 
 *  @param reason Reason for the kick | (set to "" if not provided) (default: "No reason given")
**/
int Commands::kick(Channel channel, User user, string kicked_user, string reason)
{
    if (channel.get_channel_name() == "" || channel.get_channel_name()[0] != '#' || channel.get_channel_name()[0] != '&')
    {
        cerr << "Usage: /kick #<channel> <user> <reason>" << endl;
        return -1;
    }

    if (Channel::channel_exists(channel) == true)
    {
        if (Channel::find_user(channel.get_users(), user) == 0)
        {
            if (Channel::find_user(channel.get_operator_list(), user) == 0)
            {
                if (Channel::find_user(channel.get_users(), kicked_user) == 0)
                {
                    // Use send(socket fd, message, message length, 0)
                    channel.removeUser(kicked_user);
                    kicked_user.channels_.erase(std::find(user.channels_.begin(), user.channels_.end(), channel));
                    
                    string  message = (user.getNickname() + " has kicked " + Utils::find_User(kicked_user).getNickname() + " from " + channel.get_channel_name());
                    send(user.get_fd(), message, strlen(message.c_str), 0);
                    cout << "Kicking " << kicked_user.getNickname() << " from " << channel;
                    
                    if (reason != "")
                    {
                        send(user.get_fd(), " Reason: " + reason + "\n", 12 + reason.size(), 0);
                        cout << " Reason: " << reason << endl;
                    }
                    else
                    {
                        send(user.get_fd(), " Reason: No reason given\n", 26, 0);
                        cout << " Reason: No reason given" << endl;
                    }
                }
                else { Utils::sendErrorMessage(user.getFd(), Utils::find_User(kicked_user).getNickname() + " " + channel.get_channel_name() + ERR_USERNOTINCHANNEL_M, ERR_USERNOTINCHANNEL_C); cerr << kicked_user << " " << channel.get_channel_name() << ERR_USERNOTINCHANNEL_M; return ERR_USERNOTINCHANNEL_C; } // Kicked user not in channel
            }   
            else { Utils::sendErrorMessage(user.getFd(), channel.get_channel_name() + ERR_CHANOPRIVSNEEDED_M, ERR_CHANOPRIVSNEEDED_C); cerr << user.getNickname() << " " << channel.get_channel_name() << ERR_CHANOPRIVSNEEDED_M; return ERR_CHANOPRIVSNEEDED_C; } // User is not an operator
        }
        else { Utils::sendErrorMessage(user.getFd(), channel.get_channel_name() + ERR_NOTONCHANNEL_M, ERR_NOTONCHANNEL_C); cerr << ERR_NOTONCHANNEL_C << " ERROR: " << user.getNickname() << " " << channel.get_channel_name() << ": " << ERR_NOTONCHANNEL_M; return ERR_NOTONCHANNEL_C; } // User is not in channel
    }
    else { Utils::sendErrorMessage(user.getFd(), channel.get_channel_name() + ERR_NOSUCHCHANNEL_M, ERR_NOSUCHCHANNEL_C); cerr << ERR_NOSUCHCHANNEL_C << " ERROR: " << channel.get_channel_name() << ": " << ERR_NOSUCHCHANNEL_M; return ERR_NOSUCHCHANNEL_C; } // Channel doesn't exist

    return 0;
}

/** @brief Invites a user to a channel
 * 
 *  @example /invite evaluator #general
 *  @note    Recheck else conditions to make sure they are correct. Wrote code while sleepy :)
 * 
 *  @param channel Channel to which the user will be invited | If the channel doesn't exist, pass Channel("<channel_name">, false)
 *  @param user User who is inviting the other user
 *  @param invited_user User to be invited
 */
int Commands::invite(Channel channel, User user, User invited_user)
{
    if (channel.get_channel_name() == "" || channel.get_channel_name()[0] != '#' || channel.get_channel_name()[0] != '&')
    {
        send(user.getFd(), "Usage: /invite #<channel> <user>\n", 32, 0); // Rewrite error message
        cerr << "Usage: /invite #<channel> <user>" << endl;
        return -1;
    }

    if (user.isRegistered() == true) // User is registered
    {
        if (Channel::channel_exists(channel) == true) // Channel exists
        {
            if (Channel::find_user(channel.get_users(), user) == true) // User is in channel
            {
                if ((channel.get_mode()['i'] == true && Channel::find_user(channel.get_operator_list(), user) == true) || channel.get_mode()['i'] == false) // Channel is invite only and user is an operator or channel is not invite only
                {
                    if (channel.find_user(channel.get_users(), invited_user) == false) // Invited user is not in channel
                    {
                        if (channel.find_user(channel.get_ban_list(), invited_user) == false) // User is not banned from channel
                        {
                            channel.addUser(invited_user);
                            invited_user.channels_.push_back(channel);
                            send(invited_user.getFd(), "You have been invited to " + channel.get_channel_name() + " by " + user.getNickname() + "\n", 33 + channel.get_channel_name().size() + user.getNickname().size(), 0); // Invited user recieves message
                            send(user.getFd(), invited_user.getNickname() + " has been invited to " + channel.get_channel_name() + "\n", 24 + invited_user.getNickname().size() + channel.get_channel_name().size(), 0); // Inviting user recieves message
                        }
                        else { Utils::sendErrorMessage(user.getFd(), invited_user.getNickname() + " " + channel.get_channel_name() + ERR_BANNEDFROMCHAN_M, ERR_BANNEDFROMCHAN_C); cerr << invited_user.getNickname() << " " << channel.get_channel_name() << ERR_BANNEDFROMCHAN_M; return ERR_BANNEDFROMCHAN_C; } // User is banned from channel
                    }
                    else { Utils::sendErrorMessage(user.getFd(), invited_user.getNickname() + " " +  channel.get_channel_name() + ERR_USERONCHANNEL_M, ERR_USERONCHANNEL_C); cerr << invited_user.getNickname() << " " << channel.get_channel_name() << ERR_USERONCHANNEL_M; return ERR_USERONCHANNEL_C; } // Invited user is already in channel
                }
                else { Utils::sendErrorMessage(user.getFd(), channel.get_channel_name() + ERR_CHANOPRIVSNEEDED_M, ERR_CHANOPRIVSNEEDED_C); cerr << channel.get_channel_name() << ERR_CHANOPRIVSNEEDED_M; return ERR_CHANOPRIVSNEEDED_C; } // User is not an operator (+o)
            }
            else { Utils::sendErrorMessage(user.getFd(), channel.get_channel_name() + ERR_NOTONCHANNEL_M, ERR_NOTONCHANNEL_C); cerr << channel.get_channel_name() << ERR_NOTONCHANNEL_M; return ERR_NOTONCHANNEL_C; } // User is not in channel
        }
        else { Utils::sendErrorMessage(user.getFd(), channel.get_channel_name() + ERR_NOSUCHCHANNEL_M, ERR_NOSUCHCHANNEL_C); cerr << channel.get_channel_name() << ERR_NOSUCHCHANNEL_M; return ERR_NOSUCHCHANNEL_C; } // Channel doesn't exist
    }
    else { Utils::sendErrorMessage(user.getFd(), ERR_NOTREGISTERED_M, ERR_NOTREGISTERED_C); cerr << ERR_NOTREGISTERED_M; return ERR_NOTREGISTERED_C; } // User is not registered
    
    // cout << user.getNickname() << " has invited " << invited_user.getNickname() << " to " << channel.get_channel_name() << endl;
    return 0;
}

/**
 * @brief Sends a message to a channel
 * 
 * @param channel Channel for the message to be sent | If the channel doesn't exist, pass Channel("<channel_name">, false)
 * @param user User who is sending the message
 * @param message Message to be sent as a vector of strings
 */
int Commands::privmsg(Channel channel, User user, vector<string> message)
{
    if (Channel::channel_exists(channel) == false)
    {
        Utils::sendErrorMessage(user.getFd(), channel.get_channel_name() + ERR_NOSUCHCHANNEL_M, ERR_NOSUCHCHANNEL_C);
        cerr << channel.get_channel_name() << ERR_NOSUCHCHANNEL_M;
        return ERR_NOSUCHCHANNEL_C;
    }

    if (user.isRegistered() == true)
    {
        if (Channel::channel_exists(channel) == true) // Channel exists
        {
            if (channel.find_user(user) >= 0) // User is in channel
            {
                for (vector<User>::iterator it = channel.get_users().begin(); it != channel.get_users().end(); it++) // loop over all members of channel and send message
                {
                    for (size_t i = 0; i < message.size(); i++) // loop over all strings in message
                        send(it->getFd(), user.getNickname() + ": " + message[i] + "\n", user.getNickname().size() + message[i].size() + 5, 0);
                }
            }
            else { Utils::sendErrorMessage(user.getFd(), channel.get_channel_name() + ERR_NOTONCHANNEL_M, ERR_NOTONCHANNEL_C); cerr << channel.get_channel_name() << ERR_NOTONCHANNEL_M; return ERR_NOTONCHANNEL_C; } // User is not in channel
        }
        else { Utils::sendErrorMessage(user.getFd(), channel.get_channel_name() + ERR_NOSUCHCHANNEL_M, ERR_NOSUCHCHANNEL_C); cerr << channel.get_channel_name() << ERR_NOSUCHCHANNEL_M; return ERR_NOSUCHCHANNEL_C; } // Channel doesn't exist
    }
    else { Utils::sendErrorMessage(user.getFd(), ERR_NOTREGISTERED_M, ERR_NOTREGISTERED_C); cerr << ERR_NOTREGISTERED_M; return ERR_NOTREGISTERED_C; } // User is not registered

    return 0;
}

/**
 * @brief Sends a message to a user
 * 
 * @param receiver User(string) who will recieve the message
 * @param user User who is sending the message
 * @param message Message to be sent as a vector of strings separated by spaces, so no spaces in any element
 */
int Commands::privmsg(string receiver, User user, vector<string> message)
{
    if (user.isRegistered() == true)
    {
        if (Utils::nickname_exists(receiver) == true) // User exists
        {
            if (Utils::find_User(receiver).getFd() != user.getFd()) // User is not sending a message to themselves
            {
                send(Utils::find_User(receiver).getFd(), user.getNickname() + ": ", user.getNickname().size() + 2, 0); // Send sender's nickname to reciever
                for (size_t i = 0; i < message.size(); i++) // loop over all strings in message
                    send(Utils::find_User(receiver).getFd(), message[i] + " ", message[i].size() + 2, 0); // Send message to reciever
                send(Utils::find_User(receiver).getFd(), "\n", 2, 0); // Send new line to reciever
            }
            else { Utils::sendErrorMessage(user.getFd(), "You cannot message yourself\n", -1); cerr << "You cannot message yourself"; return -1; } // User is sending a message to themselves
        }
        else { Utils::sendErrorMessage(user.getFd(), receiver + ERR_NOSUCHNICK_M, ERR_NOSUCHNICK_C); cerr << receiver << ERR_NOSUCHNICK_M; return ERR_NOSUCHNICK_C; } // User doesn't exist
    }
    else { Utils::sendErrorMessage(user.getFd(), ERR_NOTREGISTERED_M, ERR_NOTREGISTERED_C); cerr << ERR_NOTREGISTERED_M; return ERR_NOTREGISTERED_C; } // User is not registered

    return 0;
}

/**
 * @brief Changes the topic of a channel. Topic will be displayed when a user joins the channel or when the /topic command is used
 * 
 * @example /topic #general IRC
 * 
 * @param channel Channel to which the topic will be changed | If the channel doesn't exist, pass Channel("<channel_name">, false)
 * @param user User who is changing the topic
 * @param topic New topic | If no topic is provided, pass "" as the argument
 */
int Commands::topic(Channel channel, User user, string topic)
{
    if (channel.get_channel_name() == "" || channel.get_channel_name()[0] != '#' || channel.get_channel_name()[0] != '&')
    {
        send(user.getFd(), "Usage: /topic #<channel> <topic>\n", 34, 0); // Rewrite error message
        cerr << "Usage: /topic #<channel> <topic>" << endl;
        return -1;
    }

    if (user.isRegistered() == true) // User is registered
    {
        if (Channel::channel_exists(channel) == true) // Channel exists
        {
            if (channel.find_user(channel.get_users(), user) == true) // User is in channel
            {
                if ((channel.get_mode()['t'] == true && channel.find_user(channel.get_operator_list(), user) == true) || (channel.get_mode()['t'] == false)) // (Channel is topic privileged and user is an operator) or (channel is not topic privileged)
                {
                    // if (topic != "")
                    // check if topic is "" to throw RPL_NOTOPIC 
                    // check if topic is too long
                    // check if there is no topic
                    // check if topic is the same as the current topic
                    // fix order of if statements

                            channel.set_topic(topic);
                            channel.announce_channel(user.getNickname() + " has changed the topic to: " + topic + "\n"); // Announce to channel


                }
                else { Utils::sendErrorMessage(user.getFd(), channel.get_channel_name() + ERR_CHANOPRIVSNEEDED_M, ERR_CHANOPRIVSNEEDED_C); cerr << channel.get_channel_name() << ERR_CHANOPRIVSNEEDED_M; return ERR_CHANOPRIVSNEEDED_C; } // User is not an operator
            }
            else { Utils::sendErrorMessage(user.getFd(), channel.get_channel_name() + ERR_NOTONCHANNEL_M, ERR_NOTONCHANNEL_C); cerr << channel.get_channel_name() << ERR_NOTONCHANNEL_M; return ERR_NOTONCHANNEL_C; } // User is not in channel          
        }
        else { cerr << ERR_NOSUCHCHANNEL_C << " ERROR: " << ERR_NOSUCHCHANNEL_M; return ERR_NOSUCHCHANNEL_C; } // Channel doesn't exist
    }
    else { Utils::sendErrorMessage(user.getFd(), ERR_NOTREGISTERED_M, ERR_NOTREGISTERED_C); cerr << ERR_NOTREGISTERED_M; return ERR_NOTREGISTERED_C; } // User is not registered

    return 0;
}

/**
 * @brief Changes the mode of a channel.\n
 * Available modes are: i, t, k, o, l
 * 
 * @example /mode #general +i
 * 
 * @param channel Channel to which the mode will be changed
 * @param user User who is changing the mode
 * @param mode New mode (i, t, k, o, l)
 * @param state State of the mode (+ or -)
 * @param argument Argument for the mode | set to "" if not provided
 */
int Commands::mode(Channel channel, User user, char mode, char state, string argument)
{
    if (channel.get_channel_name() == "" || channel.get_channel_name()[0] != '#' || channel.get_channel_name()[0] != '&')
    {
        // Utils::sendErrorMessage(user.getFd(), )
        cerr << "Usage: /mode #<channel> <mode>" << endl;
        return -1;
    }
    else if (state != '+' && state != '-')
    {
        Utils::sendErrorMessage(user.getFd(), "ERROR " + ERR_UNKNOWNMODE_C + ": " + state + ERR_UNKNOWNMODE_M, ERR_UNKNOWNMODE_C);
        cerr << "Usage: /mode #<channel> <+/-mode> {argument}" << endl;
        return -1;
    }
    else if (mode != 'i' && mode != 't' && mode != 'k' && mode != 'o' && mode != 'l')
    {
        Utils::sendErrorMessage(user.getFd(), "ERROR " + ERR_UNKNOWNMODE_C + ": " + mode + ERR_UNKNOWNMODE_M, ERR_UNKNOWNMODE_C);
        cerr << "Usage: /mode #<channel> <+/-mode> {argument}" << endl;
        return -1;
    }

    if (user.isRegistered() == true) // User is registered
    {
        if (Channel::channel_exists(channel) == true) // Channel exists
        {
            if (channel.find_user(channel.get_users(), user) == true) // User is in channel
            {
                if (channel.find_user(channel.get_operator_list(), user) == true) // User is an operator
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
                            (mode_state == true) ? channel.set_channel_password(argument) : channel.set_channel_password("");
                            (mode_state == true) ? send(user.getFd(), ("Channel " + channel.get_channel_name() + " password has been changed\n").c_str(), 37 + channel.get_channel_name().size(), 0) : send(user.getFd(), ("Channel " + channel.get_channel_name() + " password has been removed\n").c_str(), 37 + channel.get_channel_name().size(), 0); // User recieves message
                            channel.set_mode(mode, mode_state); // Set channel mode
                            (mode_state == true) ? channel.announce_channel(user.getNickname() + " has changed " + channel.get_channel_name() + "'s password\n") : channel.announce_channel(user.getNickname() + " has removed " + channel.get_channel_name() + "'s password\n"); // Announce to channel 
                        }
                        else if (mode == 'o') // Operator privilege for channel
                        {
                            int    operator_index = Channel::find_user(channel.get_users(), argument);
                            (operator_index >= 0) ? (void)argument : Utils::sendErrorMessage(user.getFd(), channel.get_channel_name() + ERR_NOSUCHNICK_M, ERR_NOSUCHNICK_C); // User(argument) doesn't exist in channel
                            (mode_state == true) ? channel.addOperator(channel.get_users()[operator_index]) : channel.removeOperator(channel.get_users()[operator_index]);
                            channel.set_mode(mode, mode_state); // Set channel mode
                            (mode_state == true) ? channel.announce_channel(user.getNickname() + " has given operator privileges to " + argument + " in " + channel.get_channel_name() + "\n") : channel.announce_channel(user.getNickname() + " has removed operator privileges from " + argument + " in " + channel.get_channel_name() + "\n"); // Announce to channel
                        }
                        else if (mode == 'l') // User limit for channel
                        {
                            for (size_t i = 0; i < argument.size(); i++) // Check if argument is numeric (0-9)
                                (argument[i] >= '0' && argument[i] <= '9') ? (void)argument[i] : Utils::sendErrorMessage(user.getFd(), argument + ERR_NEEDMOREPARAMS_M, ERR_NEEDMOREPARAMS_C); // Return error or continue
                            (argument > "9999") ? Utils::sendErrorMessage(user.getFd(), argument + ERR_NEEDMOREPARAMS_M, ERR_NEEDMOREPARAMS_C) : (void)argument; // Return error or continue 
                            int   limit = atoi(argument.c_str());
                            (mode_state == true) ? channel.set_max_users(limit) : (void)argument; // Set user limit to argument or continue
                            channel.set_mode(mode, mode_state); // Set channel mode
                            (mode_state == true) ? channel.announce_channel(user.getNickname() + " has set " + channel.get_channel_name() + "'s user limit to " + argument + "\n") : channel.announce_channel(user.getNickname() + " has removed " + channel.get_channel_name() + "'s user limit\n"); // Announce to channel
                        }
                    }
                    else { return -1; }
                }
                else { Utils::sendErrorMessage(user.getFd(), channel.get_channel_name() + ERR_CHANOPRIVSNEEDED_M, ERR_CHANOPRIVSNEEDED_C); cerr << channel.get_channel_name() << ERR_CHANOPRIVSNEEDED_M; return ERR_CHANOPRIVSNEEDED_C; } // User is not an operator
            }
            else { Utils::sendErrorMessage(user.getFd(), channel.get_channel_name() + ERR_NOTONCHANNEL_M, ERR_NOTONCHANNEL_C); cerr << channel.get_channel_name() << ERR_NOTONCHANNEL_M; return ERR_NOTONCHANNEL_C; } // User is not in channel
        }
        else { Utils::sendErrorMessage(user.getFd(), channel.get_channel_name() + ERR_NOSUCHCHANNEL_M, ERR_NOSUCHCHANNEL_C); cerr << channel.get_channel_name() << ERR_NOSUCHCHANNEL_M; return ERR_NOSUCHCHANNEL_C; } // Channel doesn't exist
    }
    else { Utils::sendErrorMessage(user.getFd(), ERR_NOTREGISTERED_M, ERR_NOTREGISTERED_C); cerr << ERR_NOTREGISTERED_M; return ERR_NOTREGISTERED_C; } // User is not registered
    // cout << user.getNickname() << " has changed the mode to: " << state << mode << endl;
    return 0;
}

/**
 * @brief Leaves a channel
 * @example /part #general
 * 
 * @param channel Channel to be left
 * @param user User who is leaving the channel
 */
int Commands::part(Channel channel, User user)
{
    if (channel.get_channel_name() == "" || channel.get_channel_name()[0] != '#' || channel.get_channel_name()[0] != '&')
    {
        send(user.getFd(), "Usage: /part #<channel>\n", 24, 0); // Rewrite error message
        cerr << "Usage: /part #<channel>" << endl;
        return -1;
    }

    if (Channel::channel_exists(channel) == true)
    {
        if (Channel::find_user(channel.get_users(), user) == true)
        {
            channel.removeUser(user);
            user.channels_.erase(std::find(user.getChannels().begin(), user.getChannels().end(), channel));
            channel.announce_channel(user.getNickname() + " has left " + channel.get_channel_name() + "\n"); // Announce to channel
            // cout << user.getNickname() << " has left " << channel << endl;
        }
        else { Utils::sendErrorMessage(user.getFd(), channel.get_channel_name() + ERR_NOTONCHANNEL_M, ERR_NOTONCHANNEL_C); cerr << channel.get_channel_name() << ERR_NOTONCHANNEL_M; return ERR_NOTONCHANNEL_C; } // User is not in channel
    }
    else { Utils::sendErrorMessage(user.getFd(), channel.get_channel_name() + ERR_NOSUCHCHANNEL_M, ERR_NOSUCHCHANNEL_C); cerr << channel.get_channel_name() << ERR_NOSUCHCHANNEL_M; return ERR_NOSUCHCHANNEL_C; } // Channel doesn't exist
    
    send(user.getFd(), ("You have left " + channel.get_channel_name() + "\n").c_str(), 17 + channel.get_channel_name().size(), 0); // User recieves message
    // cout << user.getNickname() << " has left " << channel << endl;
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
int Commands::nick(string newNick, User user)
{
    if (Utils::nickname_exists(newNick) == true)
    {
        Utils::sendErrorMessage(user.getFd(), ERR_ALREADYREGISTRED_C + "ERROR " + ERR_ALREADYREGISTRED_M, ERR_ALREADYREGISTRED_C)
        cerr << ERR_ALREADYREGISTRED_C << "ERROR " << ERR_ALREADYREGISTRED_M << endl;
        return -1;
    }

    user.setNickname(newNick);
    // cout << user.getNickname() << " is now known as " << newNick << endl;
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
int Commands::user(string newUser, User user)
{
    if (Utils::username_exists(newUser) == true)
    {
        Utils::sendErrorMessage(user.getFd(), ERR_ALREADYREGISTRED_C + "ERROR " + ERR_ALREADYREGISTRED_M, ERR_ALREADYREGISTRED_C);
        cerr << ERR_ALREADYREGISTRED_C << "ERROR " << ERR_ALREADYREGISTRED_M << endl;
        return -1;
    }

    user.setUsername(newUser);
    // cout << user.getNickname() << " is now known as " << newUser << endl;
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
