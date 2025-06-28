#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "ft_irc.hpp"

class Client;

class Channel
{

	private :

		std::string				_name;
		std::string				_topic;
		std::string				_passKey;
		std::vector<Client*>	_users;
		std::vector<Client*>	_operators;

		bool _inviteOnly;
		bool _topicChangeOpOnly;

		int _userLimit;

	public :

		Channel(std::string name);
		~Channel();

		void 							addUser(Client* user);
		void 							addOperator(Client* user);
		void 							eraseUser(Client* user);
		void 							erasedOperator(Client* user);

		void 							sendToUsersMessage(std::string message, Client* sender);
		void 							sendToUsersNewUser(Client* sender);
		void 							sendToUsersCommand(std::string cmd);

		bool 							isOperator(Client* user);
		bool 							isOnChannel(Client* user);

		void setTopic(std::string topic);

		std::string getName(); 
		std::string getTopic(); 
		std::string getPassKey(); 

		bool getInviteOnly();
		bool getTopicOpOnly();

		int getUserLimit();

		std::vector<Client*>		getUsers();
		std::vector<Client*>		getOperators();

		void setPassKey(const std::string &passKey);
		void setInviteOnly(const bool &inviteOnly);
		void setTopicChOnly(const bool &topicChangeChOnly);
		void setUserLimit(int userLimit);

		void printUsers(Client* client);
};

#endif