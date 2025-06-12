#include "ft_irc.hpp"


void read_client(std::vector<struct pollfd>& clt)
{
	for(size_t i=0; i < clt.size(); i++)
	{
		if (clt[i].revents & POLLIN)
		{
			char receipt[4096];
			int bytes_receive = recv(clt[i].fd, receipt, sizeof(receipt) -1 , 0);
			
			if (bytes_receive)
			{
				receipt[bytes_receive] = '\0';
				std::cout << receipt << '\n';
			}
			else
			{
				std::cerr << "Error\n";
			}
		}
	}
}

int main(int argc, char **argv)
{

	int sock = socket(AF_INET, SOCK_STREAM,  IPPROTO_TCP);
	int dd = 1;
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&dd, sizeof(int));
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
	std::vector<struct pollfd> client_fds;
	std::vector<struct sockaddr_in> client_info;
	while (true)
	{
		sockaddr_in client;
		socklen_t client_size = sizeof(client);
		int clientfd = accept(sock, (struct sockaddr *)&client, &client_size);
		if (clientfd == -1)
			std::cerr << "a client failed to connect \n";
		else
		{
			char receipt[4096];
			int bytes_receive = recv(clientfd, receipt, sizeof(receipt) -1 , 0);
			std::string test = "Bienvenue sur le serveur !\n";
			send(clientfd, test.c_str(), test.size(), 0);
			int flag = 1;
			setsockopt(clientfd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
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
			struct	pollfd nfd;
			nfd.fd = clientfd;
			nfd.events = POLLIN;
			client_fds.push_back(nfd);
		}
		int ret = poll(&client_fds[0], client_fds.size(), 100);
		if (ret > 0)
		{
			std::cout << "un client a envoye un msg \n";
			read_client(client_fds);
		}
		std::cout << "tour de boucle\n";
	}

	for (size_t i=0; i < client_fds.size(); i++)
		close(client_fds[i].fd);
	close(sock);
	(void) argc;
	(void) argv;
}