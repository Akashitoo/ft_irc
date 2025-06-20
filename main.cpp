#include "ft_irc.hpp"

int main(int argc, char **argv)
{
	try
	{
		if(argc != 3)
			throw BadArguments();
		std::string pass = argv[2];
		Server server(pass);

		server.start();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error :" << e.what() << '\n';
	}
	(void) argv;
}