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

// Gerer les commandes, je geres le pass nick user et je fais la suite jeudi

void Server::handlePass(Client &client, std::istringstream &iss)
{
	std::string pass;
	iss >> pass;

	if (pass.empty())
		return;

	client.setPass(pass);

	if (pass == this->_password)
		client.setVerif(true);
	else
	{
		std::string error = "ERROR - Password incorrect\r\n";
		send(client.getFd(), error.c_str(), error.size(), 0);
		close(client.getFd());
		client.setVerif(false);
	}
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
	}
	/*else if (command == "NICK")
	{
		std::string nick;
		iss >> nick;
		if (nick.size() > 0)
			client.setNick(nick);
	}
	else if (command == "USER")
	{
		std::string user;
		iss >> user;
		if (user.size() > 0)
			client.setUser(user);
		
			//CODE PAS FINI
			// ensuite tu fais un if et tu commences a gerer la verification du client par ordre
			// mdp bon, user nick dans le client et bien recu dans le server
	}*/


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
	char receipt[4096];
	int bytes_receive = recv(clientfd, receipt, sizeof(receipt) -1 , 0);

	if (bytes_receive > 0)
	{
		receipt[bytes_receive] = '\0';
		std::string recu = receipt;
		std::cout << "premiere connexion: " << receipt;

		std::string servername = "ircserv";
		std::string nick = "abalasub";

		std::string cap_ls = ":" + servername + " CAP * LS :\r\n";
		send(clientfd, cap_ls.c_str(), cap_ls.size(), 0);

		std::string welcome = ":" + servername + " 001 " + nick + " :Bienvenue sur le serveur IRC\r\n";
		send(clientfd, welcome.c_str(), welcome.size(), 0);
	}
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

		//if (!client.getVerif() && client.getPass() != this->_password)
		//{
		//	// send error ici si handlePass pas gere
		//	std::string error = "ERROR :Mot de passe incorrect\r\n";
		//	send(client.getFd(), error.c_str(), error.size(), 0);
		//	close(client.getFd());
		//	// enlever _fds
		//	this->_fds.erase(this->_fds.begin() + fd_index);
		//	//supp cllient
		//	for (size_t k = 0; k < this->_clients.size(); ++k)
		//	{
		//		if (this->_clients[k].getFd() == client.getFd())
		//		{
		//			this->_clients.erase(this->_clients.begin() + k);
		//			break;
		//		}
		//	}
		//}
	}
}


void Server::read_client()
{
	for (size_t i = 0; i < this->_fds.size(); i++)
	{
		if (this->_fds[i].revents & POLLIN)
		{
			if (i == 0)
			{
				add_client();
			}
			else
			{
				char receipt[4096];
				int bytes_receive = recv(this->_fds[i].fd, receipt, sizeof(receipt) - 1, 0);

				if (bytes_receive > 0)
				{
					receipt[bytes_receive] = '\0';
					std::string recu = receipt;
					std::cout << "msg recu: " << receipt;

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