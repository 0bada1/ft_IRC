#include "includes/Commands.hpp"

using namespace std;

Commands::Commands() {}

Commands::~Commands() {}

int Commands::join(string channel, User user)
{
    if (channel == "" || channel[0] != '#' || channel[0] != '&')
    {
        cerr << "Error 404: JOIN #<channel>" << endl;
        return -1;
    }

    if (Channel::channel_exists(channel) == 0)
    {
        for (int i = 0; i < Server::channels_.size(); i++)
        {
            if (Server::channels_[i].get_channel_name() == channel)
            {
                Server::channels_[i].addUser(user);
            }
        }
    }
    else
    {
        Channel new_channel(channel, channel[0]);
        new_channel.addUser(user);
    }

    cout << user.getNickname() << " has joined #" << channel << endl;
    cout << "------- Help menu -------  \nJOIN - join or create a channel\t\tUsage: JOIN #<channel>\nPART - leave a channel\t\tUsage: PART #<channel>\nNICK - change your nickname\t\tUsage: NICK <new nickname>\nKICK - kick a user from a channel\t\tUsage: KICK #<channel> <username>\n" << endl;
    // cout << "-------- Commands usage --------\nJOIN #<channel>\nPART #<channel>\nNICK <new nickname>\nKICK #<channel> <user> <reason>\n" << endl;
    return 0;
}

/** @brief Kicks a user from a channel. The user kicking the other user must be an operator.\n
 * If the user is not an operator, the function will return an error message #ERR_CHANOPRIVSNEEDED\n
 * User will be removed from Channel::users_ & Channel::operator_list_ & User::channels_
 * 
 *  @example /kick #general evaluator "No reason given"
 *  @todo   Use binary search to find the channel, user, user to be kicked, and the operator for efficiency
 * 
 *  @param channel Channel from which the user will be kicked
 *  @param user User who is kicking the other user
 *  @param kicked_user User to be kicked 
 *  @param reason Reason for the kick | (set to "" if not provided) (default: "No reason given")
**/
int Commands::kick(Channel channel, User user, string kicked_user, string reason)
{
    if (channel.get_channel_name() == "" || channel[0] != '#' || channel[0] != '&')
    {
        cerr << "Usage: /kick #<channel> <user> <reason>" << endl;
        return -1;
    }

    // if (Utils::binarySearch(channel.get_operator_list()) == user)
    if (Channel::channel_exists(channel) == true)
    {
        if (Channel::find_user(channel.get_users(), user) == 0)
        {
            if (Channel::find_user(channel.get_operator_list(), user) == 0)
            {
                if (Channel::find_user(channel.get_users(), kicked_user) == 0)
                {
                    channel.removeUser(kicked_user);
                    kicked_user.channels_.erase(std::find(user.channels_.begin(), user.channels_.end(), channel));
                    cout << "Kicking " << kicked_user.getNickname() << " from " << channel;
                    if (reason != "")
                        cout << " Reason: " << reason << endl;
                    else
                        cout << " Reason: No reason given" << endl;
                }
                else { cerr << kicked_user << " " << channel.get_channel_name() << ERR_USERNOTINCHANNEL_M; return ERR_USERNOTINCHANNEL_C; }
            }
            else { cerr << user.getNickname() << " " << channel.get_channel_name() << ERR_CHANOPRIVSNEEDED_M; return ERR_CHANOPRIVSNEEDED_C; }
        }
        else { cerr << user.getNickname() << " " << channel.get_channel_name() << ERR_NOTONCHANNEL_M; return ERR_NOTONCHANNEL_C; }
    }
    else { cerr << channel.get_channel_name() << ERR_NOSUCHCHANNEL_M; return ERR_NOSUCHCHANNEL_C; }

    return 0;
}

/** @brief Invites a user to a channel
 * 
 *  @example /invite evaluator #general 
 * 
 *  @param channel Channel to which the user will be invited
 *  @param user User who is inviting the other user
 *  @param invited_user User to be invited
 */
int Commands::invite(Channel channel, User user, User invited_user)
{
    if (channel == "" || channel[0] != '#' || channel[0] != '&')
    {
        cerr << "Usage: /invite #<channel> <user>" << endl;
        return -1;
    }

    if (Channel::channel_exists(channel) == true)
    {
        for (int i = 0; i < Server::channels_.size(); i++)
        {
            if (Server::channels_[i].get_channel_name() == channel)
            {
                Server::channels_[i].addInvite(invited_user);
            }
        }
    }
    else
    {
        Channel new_channel(channel, channel[0]);
        new_channel.addInvite(invited_user);
    }
    cout << user.getNickname() << " has invited " << invited_user.getNickname() << " to " << channel << endl;
    return 0;
}

/**
 * @brief Changes the topic of a channel. Topic will be displayed when a user joins the channel or when the /topic command is used
 * 
 * @example /topic #general IRC
 * 
 * @param channel Channel to which the topic will be changed
 * @param user User who is changing the topic
 * @param topic New topic
 */
int Commands::topic(Channel channel, User user, string topic)
{
    if (channel == "" || channel[0] != '#' || channel[0] != '&')
    {
        cerr << "Usage: /topic #<channel> <topic>" << endl;
        return -1;
    }

    cout << user.getNickname() << " has changed the topic to: " << topic << endl;
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
 * @param mode New mode
 */
int Commands::mode(Channel channel, User user, string mode)
{
    if (channel == "" || channel[0] != '#' || channel[0] != '&')
    {
        cerr << "Usage: /mode #<channel> <mode>" << endl;
        return -1;
    }

    cout << user.getNickname() << " has changed the mode to: " << mode << endl;
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
    if (channel == "" || channel[0] != '#' || channel[0] != '&')
    {
        cerr << "Usage: /part #<channel>" << endl;
        return -1;
    }
    
    cout << user.getNickname() << " has left " << channel << endl;
    return 0;
}

int Commands::addUser(string newUser, User user)
{
    if (newUser == "" || newUser.size() > 9 || newUser.size() < 1)
    {
        if (newUser.size() > 9 || newUser.size() < 1)
            cerr << "Error 404: Username must be between 1 and  9 characters" << endl;
        cerr << "Usage: /user <new username>" << endl;
        return -1;
    }

    this->username_.push_back(newUser);
    // cout << user.getNickname() << " is now known as " << newUser << endl;
    return 0;
}

int Commands::nick(string newNick, User user)
{
    if (Utils::user_exists(newNick) >= 0)
    {
        cerr << "462 User already exists" << endl;
        return -1;
    }

    // cout << user.getNickname() << " is now known as " << newNick << endl;
    return 0;
}

int Commands::user(string newUser, User user)
{
    if (newUser == "" || newUser.size() > 9 || newUser.size() < 1)
    {
        if (newUser.size() > 9 || newUser.size() < 1)
            cerr << "Error 404: Username must be between 1 and  9 characters" << endl;
        else
            cerr << "Usage: /user <new username>" << endl;
        return -1;
    }

    this->username_.push_back(newUser);
    // cout << user.getNickname() << " is now known as " << newUser << endl;
    return 0;
}
