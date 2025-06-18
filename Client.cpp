#include "Client.hpp"

//mettre un args dans le construc parceque besoin pour le server 
Client::Client(int fd) : _fd(fd), _nick(""), _user(""), _pass(""),
_buffer(""), _verif(false) {
}

Client::~Client() {
	close(_fd);
}

void Client::setUser(const std::string &user) 
{
      _user = user;
}

void Client::setNick(const std::string& nick) {
    _nick = nick;
}

void Client::setPass(const std::string &pass)
{
	_password = pass;
}

std::string Client::getPass() const
{
	return _password;
}

std::string Client::getUser() const{ 
     return _user; 
}
std::string Client::getNick() const {
     return _nick;
}


int Client::getFd() const {
	return _fd;
}

std::string& Client::getBuffer() {
	return _buffer;
}

void Client::setVerif(bool value) {
    _verif = value;
}

bool Client::getVerif() const {
    return _verif;
}
//il ajt chaque information du client dans un buffer
void Client::appendToBuffer(const std::string& data) {
	_buffer += data;
}