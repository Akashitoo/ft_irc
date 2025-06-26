#include "Channel.hpp"

Channel::Channel(std::string name) : _name(name){}

Channel::~Channel(){}


std::string Channel::getName()
{
	return _name;
}

void Channel::addUser(Client& user)
{
	this->_users.push_back(user);
}

void Channel::addOperator(Client& user)
{
	this->_operators.push_back(user);
}

void Channel::sendToUsersNewUser(Client& sender)
{
	std::vector<Client>::iterator it;

	for(it = this->_users.begin(); it != this->_users.end(); it++)
	{
		if (sender.getFd() != it->getFd())
		{
			std::string cmd = ":" + sender.getNick() + "!" + sender.getUser() + "@localhost JOIN :#" + this->_name + "\r\n";
			send(it->getFd(), cmd.c_str(), cmd.size(), 0);
		}
	}
}

void Channel::sendToUsersMessage(std::string message, Client& sender)
{
	std::vector<Client>::iterator it;

	for(it = this->_users.begin(); it != this->_users.end(); it++)
	{
		if (sender.getFd() != it->getFd())
		{
			std::string cmd = ":" + sender.getNick() + "!" + sender.getUser() + "@localhost PRIVMSG #" + this->_name + " :" + message + "\r\n";
			send(it->getFd(), cmd.c_str(), cmd.size(), 0);
		}
	}
}

void Channel::RemoveUser(Client &client)
{
	for(std::vector<Client*>::iterator it = this->_users.begin(); it != _users.end(); it++)
	{

		if(*it == &client)
		{
			this->_users.erase(*it);
			break;
			// continue ;
		}
	}
}

