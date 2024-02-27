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
        int join(string channel, User user);
        int kick(Channel channel, User user, User kicked_user, string reason);
        int invite(Channel channel, User user, User invited_user);
        int topic(Channel channel, User user, string topic);
        int mode (Channel channel, User user, string mode);
        int part(Channel channel, User user);

        int addUser(string newUser, User user);
        int nick(string newNick, User user);
        int user(string newUser, User user);
        
    private:

};

#endif // COMMANDS_HPP
