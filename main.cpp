// #include "Server.hpp"
// 
// int main(int argc, char **argv)
// {
// 	if(argc == 3)
// 	{
//         Server sv;
// 		signal(SIGINT, sv.ft_signal);
// 		signal(SIGQUIT, sv.ft_signal);
// 		//	Initialisation du socket serveur;
// 		int sock = socket(AF_INET, SOCK_STREAM,  IPPROTO_TCP);
// 		if (sock == -1)
// 		std::cerr << "Error : Socket failed to create\n";
		
// 		sockaddr_in ipport;
		
// 		ipport.sin_family = AF_INET;
// 		ipport.sin_port = htons(6667);
// 		ipport.sin_addr.s_addr = inet_addr("127.0.0.1");
		
// 		if (bind(sock, (struct sockaddr *)&ipport, sizeof(ipport)) == -1)
// 		std::cerr << "Error : Failed to bind socket\n";	
// 		if (listen(sock, 10) == -1)
// 		std::cerr << "Error : Failed to listen socket\n";
		
// 		// Liste de tous les sockets 
// 		std::vector<struct pollfd> fds;
// 		fds.push_back(create_pollfd(sock));
// 		//	Boucle infini du serveur qui surveille les connexions et les nouveaux msg 
// 		// j ainchanger la boucle infini je coupes des que le signal est recu #sirine
// 		while (!g_stop)
// 		{
// 			int ret = poll(&fds[0], fds.size(), 100);
// 			if (ret > 0)
// 			read_client(fds);
// 		}
		
// 		// Fermeture de tous les sockets;
// 		for (size_t i=0; i < fds.size(); i++)
// 		close(fds[i].fd);
// 		(void) argv;
// 	}
// }