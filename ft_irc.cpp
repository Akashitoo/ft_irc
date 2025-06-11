#include "ft_irc.hpp"

int main(int argc, char **argv)
{

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
	std::vector<int> client_fds;
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
			std::cout << "connexion etabli !! \n";
		}
		client_fds.push_back(clientfd);
		client_info.push_back(client);
	}

	for (size_t i=0; i < client_fds.size(); i++)
		close(client_fds[i]);
	close(sock);
	(void) argc;
	(void) argv;
}