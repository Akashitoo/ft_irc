#include "Channel.hpp"

Channel::Channel(std::string name) : _name(name){}

Channel::~Channel(){}


std::string Channel::getName()
{
	return _name;
}

void Channel::addUser(Client* user)
{
	this->_users.push_back(user);
}

void Channel::addOperator(Client* user)
{
	this->_operators.push_back(user);
}

void Channel::eraseUser(Client* user)
{
	std::vector<Client*>::iterator it = std::find(this->_users.begin(), this->_users.end(), user);
	if (it != this->_users.end())
        this->_users.erase(it);
}

void Channel::sendToUsersNewUser(Client* sender)
{
	std::vector<Client*>::iterator it;

	for(it = this->_users.begin(); it != this->_users.end(); it++)
	{
		if (sender->getFd() != (*it)->getFd())
		{
			std::string cmd = ":" + sender->getNick() + "!" + sender->getUser() + "@localhost JOIN :#" + this->_name + "\r\n";
			send((*it)->getFd(), cmd.c_str(), cmd.size(), 0);
		}
	}
}

void Channel::sendToUsersMessage(std::string message, Client* sender)
{
	std::vector<Client*>::iterator it;

	for(it = this->_users.begin(); it != this->_users.end(); it++)
	{
		if (sender->getFd() != (*it)->getFd())
		{
			std::string cmd = ":" + sender->getNick() + "!" + sender->getUser() + "@localhost PRIVMSG #" + this->_name + " :" + message + "\r\n";
			send((*it)->getFd(), cmd.c_str(), cmd.size(), 0);
		}
	}
}

void Channel::sendToUsersCommand(std::string cmd)
{
	std::vector<Client*>::iterator it;

	for(it = this->_users.begin(); it != this->_users.end(); it++)
			send((*it)->getFd(), cmd.c_str(), cmd.size(), 0);
}

bool Channel::isOperator(Client* user)
{
	return (std::find(this->_operators.begin(), this->_operators.end(), user) != this->_operators.end());
}

std::vector<Client*> Channel::getUsers()
{
	return (this->_users);
}


void Channel::printUsers(Client* client)
{
	std::string channel = this->_name;
	std::string userList = RPL_NAMREPLY + client->getNick() + " = #" + channel + " :";

    for (size_t i=0; i < this->getUsers().size(); i++)
    {
        if (this->isOperator(this->getUsers()[i]))
            userList += "@";
        userList += this->getUsers()[i]->getNick();
        if (this->getUsers()[i] != this->getUsers().back())
            userList += " ";
    }
    userList += "\r\n";
    userList += RPL_ENDOFNAMES + client->getNick() + " #" + channel + " :" + "End of /NAMES list.\r\n";
    send(client->getFd(), userList.c_str(), userList.size(), 0);
}