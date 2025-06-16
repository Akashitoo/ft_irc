#include "ft_irc.hpp"

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
	return (nfd);
}

void add_client(std::vector<struct pollfd>& clt, std::vector<Client>& clients )
{
	int sock = clt[0].fd;
	sockaddr_in client;
	socklen_t client_size = sizeof(client);
	int clientfd = accept(sock, (struct sockaddr *)&client, &client_size);
	Client new_client(clientfd);
    clients.push_back(new_client);
	//la j implemente le client
	char receipt[4096];
	int bytes_receive = recv(clientfd, receipt, sizeof(receipt) -1 , 0);
	std::string test = "Bienvenue sur le serveur !\n";
	send(clientfd, test.c_str(), test.size(), 0);
	if (bytes_receive > 0)
	{
		receipt[bytes_receive] = '\0';
		std::cout << receipt << '\n';

		std::string servername = "monserveur";
		std::string nick = "abalasub";

		std::string cap_ls = ":" + servername + " CAP * LS :\r\n";
		send(clientfd, cap_ls.c_str(), cap_ls.size(), 0);

		std::string welcome = ":" + servername + " 001 " + nick + " :Bienvenue sur le serveur IRC\r\n";
		send(clientfd, welcome.c_str(), welcome.size(), 0);
	}
	std::cout << "connexion etabli !! \n";
	clt.push_back(create_pollfd(clientfd));
}

void read_client(std::vector<struct pollfd>& clt, std::vector<Client>& clients)
{
	
	for(size_t i=0; i < clt.size(); i++)
	{
		if (clt[i].revents & POLLIN)
		{

			if (i == 0)
				add_client(clt, clients);
			else
			{
				char receipt[4096];
				int bytes_receive = recv(clt[i].fd, receipt, sizeof(receipt) -1 , 0);
				
				if (bytes_receive)
				{
					receipt[bytes_receive] = '\0';
					std::cout << receipt << '\n';
				}
				else
					clt.erase(clt.begin() + i);
			}
		}
	}
}



int main(int argc, char **argv)
{
	if(argc == 3)
	{

		signal(SIGINT, ft_signal);
		signal(SIGQUIT, ft_signal);
		//	Initialisation du socket serveur;
		int sock = socket(AF_INET, SOCK_STREAM,  IPPROTO_TCP);
		if (sock == -1)
		std::cerr << "Error : Socket failed to create\n";
		
		sockaddr_in ipport;
		
		ipport.sin_family = AF_INET;
		ipport.sin_port = htons(6667);
		ipport.sin_addr.s_addr = inet_addr("127.0.0.1");
		
		if (bind(sock, (struct sockaddr *)&ipport, sizeof(ipport)) == -1)
		std::cerr << "Error : Failed to bind socket\n";	
		if (listen(sock, 10) == -1)
		std::cerr << "Error : Failed to listen socket\n";
		
		// Liste de tous les sockets 
		std::vector<struct pollfd> fds;
		std::vector<Client> clients;

		fds.push_back(create_pollfd(sock));
		//	Boucle infini du serveur qui surveille les connexions et les nouveaux msg 
		while (!g_stop)
		{
			int ret = poll(&fds[0], fds.size(), 100);
			if (ret > 0)
			read_client(fds, clients);
		}
		
		// Fermeture de tous les sockets;
		for (size_t i=0; i < fds.size(); i++)
		close(fds[i].fd);
		(void) argv;
	}
}