#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
// #include "Server.hpp"

//A REVOIR

class Client {
    private :

    int _fd;
    std::string _pass;
	std::string _nick;
	std::string _user;
	// std::string _hostname;
	std::string _buffer;
    bool _verif;

    public :

    Client(int fd);
	~Client();

	
	std::string& getBuffer();
	void appendToBuffer(const std::string& data);
    
    //getter
    std::string getPass() const;
    std::string getNick() const;
    std::string getUser() const;
	int getFd() const;
    bool getVerif() const;
    //setter
    void setPass(const std::string& pass);
    void setUser(const std::string& user);
    void setNick(const std::string& nick);
    // void setJoin(const std::string& join);
    void setVerif(bool value);

    
};
#endif