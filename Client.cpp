#include "Client.hpp"

// a revoir 

Client::Client(int fd) : _fd(fd)
{

}
Client::Client(std::string pass, std::string nick, std::string user) : _pass(pass), _nick(nick), _user(user)  {}

Client::~Client() {}



int Client::getFd() const {
    return _fd;
}
// bool Client::getPass() const
// {
    
// }
// 	void setAuthenticated(bool value);
void Client::setPass(const std::string &pass)
{
    _pass = pass;
}
void Client::setUser(const std::string &user) 
{
     _user = user;
}
void Client::setNick(const std::string& nick) {
    _nick = nick;
}

std::string Client::getPass() const{
    return _pass;
}
std::string Client::getUser() const{ 
    return _user; 
}
std::string Client::getNick() const {
    return _nick;
}