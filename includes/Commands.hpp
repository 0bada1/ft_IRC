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

		// ITERATORS
		vector<Channel>::iterator	channel_it;

        // METHODS
        static int join(Channel &channel, User &user, string key);
        static int kick(Channel &channel, User &user, string kicked_user, string reason);
        static int invite(Channel &channel, User &user, string invited_user);
        static int privmsg(Channel &channel, User &user, vector<string> message);
        static int privmsg(string receiver, User &user, vector<string> message);
        static int notice(User &user, vector<string> message);
        static int notice(string receiver, User &user, vector<string> message);
        static int topic(Channel &channel, User &user, string topic);
        static int mode (Channel &channel, User &user, char mode, char state, string argument);
        static int part(Channel &channel, User &user);

        static int nick(string newNick, User &user);
        static int user(string newUser, User &user);
		static int pass(User &user, string pass);

        static int displayChannelIntro(User user);
        
    private:

};

#endif // COMMANDS_HPP
