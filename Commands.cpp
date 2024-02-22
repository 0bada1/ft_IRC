#include "includes/Commands.hpp"

using namespace std;

Commands::Commands() {}

Commands::~Commands() {}

int Commands::join(string channel, Users user)
{
    if (channel == "" || channel[0] != '#')
    {
        cerr << "Error 404: JOIN #<channel>" << endl;
        return -1;
    }

    cout << user.getNickname() << " has joined #" << channel << endl;
    cout << "------- Help menu -------  \nJOIN - join a channel\nPART - leave a channel\nNICK - change your nickname\nKICK - kick a user from a channel\n" << endl;
    cout << "-------- Commands usage --------\nJOIN #<channel>\nPART #<channel>\nNICK <new nickname>\nKICK #<channel> <user> <reason>\n" << endl;
    return 0;
}

int Commands::kick(string channel, Users user, Users kicked_user, string reason)
{
    if (channel == "" || channel[0] != '#')
    {
        cerr << "Usage: /kick #<channel> <user> <reason>" << endl;
        return -1;
    }

    cout << "Kicking " << kicked_user.getNickname() << " from " << channel << " for " << reason << endl;
    return 0;
}

int Commands::invite(string channel, Users user, Users invited_user)
{
    if (channel == "" || channel[0] != '#')
    {
        cerr << "Usage: /invite #<channel> <user>" << endl;
        return -1;
    }

    cout << user.getNickname() << " has invited " << invited_user.getNickname() << " to " << channel << endl;
    return 0;
}

int Commands::topic(string channel, Users user, string topic)
{
    if (channel == "" || channel[0] != '#')
    {
        cerr << "Usage: /topic #<channel> <topic>" << endl;
        return -1;
    }

    cout << user.getNickname() << " has changed the topic to: " << topic << endl;
    return 0;
}

int Commands::part(string channel, Users user)
{
    if (channel == "" || channel[0] != '#')
    {
        cerr << "Usage: /part #<channel>" << endl;
        return -1;
    }
    
    cout << user.getNickname() << " has left " << channel << endl;
    return 0;
}

int Commands::addUser(string newUser, Users user)
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

int Commands::nick(string newNick, Users user)
{
    if (Utils::user_exists(newNick) >= 0)
    {
        cerr << "462 User already exists" << endl;
        return -1;
    }

    // cout << user.getNickname() << " is now known as " << newNick << endl;
    return 0;
}

int Commands::user(string newUser, Users user)
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