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

#define ERR_NOSUCHNICK         ":localhost 401 "
#define ERR_NOSUCHSERVER       ":localhost 402 "
#define ERR_NOSUCHCHANNEL      ":localhost 403 "
#define ERR_CANNOTSENDTOCHAN   ":localhost 404 "
#define ERR_TOOMANYCHANNELS    ":localhost 405 "
#define ERR_WASNOSUCHNICK      ":localhost 406 "
#define ERR_TOOMANYTARGETS     ":localhost 407 "
#define ERR_NOORIGIN           ":localhost 409 "
#define ERR_NORECIPIENT        ":localhost 411 "
#define ERR_NOTEXTTOSEND       ":localhost 412 "
#define ERR_NOTOPLEVEL         ":localhost 413 "
#define ERR_WILDTOPLEVEL       ":localhost 414 "
#define ERR_BADMASK            ":localhost 415 "

#define ERR_UNKNOWNCOMMAND     ":localhost 421 "
#define ERR_NOMOTD             ":localhost 422 "
#define ERR_NOADMININFO        ":localhost 423 "
#define ERR_FILEERROR          ":localhost 424 "

#define ERR_NONICKNAMEGIVEN    ":localhost 431 "
#define ERR_ERRONEUSNICKNAME   ":localhost 432 "
#define ERR_NICKNAMEINUSE      ":localhost 433 "
#define ERR_NICKCOLLISION      ":localhost 436 "

#define ERR_USERNOTINCHANNEL   ":localhost 441 "
#define ERR_NOTONCHANNEL       ":localhost 442 "
#define ERR_USERONCHANNEL      ":localhost 443 "
#define ERR_NOLOGIN            ":localhost 444 "
#define ERR_SUMMONDISABLED     ":localhost 445 "
#define ERR_USERSDISABLED      ":localhost 446 "

#define ERR_NOTREGISTERED      ":localhost 451 "

#define ERR_NEEDMOREPARAMS     ":localhost 461 "
#define ERR_ALREADYREGISTRED   ":localhost 462 "
#define ERR_NOPERMFORHOST      ":localhost 463 "
#define ERR_PASSWDMISMATCH     ":localhost 464 "
#define ERR_YOUREBANNEDCREEP   ":localhost 465 "

#define ERR_KEYSET             ":localhost 467 "
#define ERR_CHANNELISFULL      ":localhost 471 "
#define ERR_UNKNOWNMODE        ":localhost 472 "
#define ERR_INVITEONLYCHAN     ":localhost 473 "
#define ERR_BANNEDFROMCHAN     ":localhost 474 "
#define ERR_BADCHANNELKEY      ":localhost 475 "
#define ERR_NOPRIVILEGES       ":localhost 481 "
#define ERR_CHANOPRIVSNEEDED   ":localhost 482 "
#define ERR_CANTKILLSERVER     ":localhost 483 "

#define ERR_NOOPERHOST         ":localhost 491 "

#define ERR_UMODEUNKNOWNFLAG   ":localhost 501 "
#define ERR_USERSDONTMATCH     ":localhost 502 "

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