#include "includes/User.hpp"

using namespace std;

// CONSTRUCTORS
User::User() {}

User::User(string username, string nickname)
{
    this->username_ = username;
    this->nickname_ = nickname;
    this->isRegistered_ = false;
}

User::~User() {}

// OVERLOAD
bool    User::operator==(User const &rhs)
{
    if (this->nickname_ == rhs.getNickname() && this->username_ == rhs.getUsername())
        return true;
    return false;
}


// GETTERS
string User::getUsername() const { return this->username_; }

string User::getNickname() const { return this->nickname_; }

// METHODS
