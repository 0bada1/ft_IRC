#ifndef COMMANDS_HPP
#define COMMANDS_HPP

// #include "Server.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include "Channel.hpp"
#include "User.hpp"

using namespace std;

class Commands
{
    public:
        // CONSTRUCTORS
        Commands();
        ~Commands();

        // METHODS
        int join(Channel channel, User user, string key);
        int kick(Channel channel, User user, string kicked_user, string reason);
        int invite(Channel channel, User user, string invited_user);
        int privmsg(Channel channel, User user, vector<string> message);
        int privmsg(string receiver, User user, vector<string> message);
        int notice(User user, vector<string> message);
        int notice(string receiver, User user, vector<string> message);
        int topic(Channel channel, User user, string topic);
        int mode (Channel channel, User user, char mode, char state, string argument);
        int part(Channel channel, User user);

        int nick(string newNick, User user);
        int user(string newUser, User user);

        int displayChannelIntro(User user);
        
    private:

};

#endif // COMMANDS_HPP
