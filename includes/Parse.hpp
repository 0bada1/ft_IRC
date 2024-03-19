#ifndef PARSE_HPP
#define PARSE_HPP

#include "User.hpp"

bool isNumeric(const std::string& str);
void    handleNickCommand(std::vector<std::string>& parameters,User& currentUser);
void    handleKickCommand(std::vector<std::string>& parameters,User& currentUser);
void    handlePrivmsgCommand(std::vector<std::string> parameters,User& currentUser);
void    handleJoinCommand(std::vector<std::string>& parameters,User currentUser);
void    handlePartCommand(std::vector<std::string>& parameters,User& currentUser);
void    handleModeCommand(std::vector<std::string>& parameters,User& currentUser);
void    handleTopicCommand(std::vector<std::string> &parameters,User &currentUser);
void    handleNoticeCommand(std::vector<std::string> &parameters,User &currentUser);
void 	parse(User &currentUser, std::string input);
void    handleCapCommand(std::vector<std::string> &parameters,User &currentUser);


#endif // PARSE_HPP