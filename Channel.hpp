#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "ft_irc.hpp"

class Channel
{

	private :

		std::string				_name;
		std::vector<Client>	_users;
		std::vector<Client>	_operators;
	
	public :

		Channel(std::string name);
		Channel(const Channel& src);
		Channel& operatot=(const Channel& src);
		~Channel();
};

#endif