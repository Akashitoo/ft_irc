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
#include <csignal>

class Server{
int   ft_signal(int signum); 
};

#endif