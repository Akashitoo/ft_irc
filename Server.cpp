#include "Server.hpp"

int g_stop = 0;

void ft_signal(int signum)
{
    (void)signum;
    std::cout << "Signal reçu, arret du serv" << std::endl;
    g_stop = 1;
}

struct pollfd create_pollfd(int sock)
{
	struct	pollfd nfd;
	nfd.fd = sock;
	nfd.events = POLLIN;
	nfd.revents = 0;
	return (nfd);
}

// COMMANDE

void Server::handlePass(Client &client, std::istringstream &iss)
{
	std::string pass;
	iss >> pass;

	if (pass.empty())
	{
		std::string error_msg = ":localhost 430 :No password given\r\n";
        send(client.getFd(), error_msg.c_str(), error_msg.size(), 0);
        return;
	}

	client.setPass(pass);

	if (pass == this->_password)
		client.setVerif(true);
	else
	{
		std::string error = "ERROR - Password incorrect\r\n";
		send(client.getFd(), error.c_str(), error.size(), 0);
		// close(client.getFd());
		client.setVerif(false);
	}
}

void Server::handleNick(Client &client, std::istringstream &iss)
{
	 if (!client.getVerif())
	{
		std::string error_msg = ":localhost 430 : NO VALID\r\n";
        send(client.getFd(), error_msg.c_str(), error_msg.size(), 0);
        return;
	}
    std::string nick;
    iss >> nick;
    if (nick.empty())
    {
        std::string error_msg = ":localhost 431 :No nickname given\r\n";
        send(client.getFd(), error_msg.c_str(), error_msg.size(), 0);
        return;
    }
    bool valid = true;
    for (size_t i = 0; i < nick.length(); i++)
    {
        char c = nick[i];
        if (!isalnum(c) && c != '-' && c != '_' && c != '[' && c != ']' && c != '\\' && c != '`' && c != '^' && c != '{' && c != '}')
        {
            valid = false;
            break;
        }
    }
    if (!valid)
    {
        std::string error_msg = ":localhost 432 " + nick + " :Erroneous nickname\r\n";
        send(client.getFd(), error_msg.c_str(), error_msg.size(), 0);
        return;
    }
    for (size_t i = 0; i < _clients.size(); i++)
    {
        if (_clients[i].getNick() == nick && _clients[i].getFd() != client.getFd())
        {
            std::string error_msg = ":localhost 433 * " + nick + " :Nickname is already in use\r\n";
            send(client.getFd(), error_msg.c_str(), error_msg.size(), 0);
            return;
        }
    }
    std::string oldNick = client.getNick();
    client.setNick(nick);
    std::string nick_msg;
    if (oldNick.empty())
        nick_msg = ":" + nick + " NICK :" + nick + "\r\n";
    else
        nick_msg = ":" + oldNick + " NICK :" + nick + "\r\n";
    send(client.getFd(), nick_msg.c_str(), nick_msg.size(), 0);
    checkRegistration(client);
}

void Server::handleUser(Client &client, std::istringstream &iss)
{
    std::string username, hostname, servername, realname;
    if (!client.getVerif())
	{
		std::string error_msg = ":localhost 430 : NO VALID\r\n";
        send(client.getFd(), error_msg.c_str(), error_msg.size(), 0);
        return;
	}
    iss >> username;
    if (username.empty())
    {
        std::string error_msg = ":localhost 461 " + client.getNick() + " USER :Not enough parameters\r\n";
        send(client.getFd(), error_msg.c_str(), error_msg.size(), 0);
        return;
    }
    iss >> hostname >> servername;
    std::string temp;
    if (iss >> temp && temp[0] == ':')
    {
        realname = temp.substr(1);
        std::string rest;
        std::getline(iss, rest);
        realname += rest;
    }
    client.setUser(username);
    checkRegistration(client);
}

void Server::checkRegistration(Client &client)
{
    if (client.getVerif() && !client.getNick().empty() && !client.getUser().empty())
    {
        std::string nick = client.getNick();
        std::string user = client.getUser();
        
        // Envoyer les messages de bienvenue
        // RPL_WELCOME (001)
        std::string welcome = ":localhost 001 " + nick + " :Welcome to the IRC Network " + nick + "!" + user + "@localhost\r\n";
        send(client.getFd(), welcome.c_str(), welcome.size(), 0);
        
        // RPL_YOURHOST (002)
        std::string yourhost = ":localhost 002 " + nick + " :Your host is localhost, running version 1.0\r\n";
        send(client.getFd(), yourhost.c_str(), yourhost.size(), 0);
        
        // RPL_CREATED (003)
        // std::string created = ":localhost 003 " + nick + " :This server was created June 2025\r\n";
        // send(client.getFd(), created.c_str(), created.size(), 0);
        
        // RPL_MYINFO (004)
        std::string myinfo = ":localhost 004 " + nick + " localhost 1.0 o o\r\n";
        send(client.getFd(), myinfo.c_str(), myinfo.size(), 0);
    }
}

Channel* Server::findChannel(std::string name)
{
	std::vector<Channel>::iterator it;

	for (it = this->_channels.begin(); it != this->_channels.end(); it++)
	{
		if (it->getName() == name)
			return &(*it);
	}
	return (NULL);
}

void Server::handleCommand(Client &client, const std::string &line)
{
	std::istringstream iss(line);
	std::string command;
	iss >> command;

	if (command == "PASS")
		handlePass(client, iss);
		//if (pass == server_password)
      //   client.setVerif(true);
	// }
	else if (command == "NICK")
        handleNick(client, iss);
	else if(command == "USER")
		handleUser(client, iss);
	else if (command == "JOIN")
	{
		std::string channel = line.substr(line.find("#") + 1);
		Channel* chan = this->findChannel(channel);
		if (!chan)
		{
			Channel tmp(channel);
			this->_channels.push_back(tmp);
			chan = &this->_channels.back();
			chan->addOperator(client);
		}
		chan->addUser(client);
		chan->sendToUsersNewUser(client);
	}
	else if (command == "PRIVMSG")
	{
		std::size_t dest = line.find(" ");

		if( line[ dest + 1] == '#')
		{
			std::string channel = line.substr(dest + 2, line.find(" ", dest + 1) - (dest + 2));
			Channel* chan = this->findChannel(channel);
			if (!chan)
			{

			}
			std::string msg = line.substr(line.find(" ", dest + 1) + 2);
			chan->sendToUsersMessage(msg , client);
		}
		else
		{
			std::string target = line.substr(dest + 1, line.find(" ", dest + 1) - (dest + 1));
			std::string msg = line.substr(line.find(" ", dest + 1) + 2);
			size_t len = this->_clients.size();
			for (size_t i = 0; i < len; i++)
					if (this->_clients[i].getNick() == target)
					{
						std::string cmd = ":" + client.getNick() + "!" + client.getUser() + "@localhost PRIVMSG " + target + " :" + msg + "\r\n";
						send(_clients[i].getFd(), cmd.c_str(), cmd.size(), 0);
						return ;
					}
						
		} 
	}

	else if (command == "PING")
	{
    std::string token;
    iss >> token;
    if (token.empty() && line.find(":") != std::string::npos)
        token = line.substr(line.find(":"));
    std::string pong = "PONG " + token + "\r\n";
    send(client.getFd(), pong.c_str(), pong.size(), 0);
	}
	/*else if (command == "NICK")
	{
		std::string nick;
		iss >> nick;
		if (nick.size() > 0)
			client.setNick(nick);
	}
	else if (command == "USER")
        handleUser(client, iss);
	// 	std::string nick;
	// 	iss >> nick;
	// 	if (nick.size() > 0)
	// 		client.setNick(nick);
	// }
	// else if (command == "USER")
	// {
	// 	std::string user;
	// 	iss >> user;
	// 	if (user.size() > 0)
	// 		client.setUser(user);
		
			//CODE PAS FINI
			// ensuite tu fais un if et tu commences a gerer la verification du client par ordre
			// mdp bon, user nick dans le client et bien recu dans le server
	}*/
}

void Server::add_client()
{
	int sock = this->_fds[0].fd;
	sockaddr_in client;
	socklen_t client_size = sizeof(client);
	int clientfd = accept(sock, (struct sockaddr *)&client, &client_size);
	if (clientfd < 0)
		return;
	Client new_client(clientfd);
	this->_clients.push_back(new_client);
	this->_fds.push_back(create_pollfd(clientfd));
}

void Server::handleClientInput(Client &client, const std::string &input, size_t fd_index)
{
    (void)fd_index;
	std::istringstream iss(input);
	std::string line;

	while (std::getline(iss, line))
	{
		if (!line.empty() && line[line.length() - 1] == '\r')
			line = line.substr(0, line.length() - 1);
		handleCommand(client, line);
	}
}


void Server::read_client()
{
	for (size_t i = 0; i < this->_fds.size(); i++)
	{
		if (this->_fds[i].revents & POLLIN)
		{
			if (i == 0)
				add_client();
			else
			{
				char receipt[4096];
				int bytes_receive = recv(this->_fds[i].fd, receipt, sizeof(receipt) - 1, 0);

				if (bytes_receive > 0)
				{
					receipt[bytes_receive] = '\0';
					std::string recu = receipt;
					std::cout << receipt;

					for (size_t j = 0; j < this->_clients.size(); ++j)
					{
						if (this->_clients[j].getFd() == this->_fds[i].fd)
						{
							handleClientInput(this->_clients[j], recu, i);
							break;
						}
					}
				}
				else
				{
					close(this->_fds[i].fd);
					this->_fds.erase(this->_fds.begin() + i);
					--i;
				}
			}
		}
	}
}


Server::Server(std::string password, int port): _password(password), _port(port){}

Server::~Server(){}

void Server::init()
{
	int sock = socket(AF_INET, SOCK_STREAM,  IPPROTO_TCP);
	int opt = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (sock == -1)
		std::cerr << "Error : Socket failed to create\n";
	
	sockaddr_in ipport;
	
	ipport.sin_family = AF_INET;
	ipport.sin_port = htons(this->_port);
	ipport.sin_addr.s_addr = inet_addr("127.0.0.1");	
	
	if (bind(sock, (struct sockaddr *)&ipport, sizeof(ipport)) == -1)
	{
		close(sock);
		throw BindFailed();
	}
		
	if (listen(sock, 10) == -1)
	{
		close(sock);
		throw ListenFailed();
	}

	this->_fds.push_back(create_pollfd(sock));
}

void Server::start()
{
	this->init();
	signal(SIGINT, ft_signal);
	signal(SIGQUIT, ft_signal);

	while (!g_stop)
	{
		int ret = poll(&this->_fds[0], this->_fds.size(), 100);
		if (ret > 0)
		    read_client();
	}
	
	// Fermeture de tous les sockets;
	for (size_t i=0; i < this->_fds.size(); i++)
		close(this->_fds[i].fd);
}