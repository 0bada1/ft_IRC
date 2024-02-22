#ifndef USERS_HPP
#define USERS_HPP

#include "Server.hpp"

using namespace std;

class Users
{
    public:
        // CONSTRUCTORS
        Users();
        Users(string username, string nickname);
        ~Users();

        // METHODS

        // GETTERS
        string getUsername() const;
        string getNickname() const;
        
    private:
        vector<string> username_;
        vector<string> nickname_;

};

# endif // USERS_HPP