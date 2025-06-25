#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "ft_irc.hpp"

class Client;

class Channel
{

	private :

		std::string				_name;
		std::vector<Client>	_users;
		std::vector<Client>	_operators;

		

	public :

		Channel(std::string name);
		~Channel();

		void addUser(Client& user);
		void addOperator(Client& user);
		void sendToUsersMessage(std::string message, Client& sender);
		void sendToUsersNewUser(Client& sender);

		std::string getName(); 
};

#endif