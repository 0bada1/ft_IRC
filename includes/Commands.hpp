#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "Server.hpp"
#include "Users.hpp"

using namespace std;
class Commands
{
    public:
        // CONSTRUCTORS
        Commands();
        ~Commands();

        // METHODS
        int join(string channel, Users user);
        int kick(string channel, Users user, Users kicked_user, string reason);
        int invite(string channel, Users user, Users invited_user);
        int topic(string channel, Users user, string topic);
        int mode (string channel, Users user, string mode);
        int part(string channel, Users user);

        int addUser(string newUser, Users user);
        int nick(string newNick, Users user);
        int user(string newUser, Users user);
        
    private:

};

#endif // COMMANDS_HPP