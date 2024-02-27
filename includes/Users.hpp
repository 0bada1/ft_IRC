#ifndef USER_HPP
#define USER_HPP

// #include "Server.hpp"
#include "Channel.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
// #include <alogrithm>
#include <cstring>

using namespace std;

class User
{
    public:
        // CONSTRUCTORS
        User();
        User(string username, string nickname);
        ~User();

        // OVERLOAD
        bool    User::operator==(User const &rhs);

        // METHODS

        // GETTERS
        string getUsername() const;
        string getNickname() const;
        
    private:
        vector<Channel> channels_;
        string  username_;
        string  nickname_;
        bool    isRegistered_;

};

# endif // USER_HPP
