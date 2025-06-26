#ifndef FT_IRC_HPP
# define FT_IRC_HPP

# include <iostream>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <vector>
# include <sys/poll.h>
# include <netinet/tcp.h>
# include <csignal>
# include <csignal>
# include <stdexcept>
# include <sstream>
# include <cstdlib>
# include <cerrno>
# include <cstring>

# include "Client.hpp"
# include "Server.hpp"
# include "Channel.hpp"

// Reply codes
#define RPL_WELCOME        ":localhost 001 "
#define RPL_YOURHOST       ":localhost 002 "
#define RPL_CREATED        ":localhost 003 "
#define RPL_MYINFO         ":localhost 004 "
#define RPL_BOUNCE         ":localhost 005 "

// Error codes
#define ERR_NOSUCHNICK     		":localhost 401 "
#define ERR_NOSUCHSERVER   		":localhost 402 "
#define ERR_NOSUCHCHANNEL  		":localhost 403 "
#define ERR_CANNOTSENDTOCHAN 	":localhost 404 "
#define ERR_TOOMANYCHANNELS 	":localhost 405 "
#define ERR_NORECIPIENT    		":localhost 411 "
#define ERR_NOTEXTTOSEND   		":localhost 412 "
#define ERR_UNKNOWNCOMMAND 		":localhost 421 "
#define ERR_NONICKNAMEGIVEN 	":localhost 431 "
#define ERR_ERRONEUSNICKNAME 	":localhost 432 "
#define ERR_NICKNAMEINUSE  		":localhost 433 "
#define ERR_USERNOTINCHANNEL 	":localhost 441 "
#define ERR_NOTONCHANNEL   		":localhost 442 "
#define ERR_USERONCHANNEL  		":localhost 443 "
#define ERR_NEEDMOREPARAMS 		":localhost 461 "
#define ERR_ALREADYREGISTRED 	":localhost 462 "
#define ERR_PASSWDMISMATCH 		":localhost 464 "
#define ERR_CHANNELISFULL  		":localhost 471 "
#define ERR_INVITEONLYCHAN 		":localhost 473 "
#define ERR_BADCHANNELKEY  		":localhost 475 "

class BindFailed : public std::exception {

	public:
	 virtual const char* what() const throw()
	 {
		return "Failed to bind socket";
	 }
};

class ListenFailed : public std::exception {

	public:
	 virtual const char* what() const throw()
	 {
		return "Failed to listen socket";
	 }
};

class BadArguments : public std::exception {

	public:
	 virtual const char* what() const throw()
	 {
		return "Bad arguments ! usage: ./ircserv <port> <password>";
	 }
};

#endif