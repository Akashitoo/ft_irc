#include "Client.hpp"

// a revoir 


Client::Client(int fd) : _fd(fd) {}

Client::~Client() {}



int Client::getFd() const {
    return _fd;
}
// bool Client::getPass() const
// {
    
// }
// 	void setAuthenticated(bool value);

// void Client::setUser(const std::string &user) 
// {
//      _user = user;
// }
// void Client::setNick(const std::string& nick) {
//     _nick = nick;
// }

// std::string Client::getUser() const{ 
//     return _user; 
// }
// std::string Client::getNick() const {
//     return _nick;
// }