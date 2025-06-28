#ifndef CLIENT_HPP
#define CLIENT_HPP

# include "Server.hpp"

//A REVOIR
class Channel;

class Client {
    private :

    int _fd;
    std::string _pass;
	std::string _nick;
	std::string _user;
	// std::string _hostname;
	std::string _buffer;
    bool _verif;
	bool _disconnected;
    std::vector<Channel*> _joinedChannels;

    public :

    Client(int fd);
	~Client();

    bool operator==(const Client& user);
	
	std::string& getBuffer();
	void appendToBuffer(const std::string& data);
    
    //getter
    std::string getPass() const;
    std::string getNick() const;
    std::string getUser() const;
	int getFd() const;
    bool getVerif() const;
    bool getDisconnected() const;
    //setter
    void setPass(const std::string& pass);
    void setUser(const std::string& user);
    void setNick(const std::string& nick);
    // void setJoin(const std::string& join);
    void setVerif(bool value);
    void setDisconnected(bool value);
    void joinChannel(Channel* channel);
    
    std::vector<Channel *> getJoinedChannels();
    
};
#endif