#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
// #include "Server.hpp"

//A REVOIR

class Client {
    private :

    int _fd;
    std::string _pass
	std::string _nick;
	std::string _user;
	std::string _hostname;
	std::string _buffer;
    bool _verif;

    public :

    Client(int fd);
	~Client();

	int getFd() const;
	
	std::string& getBuffer();
	void appendToBuffer(const std::string& data);
  
    void setVerif(bool value);
	bool isVerif() const;
    bool getPass();
    // void setPass()
    std::string getUser() const;
    std::string getNick() const;
    std::string getPass() const;
    void setPass(const std::string &pass);
    void setUser(const std::string& user);
    void setNick(const std::string& nick);
    
};
#endif