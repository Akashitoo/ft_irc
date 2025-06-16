#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
// #include "Server.hpp"

//A REVOIR

class Client {
    private :

    bool _pass;
    int _fd;
    std::string _nick;
    std::string  _user;
    bool _register; //nick et pass

    public :

    Client();
    ~Client();

    bool getPass();
    // void setPass()
    std::string getUser() const;
    std::string getNick() const;
    void setUser(const std::string& user);
    void setNick(const std::string& nick);
    
};
#endif