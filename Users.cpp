#include "includes/Users.hpp"

using namespace std;

// CONSTRUCTORS
Users::Users() {}

Users::Users(string usersname, string nickname)
{
    this->username_.push_back(username);
    this->nickname_.push_back(nickname);
}

Users::~Users() {}

// GETTERS
string Users::getUsersname() const { return usersname; }

string Users::getNickname() const { return nickname; }

// METHODS
