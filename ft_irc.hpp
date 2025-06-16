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
# include <stdexcept>

//class Server{
//int   ft_signal(int signum); 
//};

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