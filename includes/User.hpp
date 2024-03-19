#ifndef USER_HPP
#define USER_HPP

#include "Server.hpp"
#include "Channel.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
// #include <alogrithm>
#include <cstring>

// User error messages (RFC 2812 section 5.2)
#define ERR_ALREADYREGISTRED_M      " :Unauthorized command (already registered)\n" // 462		Returned by the server to any link which tries to change part of the registered details (such as password or user details from second USER message).

// User error codes (RFC 2812 section 5.2)
#define ERR_ALREADYREGISTRED_C      462	// " :Unauthorized command (already registered)\n"	    Returned by the server to any link which tries to change part of the registered details (such as password or user details from second USER message).

using namespace std;
class Channel;

class User
{
    public:
        // CONSTRUCTORS
        User();
		User(int fd);
        User(int fd, string username, string nickname);
        ~User();

        // OVERLOAD
        bool    operator==(User const &rhs);

        // METHODS
        bool    user_exists(string nickname);
        bool    user_exists(User nickname);
		void	execute(string cmd, User *user);

		bool	user_options(User *user, std::vector<std::string> splitmsg); // CHANGE
		int		authorise(User *user, std::string cmd); // CHANGE
		bool	parse_cmd(std::string str); // CHANGE
		void	change_user(User *user, std::vector<std::string> splitmsg); // CHANGE


        // GETTERS
        vector<Channel> getChannels() const;
		string			getInput() const;
        string          getUsername() const;
        string          getNickname() const;
        int             getFd() const;
        bool            isRegistered() const;

        // SETTERS
        void    setUsername(string username);
        void    setNickname(string nickname);
        void    setRegistered(bool isRegistered);
        void    setFd(int fd);
		void	setInput(string input);
        void    addChannel(Channel channel);
        void    removeChannel(Channel channel);
        void    closeFd();

		// FLAGS
		int	nick_flag;
		int	user_flag;
		int	pass_flag;
		bool	isAuth; // change
		vector<string> _cmd; // CHANGE
		string	pass; // CHANGE
		int		pass_issue; // CHANGE
		bool	change_flag; // CHANGE
		int		alr_reg; //change

		// ITERATORS
		std::vector<User>::iterator it_u;
		std::vector<User>::iterator it_i;


    private:
        int     fd_;				// File descriptor of client/user
        bool    isRegistered_;		// Has user enetered password and set a username?
        string  username_;			// Username (if any)
        string  nickname_;			// Nickname (required)
        vector<Channel> channels_;	// List of channels client/user has joined
		string	input_;				// Input of client

};

# endif // USER_HPP
