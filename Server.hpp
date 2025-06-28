#ifndef SERVER_HPP
# define SERVER_HPP

# include "ft_irc.hpp"

class Client;
class Channel;

class Server
{

	private :

	std::vector<struct pollfd> _fds;
	std::vector<Client*> _clients;
	std::vector<Channel*> _channels;
	const std::string _password;
	
	const	int 			_port;
	void handleClientInput(Client *client, const std::string &input, size_t fd_index);
	void handleCommand(Client *client, const std::string &line);
	void handlePass(Client *client, const std::string &line);
	void handleNick(Client *client, const std::string &line);
	void handleUser(Client *client, const std::string &line);
	// void handlePing(Client &client, std::istringstream &iss); idk if this is mandatory to keep
	void handlePing(Client *client, const std::string &line);
	void handleJoin(Client *client, const std::string &line);
	void handleKick(Client *client, const std::string &line);
	void handlePrivateMessage(Client *client, const std::string &line);
	void handleTopic(Client *client, const std::string &line);
	void handleMode(Client *client, const std::string &line);
	

	void checkRegistration(Client *client);
	Channel* findChannel(std::string name);

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