#include "includes/Server.hpp"
#include "includes/User.hpp"
#include "includes/Commands.hpp"
#include "includes/Parse.hpp"
#include "includes/Channel.hpp"

/*###################################################################*/
void    handleNickCommand(std::vector<std::string>& parameters,User& currentUser)
{
    if (parameters.size() != 1)
    {
        Utils::sendErrorMessage(currentUser.getFd(), ERR_NEEDMOREPARAMS_M, ERR_NEEDMOREPARAMS_C);
		return ;
    }
    else
        Commands::nick(parameters[0],currentUser);
}

/*###################################################################*/
void    handleKickCommand(std::vector<std::string>& parameters,User &currentUser)
{
    std::string reason = "";
    if (parameters.size() < 2)
    {
        Utils::sendErrorMessage(currentUser.getFd(), ERR_NEEDMOREPARAMS_M, ERR_NEEDMOREPARAMS_C);
		return ;
    }
    if( parameters.size() >= 3 && parameters[2].size() > 0)
	{
		for (size_t i = 2; i < parameters.size(); i++)
        	reason = reason + parameters[i] + " ";
	}
    else
		reason = "";
	if (Utils::channel_exists(parameters[0]) == true)
		Commands::kick(Utils::find_Channel(parameters[0]), currentUser,parameters[1],reason);
	else
		Utils::sendErrorMessage(currentUser.getFd(), parameters[0] + ERR_NOSUCHCHANNEL_M, ERR_NOSUCHCHANNEL_C);
}

/*###################################################################*/
void	handleInviteCommand(std::vector<std::string> parameters, User &currentUser)
{
	if (parameters.size() != 2)
	{
		Utils::sendErrorMessage(currentUser.getFd(), ERR_NEEDMOREPARAMS_M, ERR_NEEDMOREPARAMS_C);
		return ;
	}

	if (Utils::channel_exists(parameters[0]) == true)
    	Commands::invite(Utils::find_Channel(parameters[0]), currentUser, parameters[1]);
	else
		Utils::sendErrorMessage(currentUser.getFd(), parameters[0] + ERR_NOSUCHCHANNEL_M, ERR_NOSUCHCHANNEL_C);
}

/*###################################################################*/
void    handlePrivmsgCommand(std::vector<std::string> parameters,User& currentUser)
{
    std::vector<std::string> message;
    if (parameters.size()  < 2)
    {
        Utils::sendErrorMessage(currentUser.getFd(), ERR_NEEDMOREPARAMS_M, ERR_NEEDMOREPARAMS_C);
		return ;
    }
    for (size_t i = 1; i < parameters.size(); i++)
        message.push_back(parameters[i]);
    if(parameters[0][0] == '#' || parameters[0][0] == '&' )
	{
		if (Utils::channel_exists(parameters[0]) == true)
        	Commands::privmsg(Utils::find_Channel(parameters[0]), currentUser, message);
		else
			Utils::sendErrorMessage(currentUser.getFd(), parameters[0] + ERR_NOSUCHCHANNEL_M, ERR_NOSUCHCHANNEL_C);
	}
    else
        Commands::privmsg(parameters[0],currentUser,message);
}

/*###################################################################*/
void    handleJoinCommand(std::vector<std::string>& parameters,User currentUser)
{
    std::string password;
    if (parameters.size() < 1 || parameters.size() > 2)
    {
        Utils::sendErrorMessage(currentUser.getFd(), ERR_NEEDMOREPARAMS_M, ERR_NEEDMOREPARAMS_C);
		return ;
    }

    if (parameters.size() == 2 && parameters[1].size() > 0)
        password = parameters[1];
    else
	{
		password = "";
		Channel new_channel(parameters[0], false);
		if (Utils::channel_exists(new_channel) == true)
			Commands::join(Utils::find_Channel(new_channel.get_channel_name()), currentUser, password);
		else
			Commands::join(new_channel, currentUser, password);

		// if (Utils::channel_exists(parameters[0]) == true)
        // 	Commands::join(Utils::find_Channel(parameters[0]), currentUser, password);
		// else
		// 	Utils::sendErrorMessage(currentUser.getFd(), parameters[0] + ERR_NOSUCHCHANNEL_M, ERR_NOSUCHCHANNEL_C);
	}
}

/*###################################################################*/
void    handlePartCommand(std::vector<std::string>& parameters,User &currentUser)
{
    if (parameters.size()  < 1)
    {
        Utils::sendErrorMessage(currentUser.getFd(), ERR_NEEDMOREPARAMS_M, ERR_NEEDMOREPARAMS_C);
		return ;
    }
    if(Utils::channel_exists(parameters[0]))
        Commands::part(Utils::find_Channel(parameters[0]), currentUser);
    else if(!Utils::channel_exists(parameters[0]))
        Commands::part(Utils::find_Channel(parameters[0]), currentUser);
}

/*###################################################################*/
void    handleModeCommand(std::vector<std::string>& parameters,User &currentUser)
{
    if (parameters.size() < 2)
    {
        Utils::sendErrorMessage(currentUser.getFd(), ERR_NEEDMOREPARAMS_M, ERR_NEEDMOREPARAMS_C);
		return ;
    }
    std::string mode_arg;
    if(parameters.size() >= 3 && parameters[2].size() > 0)
        mode_arg = parameters[2];
    else 
        mode_arg = "";
    if(Utils::channel_exists(parameters[0]))
        Commands::mode(Utils::find_Channel(parameters[0]), currentUser, parameters[1][1], parameters[1][0], mode_arg);
    else if(!Utils::channel_exists(parameters[0]))
        Commands::mode(Utils::find_Channel(parameters[0]), currentUser, parameters[1][1], parameters[1][0], mode_arg);
}
/*###################################################################*/
void    handleTopicCommand(std::vector<std::string> &parameters,User &currentUser)
{
    if (parameters.size() < 1 || parameters.size() > 2)
    {
        Utils::sendErrorMessage(currentUser.getFd(), ERR_NEEDMOREPARAMS_M, ERR_NEEDMOREPARAMS_C);
		return ;
    }
    else
    {
        std::string topic;
        if(parameters.size() > 1 && parameters[1].size() > 0)
            topic = parameters[1];
        else topic = "";
        if(Utils::channel_exists(parameters[0]))
            Commands::topic(Utils::find_Channel(parameters[0]), currentUser, topic);
        else if(!Utils::channel_exists(parameters[0]))
            Commands::topic(Utils::find_Channel(parameters[0]), currentUser, topic);
    }
}

/*###################################################################*/
void    handleNoticeCommand(std::vector<std::string> &parameters,User &currentUser)
{
    std::vector<std::string> message;
    if (parameters.size()  < 1 || (Utils::nickname_exists(parameters[0]) && parameters.size() < 2))
    {
        Utils::sendErrorMessage(currentUser.getFd(), ERR_NEEDMOREPARAMS_M, ERR_NEEDMOREPARAMS_C);
		return ;
    }
    if(Utils::nickname_exists(parameters[0]))
    {
        for (size_t i = 1; i < parameters.size(); i++)
            message.push_back(parameters[i]);
        Commands::notice(parameters[0],currentUser,message);
    }
    else
    {
        for (size_t i = 0; i < parameters.size(); i++)
            message.push_back(parameters[i]);
        Commands::notice(currentUser,message);
    }
}

/*###################################################################*/
void    handleCapCommand(std::vector<std::string> &parameters,User &currentUser)
{
    if (parameters.size()  < 1)
    {
        Utils::sendErrorMessage(currentUser.getFd(), ERR_NEEDMOREPARAMS_M, ERR_NEEDMOREPARAMS_C);
		return ;
    }
	if(parameters.size() >= 3 && parameters[0] == "LS" && parameters[1] == "302")
	{
		std::string S = "CAP * ACK :multi-prefix\r\n";
		send(currentUser.getFd(), S.c_str(), strlen(S.c_str()), 0);
	}
    else if (parameters.size() >= 2 && parameters[0] == "LS")
	{
	    std::string S = "CAP * ACK :multi-prefix\r\n";
		send(currentUser.getFd(),S.c_str(), strlen(S.c_str()), 0);
	}
	else if (parameters.size() >= 3 && parameters[0] == "REQ" && parameters[1] == "multi-prefix")
	{
		std::string S = "CAP * ACK :multi-prefix\n";
		send(currentUser.getFd(),S.c_str(), strlen(S.c_str()), 0);
	}
    return;
}

/*###################################################################*/
void    handlePassCommand(std::vector<std::string> &parameters,User &currentUser)
{
    if (parameters.size()  != 1)
    {
        Utils::sendErrorMessage(currentUser.getFd(), ERR_NEEDMOREPARAMS_M, ERR_NEEDMOREPARAMS_C);
		return ;
    }
    Commands::pass(currentUser, Server::_password);
}
/*###################################################################*/
void   handleUserCommand(std::vector<std::string> &parameters,User &currentUser)
{
    if (parameters.size()  != 1)
    {
        Utils::sendErrorMessage(currentUser.getFd(), ERR_NEEDMOREPARAMS_M, ERR_NEEDMOREPARAMS_C);
		return ;
    }
    Commands::user(parameters[0],currentUser);
}
/*###################################################################*/

void parse(User &currentUser, std::string input)
{
        std::istringstream iss(input);
        std::string command;
        std::vector<std::string> parameters;
        iss >> command; 
        std::string param;
		std::cout << command << std::endl;
        if (currentUser.user_flag == 1 && currentUser.nick_flag == 1 && currentUser.pass_flag == 1)
            currentUser.setRegistered(true);
        while(iss >> param)
            parameters.push_back(param);
        if (command == "NICK")
            handleNickCommand(parameters, currentUser);
        else if (command == "PASS")
            handlePassCommand(parameters,currentUser);
        else if (command == "USER")
            handleUserCommand(parameters,currentUser);
        else if(currentUser.isRegistered() == true)
        {
            if (command == "KICK")
                handleKickCommand(parameters, currentUser);
			else if (command == "INVITE")
				handleInviteCommand(parameters, currentUser);
            else if (command == "PRIVMSG")
                handlePrivmsgCommand(parameters, currentUser);
            else if (command == "JOIN")
                handleJoinCommand(parameters, currentUser);
            else if (command == "PART")
                handlePartCommand(parameters, currentUser);
            else if (command == "MODE")
                handleModeCommand(parameters, currentUser);
            else if(command == "TOPIC")
                handleTopicCommand(parameters, currentUser);
            else if (command == "NOTICE")
                handleNoticeCommand(parameters, currentUser);
            else if(command == "CAP")
                handleCapCommand(parameters, currentUser);
            else
            {
                Utils::sendErrorMessage(currentUser.getFd(), "421 :Unknown command", 421);
                return;
            }
        }
        else if(currentUser.isRegistered() == false)
            Utils::sendErrorMessage(currentUser.getFd(), "451 :You have not registered", 451);
        else
            Utils::sendErrorMessage(currentUser.getFd(), "421 :Unknown command", 421);
}
/*###################################################################*/