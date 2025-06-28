# include "Server.hpp"

template <typename T>
void pr(T toPr) { std::cout << toPr << std::endl; }

void Server::handlePass(Client *client, const std::string &line)
{
    std::istringstream iss(line.substr(PASS_DELIM));
	std::string pass;
	iss >> pass;

	if (pass.empty())
	{
		std::string error_msg = ":localhost 430 :No password given\r\n";
        send(client->getFd(), error_msg.c_str(), error_msg.size(), 0);
        return;
	}

	client->setPass(pass);

	if (pass == this->_password)
		client->setVerif(true);
	else
	{
		std::string error = "ERROR - Password incorrect\r\n";
		send(client->getFd(), error.c_str(), error.size(), 0);
		// close(client.getFd());
		client->setVerif(false);
	}
}

void Server::handleNick(Client *client, const std::string &line)
{
	 if (!client->getVerif())
	{
		std::string error_msg = ":localhost 430 : NO VALID\r\n";
        send(client->getFd(), error_msg.c_str(), error_msg.size(), 0);
        return;
	}
    std::istringstream iss(line.substr(NICK_DELIM));
    std::string nick;
    iss >> nick;
    if (nick.empty())
    {
        std::string error_msg = ":localhost 431 :No nickname given\r\n";
        send(client->getFd(), error_msg.c_str(), error_msg.size(), 0);
        return;
    }
    bool valid = true;
    for (size_t i = 0; i < nick.length(); i++)
    {
        char c = nick[i];
        if (!isalnum(c) && c != '-' && c != '_' && c != '[' && c != ']' && c != '\\' && c != '`' && c != '^' && c != '{' && c != '}')
        {
            valid = false;
            break;
        }
    }
    if (!valid)
    {
        std::string error_msg = ":localhost 432 " + nick + " :Erroneous nickname\r\n";
        send(client->getFd(), error_msg.c_str(), error_msg.size(), 0);
        return;
    }
    for (size_t i = 0; i < _clients.size(); i++)
    {
        if (_clients[i]->getNick() == nick && _clients[i]->getFd() != client->getFd())
        {
            std::string error_msg = ":localhost 433 * " + nick + " :Nickname is already in use\r\n";
            send(client->getFd(), error_msg.c_str(), error_msg.size(), 0);
            return;
        }
    }
    std::string oldNick = client->getNick();
    client->setNick(nick);
    std::string nick_msg;
    if (oldNick.empty())
        nick_msg = ":" + nick + " NICK :" + nick + "\r\n";
    else
        nick_msg = ":" + oldNick + " NICK :" + nick + "\r\n";
    send(client->getFd(), nick_msg.c_str(), nick_msg.size(), 0);
    checkRegistration(client);
}

void Server::handleUser(Client *client, const std::string &line)
{
    if (!client->getVerif())
	{
		std::string error_msg = ":localhost 430 : NO VALID\r\n";
        send(client->getFd(), error_msg.c_str(), error_msg.size(), 0);
        return;
	}
    std::istringstream iss(line.substr(USER_DELIM));
    std::string username, hostname, servername, realname;
    iss >> username;
    if (username.empty())
    {
        std::string error_msg = ":localhost 461 " + client->getNick() + " USER :Not enough parameters\r\n";
        send(client->getFd(), error_msg.c_str(), error_msg.size(), 0);
        return;
    }
    iss >> hostname >> servername;
    std::string temp;
    if (iss >> temp && temp[0] == ':')
    {
        realname = temp.substr(1);
        std::string rest;
        std::getline(iss, rest);
        realname += rest;
    }
    client->setUser(username);
    checkRegistration(client);
}

void Server::handleJoin(Client *client, const std::string &line)
{
	std::string channel = line.substr(line.find("#") + 1);
	Channel* chan = this->findChannel(channel);
	if (!chan)
	{
		Channel* tmp = new Channel(channel);
		this->_channels.push_back(tmp);
        chan = this->_channels.back();
		chan->addOperator(client);
	}
    client->joinChannel(chan);
	chan->addUser(client);
	chan->sendToUsersNewUser(client);
    chan->printUsers(client);
}

void Server::handlePrivateMessage(Client *client, const std::string &line)
{
	std::size_t dest = line.find(" ");

	if( line[ dest + 1] == '#')
	{
		std::string channel = line.substr(dest + 2, line.find(" ", dest + 1) - (dest + 2));
		Channel* chan = this->findChannel(channel);
		if (!chan)
		{
		}
		std::string msg = line.substr(line.find(" ", dest + 1) + 2);
		chan->sendToUsersMessage(msg , client);
	}
	else
	{
		std::string target = line.substr(dest + 1, line.find(" ", dest + 1) - (dest + 1));
		std::string msg = line.substr(line.find(" ", dest + 1) + 2);
		size_t len = this->_clients.size();
		for (size_t i = 0; i < len; i++)
			if (this->_clients[i]->getNick() == target)
			{
				std::string cmd = ":" + client->getNick() + "!" + client->getUser() + "@localhost PRIVMSG " + target + " :" + msg + "\r\n";
				send(_clients[i]->getFd(), cmd.c_str(), cmd.size(), 0);
				return ;
			}
        std::string errorReply = std::string(ERR_NOSUCHNICK) + target + " :No such nick\r\n";
        send(client->getFd(), errorReply.c_str(), errorReply.size(), 0);
	}
}

void Server::handlePing(Client *client, const std::string &line)
{
	std::string token;
    std::size_t pos = line.find(':');
    if (pos != std::string::npos)
        token = line.substr(pos + 1);
    else
        token = "";
    std::string pong = "PONG :" + token + "\r\n";
    send(client->getFd(), pong.c_str(), pong.size(), 0);
}

void Server::handleKick(Client *client, const std::string &line)
{
    std::istringstream iss(line.substr(KICK_DELIM));

    std::string channel, nick, reason;
    Channel *chan;

    iss >> channel >> nick;

    reason = line.substr(line.find(" :") + 2);
    channel = channel.substr(1);
    chan = findChannel(channel);
    
    if (chan == NULL) 
    {
        std::string errorReply = std::string(ERR_NOSUCHCHANNEL) + "#" + channel + " :No such channel\r\n";
        send(client->getFd(), errorReply.c_str(), errorReply.size(), 0);
        return;
    }
    if (!chan->isOperator(client))
    {
        std::string errorReply = std::string(ERR_CHANOPRIVSNEEDED) + "#" + channel + " :You're not channel operator\r\n";

        std::cout << errorReply << '\n';
        send(client->getFd(), errorReply.c_str(), errorReply.size(), 0);
        return ;
    }
    for (size_t i = 0; i < chan->getUsers().size(); i++)
    {
        if (chan->getUsers()[i]->getNick() == nick)
        {
            Client* kicked;

            kicked = chan->getUsers()[i];
            std::string cmd = ":" + client->getNick() + "!" + client->getUser() + "@localhost KICK #" + channel + " " + chan->getUsers()[i]->getNick() + " :"+ reason + "\r\n";
            chan->sendToUsersCommand(cmd);
            chan->eraseUser(kicked);
            send(kicked->getFd(), cmd.c_str(), cmd.size(), 0);
            return ;
        }
    }
    std::string errorReply = std::string(ERR_NOSUCHNICK) + client->getNick() + " " + nick + " #" + channel +": No such nick\r\n";
    send(client->getFd(), errorReply.c_str(), errorReply.size(), 0);
}

void Server::handleTopic(Client *client, const std::string &line)
{
    // |TOPIC |
    // |TOPIC #channelPARAM :<newTopicPARAM>|
    // |TOPIC #channelPARAM|
    if (line == "TOPIC ") 
    {
        // check if raw command is TOPIC without params with "/topic" in irssi
        std::string errorRPL = ERR_NEEDMOREPARAMS + client->getNick() + " TOPIC :Not enough parameters\r\n";
        send(client->getFd(), errorRPL.c_str(), errorRPL.size(), 0); return;
    }
    std::istringstream iss(line.substr(TOPIC_DELIM));
    std::string channelPARAM; iss >> channelPARAM; if (channelPARAM.at(0) == '#') { channelPARAM.erase(0, 1); }// removing '#' in front of channel if he s here
    Channel *tempChan = findChannel(channelPARAM);
    if (tempChan == NULL)   
    {
        // check no such chann
        std::string errorRPL = ERR_NOSUCHCHANNEL + client->getNick() + " #" + channelPARAM + " :No such channel\r\n";
        send(client->getFd(), errorRPL.c_str(), errorRPL.size(), 0); return;
    }
    else if (!tempChan->isOnChannel(client))
    {
        // check not on chann
        std::string errorRPL = ERR_NOTONCHANNEL + client->getNick() + " #" + channelPARAM + " :You're not on that channel\r\n";
        send(client->getFd(), errorRPL.c_str(), errorRPL.size(), 0); return;
    }
    if (iss.eof())
    {
        // view topics
        std::string RPL;
        if (tempChan->getTopic().empty())
            RPL = RPL_NOTOPIC + client->getNick() + " #" + channelPARAM + " :No topic is set\r\n";
        else
            RPL = RPL_TOPIC + client->getNick() + " #" + channelPARAM + " :" + tempChan->getTopic() + "\r\n";
        send(client->getFd(), RPL.c_str(), RPL.size(), 0);
    }
    else
    {
        // setting topics
        if (!tempChan->isOperator(client))   
        {
            // check trying to change without being operator
            std::string errorRPL = std::string(ERR_CHANOPRIVSNEEDED) + "#" + channelPARAM + " :You're not channel operator\r\n";
            send(client->getFd(), errorRPL.c_str(), errorRPL.size(), 0); return;
        }
        std::string newTopicPARAM = line.substr(line.find(":") + 1);
        tempChan->setTopic(newTopicPARAM);
        std::string RPL = ":" + client->getNick() + "!" + client->getUser() + "@localhost TOPIC #" + channelPARAM + " :" + newTopicPARAM + "\r\n";
        send(client->getFd(), RPL.c_str(), RPL.size(), 0);
    }
}

void Server::handleMode(Client *client, const std::string &line)
{
    // |MODE #channel +o khadj-me|
    // |MODE #channel -m|
    // |MODE khadj-me|

    (void)client;
    (void)line;
}



