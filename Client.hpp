#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
// #include "Server.hpp"

//A REVOIR

class Client {
    private :

    // bool _pass;
    int _fd;
    std::string _pass;
    std::string _nick;
    std::string  _user;
    std::string _join;
    // bool _register; //nick et pass

    public :

    Client(int fd);
    Client(std::string pass, std::string nick, std::string user);
    ~Client();

    int getFd() const;
    // bool getPass();
    // void setPass()
    std::string getPass() const;
    std::string getNick() const;
    std::string getUser() const;
    std::string getJoin() const;
    void setPass(const std::string& pass);
    void setNick(const std::string& nick);
    void setUser(const std::string& user);
    void setJoin(const std::string& join);

    
};
#endif