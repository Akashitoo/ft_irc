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
void Server::handleCommand(Client &client, const std::string &line)
{
	std::istringstream iss(line);
	std::string command;
	iss >> command;

	if (command == "PASS")
	{
		std::string pass;
		iss >> pass;
		if (pass.size() > 0)
			client.setPass(pass);
		//if (pass == server_password)
      //   client.setVerif(true);
	}
	else if (command == "NICK")
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
	}
}

void Server::add_client()
{
	int sock = this->_fds[0].fd;
	sockaddr_in client;
	socklen_t client_size = sizeof(client);
	int clientfd = accept(sock, (struct sockaddr *)&client, &client_size);
	//la j implemente le client je co au serv
	Client new_client(clientfd);
    this->_clients.push_back(new_client); 
	char receipt[4096];
	int bytes_receive = recv(clientfd, receipt, sizeof(receipt) -1 , 0);
	std::string test = "Bienvenue sur le serveur !\n";
	send(clientfd, test.c_str(), test.size(), 0);
	if (bytes_receive > 0)
	{
		receipt[bytes_receive] = '\0';
		std::string recu = receipt;
		std::cout << receipt;
		//Demain je dois changer cette condition vu que je geres les commandes
		std::string servername = "ircserv";
		std::string nick = "abalasub";

		std::string cap_ls = ":" + servername + " CAP * LS :\r\n";
		send(clientfd, cap_ls.c_str(), cap_ls.size(), 0);

		std::string welcome = ":" + servername + " 001 " + nick + " :Bienvenue sur le serveur IRC\r\n";
		send(clientfd, welcome.c_str(), welcome.size(), 0);
	}
	this->_fds.push_back(create_pollfd(clientfd));
}

void Server::read_client()
{
	
	for(size_t i=0; i < this->_fds.size(); i++)
	{
		if (this->_fds[i].revents & POLLIN)
		{

			if (i == 0)
				add_client();
			else
			{
				char receipt[4096];
				int bytes_receive = recv(this->_fds[i].fd, receipt, sizeof(receipt) -1 , 0);
				
				if (bytes_receive)
				{
					receipt[bytes_receive] = '\0';
					std::string recu = receipt;
					std::cout << receipt;
					for(size_t j = 0; j < this->_clients.size(); ++j)
					{
						if(this->_clients[j].getFd() == this->_fds[i].fd)
						{
							std::istringstream iss(recu);
							std::string line;
							while(std::getline(iss, line))
							{
								if(!line.empty() && line[line.length() - 1] == '\r')
    								line = line.substr(0, line.length() - 1);

								handleCommand(this->_clients[j], line);
							}
							break;
						}
					}
				}
				else
				{
					close(this->_fds[i].fd);
					this->_fds.erase(this->_fds.begin() + i);
				}
			}
		}
	}
}

Server::Server(std::string password): _password(password){}

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
	ipport.sin_port = htons(6667);
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

//void Server::start()
//{
//	signal(SIGINT, ft_signal);
//	signal(SIGQUIT, ft_signal);
//	//	Initialisation du socket serveur;
//	int sock = socket(AF_INET, SOCK_STREAM,  IPPROTO_TCP);
//	int opt = 1;
//	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
//	if (sock == -1)
//		std::cerr << "Error : Socket failed to create\n";
	
//	sockaddr_in ipport;
	
//	ipport.sin_family = AF_INET;
//	ipport.sin_port = htons(6667);
//	ipport.sin_addr.s_addr = inet_addr("127.0.0.1");
	
//	if (bind(sock, (struct sockaddr *)&ipport, sizeof(ipport)) == -1)
//	{
//		close(sock);
//		throw BindFailed();
//	}
		
//	if (listen(sock, 10) == -1)
//	{
//		close(sock);
//		throw ListenFailed();
//	}

//	this->_fds.push_back(create_pollfd(sock));
//	//	Boucle infini du serveur qui surveille les connexions et les nouveaux msg 
//	while (!g_stop)
//	{
//		int ret = poll(&this->_fds[0], this->_fds.size(), 100);
//		if (ret > 0)
//		read_client();
//	}
	
//	// Fermeture de tous les sockets;
//	for (size_t i=0; i < this->_fds.size(); i++)
//		close(this->_fds[i].fd);
//}