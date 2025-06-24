#ifndef SERVER_HPP
# define SERVER_HPP

# include "ft_irc.hpp"

class Client;
class Channel;

class Server
{

	private :

	std::vector<struct pollfd> _fds;
	std::vector<Client> _clients;
	std::vector<Channel> _channels;
	const std::string _password;
	const	int 			_port;
	void handleClientInput(Client &client, const std::string &input, size_t fd_index);
	void handleCommand(Client &client, const std::string &line);
	void handlePass(Client &client, std::istringstream &iss);
	void handleNick(Client &client, std::istringstream &iss);
	void handleUser(Client &client, std::istringstream &iss);
	void checkRegistration(Client &client);
	Channel& findChannel(std::string name);

	public : 

		Server(std::string password, int port);
		Server(const Server& src);
		Server& operator=(const Server& src);
		~Server();

		void init();
		void start();
		void add_client();
		void read_client();

};

#endif