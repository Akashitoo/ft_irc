#include "Client.hpp"

//mettre un args dans le construc parceque besoin pour le server 
Client::Client(int fd) : _fd(fd), _pass(""), _nick(""), _user(""),
_buffer(""), _verif(false) {
}

Client::~Client() {
	//normalement on close les fd
}

//setter
void Client::setUser(const std::string &user)
{
      _user = user;
}

void Client::setNick(const std::string& nick) {
    _nick = nick;
}

void Client::setPass(const std::string &pass)
{
	_pass = pass;
}
void Client::setVerif(bool value) {
    _verif = value;
}
void Client::setDisconnected(bool value) {
    _disconnected = value;
}
//getter
std::string Client::getPass() const{
	return _pass;
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


bool Client::getVerif() const {
    return _verif;
}

bool Client::getDisconnected() const {
    return _disconnected;
}
//il ajt chaque information du client dans un buffer
void Client::appendToBuffer(const std::string& data) {
	_buffer += data;
}

bool Client::operator==(const Client& user)
{
    return (this->_fd == user.getFd());
}


void Client::joinChannel(Channel* channel)
{
    this->_joinedChannels.push_back(channel);
}

std::vector<Channel *> Client::getJoinedChannels()
{
    return(this->_joinedChannels);
}