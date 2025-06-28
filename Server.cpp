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

Server::Server(std::string password, int port): _password(password), _port(port){}

Server::~Server(){}

void Server::checkRegistration(Client *client)
{
    if (client->getVerif() && !client->getNick().empty() && !client->getUser().empty())
    {
        std::string nick = client->getNick();
        std::string user = client->getUser();
        
        // Envoyer les messages de bienvenue
        // RPL_WELCOME (001)
        std::string welcome = RPL_WELCOME + nick + " :Welcome to the IRC Network " + nick + "!" + user + "@localhost\r\n";
        send(client->getFd(), welcome.c_str(), welcome.size(), 0);
        
        // RPL_YOURHOST (002)
        std::string yourhost = RPL_YOURHOST + nick + " :Your host is localhost, running version 1.0\r\n";
        send(client->getFd(), yourhost.c_str(), yourhost.size(), 0);
        
        // RPL_CREATED (003)
        // std::string created = ":localhost 003 " + nick + " :This server was created June 2025\r\n";
        // send(client.getFd(), created.c_str(), created.size(), 0);
        
        // RPL_MYINFO (004)
        std::string myinfo = RPL_MYINFO + nick + " ircserv\r\n";
        send(client->getFd(), myinfo.c_str(), myinfo.size(), 0);
    }
}

Channel* Server::findChannel(std::string name)
{
	std::vector<Channel*>::iterator it;

	for (it = this->_channels.begin(); it != this->_channels.end(); it++)
	{
		if ((*it)->getName() == name)
			return *it;
	}
	return (NULL);
}

void Server::handleCommand(Client *client, const std::string &line)
{
	std::istringstream iss(line);
	std::string command;
	iss >> command;

	t_NameToFunc cmds[] = {
		{ "PASS", &Server::handlePass },
		{ "NICK", &Server::handleNick },
		{ "USER", &Server::handleUser },
		{ "JOIN", &Server::handleJoin },
		{ "PRIVMSG", &Server::handlePrivateMessage },
		{ "PING", &Server::handlePing },
		{ "KICK", &Server::handleKick },
		{ "TOPIC", &Server::handleTopic },
		{ "MODE", &Server::handleMode },
		{ "", NULL }
	};
	short i = -1; 
	while (cmds[++i].RAW != "") 
		if (cmds[i].RAW == command) 
			return ((this->*cmds[i].handle)(client, line), (void)0);
}

void Server::add_client()
{
	int sock = this->_fds[0].fd;
	sockaddr_in client;
	socklen_t client_size = sizeof(client);
	int clientfd = accept(sock, (struct sockaddr *)&client, &client_size);
	if (clientfd < 0)
		return;

	Client * new_client = new Client(clientfd);

	this->_clients.push_back(new_client);
	this->_fds.push_back(create_pollfd(clientfd));
}

void Server::handleClientInput(Client *client, const std::string &input, size_t fd_index)
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
						if (this->_clients[j]->getFd() == this->_fds[i].fd)
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
	
	for (size_t i=0; i < this->_clients.size(); i++)
		delete this->_clients[i];
	for (size_t i=0; i < this->_channels.size(); i++)
		delete this->_channels[i];
}