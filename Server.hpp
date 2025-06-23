#ifndef SERVER_HPP
# define SERVER_HPP

# include "ft_irc.hpp"
# include "Client.hpp"

class Server
{

	private :

		std::vector<struct pollfd> _fds;
		std::vector<Client> _clients;
		const std::string _password;

		void add_client();
		void read_client();
		void handleCommand(Client &client, const std::string &line);
		void init();

	public : 

		Server(std::string password);
		Server(const Server& src);
		Server& operator=(const Server& src);
		~Server();

		void start();
};

#endif