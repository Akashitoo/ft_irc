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

	close(sock);
	(void) argc;
	(void) argv;
}