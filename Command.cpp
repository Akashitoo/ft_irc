# include "Server.hpp"

std::vector<std::string> split(const std::string& str, char delimiter) 
{
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    
    while (std::getline(iss, token, delimiter))
        if (!token.empty())
            tokens.push_back(token);
    return tokens;
}

void Server::handlePass(Client *client, const std::string &line)
{
    std::istringstream iss(line.substr(PASS_DELIM));
    if (iss.rdbuf()->in_avail() == 0) 
    {
        // check if raw command is TOPIC without params with "/topic" in irssi
        std::string errorRPL = std::string(ERR_NEEDMOREPARAMS) + client->getNick() + " PASS :Not enough parameters\r\n";
        send(client->getFd(), errorRPL.c_str(), errorRPL.size(), 0); return;
    }
	std::string pass;
	iss >> pass;
    // apparement ce rpl 430 est pas commun et pas dans le rfc si j ai bien compris
	// if (pass.empty())
	// {
	// 	std::string error_msg = ":localhost 430 :No password given\r\n";
    //     send(client->getFd(), error_msg.c_str(), error_msg.size(), 0);
    //     return;
	// }

    if (client->getConnected())
    {
		std::string errorRPL = std::string(ERR_ALREADYREGISTRED) + client->getNick() + " :You may not reregister\r\n"; 
        send(client->getFd(), errorRPL.c_str(), errorRPL.size(), 0); return;
    }

	client->setPass(pass);

	if (pass == this->_password)
		client->setVerif(true);
	else
	{
		std::string errorRPL = std::string(ERR_PASSWDMISMATCH) + client->getNick() + " :Password incorrect\r\n";
		send(client->getFd(), errorRPL.c_str(), errorRPL.size(), 0);
		// close(client.getFd());
		client->setVerif(false);
	}
}

void Server::handleNick(Client *client, const std::string &line)
{
    // ERROR RPLS 431 432 433 462
	if (!client->getVerif())
	{
        std::string errorRPL = std::string(ERR_NOTREGISTERED) + "* :You have not registered\r\n";
        send(client->getFd(), errorRPL.c_str(), errorRPL.size(), 0); return;
	}
    std::istringstream iss(line.substr(NICK_DELIM));
    if (iss.rdbuf()->in_avail() == 0) 
    {
        std::string errorRPL = ":localhost 431 * :Not nickname given\r\n";
        send(client->getFd(), errorRPL.c_str(), errorRPL.size(), 0); return;
    }
    std::string nick;
    iss >> nick;
    for (size_t i = 0; i < nick.length(); i++)
    {
        char c = nick[i];
        if (!isalnum(c) && c != '-' && c != '_' && c != '[' && c != ']' && c != '\\' && c != '`' && c != '^' && c != '{' && c != '}')
        {
            std::string error_msg = ":localhost 432 * " + nick + " :Erroneous nickname\r\n";
            send(client->getFd(), error_msg.c_str(), error_msg.size(), 0); return;
        }
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
		std::string errorRPL = std::string(ERR_NOTREGISTERED) + "* :You have not registered\r\n";
        send(client->getFd(), errorRPL.c_str(), errorRPL.size(), 0); return;
	}
    std::istringstream iss(line.substr(USER_DELIM));
    if (iss.rdbuf()->in_avail() == 0)
    {
        std::string error_msg = std::string(ERR_NEEDMOREPARAMS) + client->getNick() + " USER :Not enough parameters\r\n";
        send(client->getFd(), error_msg.c_str(), error_msg.size(), 0);
        return;
    }
    std::string username, hostname, servername, realname;
    iss >> username;
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

    std::string cmd, channels, passwords;
    std::istringstream iss(line);
    iss >> cmd >> channels >> passwords;

    std::vector<std::string> chan_list = split(channels, ',');
    std::vector<std::string> pass_list = split(passwords, ',');

    for (size_t i=0; i < chan_list.size(); i++)
    {
        std::string channel = &chan_list[i][1];

        Channel* chan = this->findChannel(channel);
        if (!chan)
        {
            Channel* tmp = new Channel(channel);
            this->_channels.push_back(tmp);
            chan = this->_channels.back();
            chan->addOperator(client);
        }
        if (!chan->getModes().empty())
        {
            if (chan->getModes().find('k') != std::string::npos)
            {
                if (i >= pass_list.size())
                {
                    std::string errorReply = std::string(ERR_BADCHANNELKEY) + client->getNick() + " #" + channel + " :Bad channel key\r\n";
                    send(client->getFd(), errorReply.c_str(), errorReply.size(), 0);
                    return;
                }
                std::string password = pass_list[i];
                if (password != chan->getPassKey())
                {
                    std::cout << password << " : " << chan->getPassKey() << "\n"; 
                    std::string errorReply = std::string(ERR_BADCHANNELKEY) + client->getNick() + " #" + channel + " :Bad channel key\r\n";
                    send(client->getFd(), errorReply.c_str(), errorReply.size(), 0);
                    return;
                }
            }
            if (chan->getModes().find('i') != std::string::npos)
            {
                if (!chan->isInvited(client))
                {
                    // std::cout << "sdfsdfsdfds\n";
                    std::string errorReply = std::string(ERR_INVITEONLYCHAN) + client->getNick() + " #" + channel + " :You're not invited\r\n";
                    send(client->getFd(), errorReply.c_str(), errorReply.size(), 0);
                    return;
                }
            }
            if (chan->getModes().find('l') != std::string::npos)
            {
                if (chan->getUserLimit() == (int)chan->getUsers().size())
                {
                    std::string errorReply = std::string(ERR_CHANNELISFULL) + client->getNick() + " #" + channel + " :Channel is full\r\n";
                    send(client->getFd(), errorReply.c_str(), errorReply.size(), 0);
                    return;
                }
            }
        }
        if (chan->isOnChannel(client))
        {
            std::string errorReply = std::string(ERR_USERONCHANNEL) + client->getNick() + " #" + channel + " :User is already on channel\r\n";
            send(client->getFd(), errorReply.c_str(), errorReply.size(), 0);
            return;
        }
        client->joinChannel(chan);
        chan->addUser(client);
        chan->sendToUsersNewUser(client);
        chan->printUsers(client);
        chan->printTopic(client);
    }
}

void Server::handlePrivateMessage(Client *client, const std::string &line)
{
	std::size_t dest = line.find(" ");

	if( line[ dest + 1] == '#')
	{
		std::string channel = line.substr(dest + 2, line.find(" ", dest + 1) - (dest + 2));
		Channel* chan = this->findChannel(channel);
		if (!chan)
            return;
        if (!chan->isOnChannel(client))
        {
            std::string errorReply = std::string(ERR_CANNOTSENDTOCHAN) + client->getNick() + " #" + channel + " :Cannot send to channel\r\n";
            send(client->getFd(), errorReply.c_str(), errorReply.size(), 0);
            return;
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
        std::string errorReply = std::string(ERR_NOSUCHNICK) + target + " :No such nick/channel\r\n";
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
            //send(kicked->getFd(), cmd.c_str(), cmd.size(), 0);
            return ;
        }
    }
    std::string errorReply = std::string(ERR_NOSUCHNICK) + "#"+ channel + " :No such nick\r\n";
    send(client->getFd(), errorReply.c_str(), errorReply.size(), 0);
}


void Server::handleTopic(Client* client, const std::string &line)
{
    // |TOPIC |
    // |TOPIC #channelPARAM :<newTopicPARAM>|
    // |TOPIC #channelPARAM|

    std::istringstream iss(line.substr(TOPIC_DELIM));
    if (iss.rdbuf()->in_avail() == 0) 
    {
        // check if raw command is TOPIC without params with "/topic" in irssi
        std::string errorRPL = std::string(ERR_NEEDMOREPARAMS) + client->getNick() + " TOPIC :Not enough parameters\r\n";
        send(client->getFd(), errorRPL.c_str(), errorRPL.size(), 0); return;
    }
    std::string channelPARAM; iss >> channelPARAM; if (channelPARAM.at(0) == '#') { channelPARAM.erase(0, 1); }// removing '#' in front of channel if he s here
    Channel *tempChan = findChannel(channelPARAM);
    if (tempChan == NULL)   
    {
        // check no such chann
        std::string errorRPL = std::string(ERR_NOSUCHCHANNEL) + client->getNick() + " #" + channelPARAM + " :No such channel\r\n";
        send(client->getFd(), errorRPL.c_str(), errorRPL.size(), 0); return;
    }
    else if (!tempChan->isOnChannel(client))
    {
        // check not on chann
        std::string errorRPL = std::string(ERR_NOTONCHANNEL) + client->getNick() + " #" + channelPARAM + " :You're not on that channel\r\n";
        send(client->getFd(), errorRPL.c_str(), errorRPL.size(), 0); return;
    }
    if (iss.rdbuf()->in_avail() == 0)
    {
        std::cout << "SALAM\n";
        // view topics
        std::string RPL;
        if (tempChan->getTopic().empty())
            RPL = std::string(RPL_NOTOPIC) + client->getNick() + " #" + channelPARAM + " :No topic is set\r\n";
        else
            RPL = std::string(RPL_TOPIC) + client->getNick() + " #" + channelPARAM + " :" + tempChan->getTopic() + "\r\n";
        send(client->getFd(), RPL.c_str(), RPL.size(), 0);
    }
    else
    {
        // setting topics
        if (!tempChan->isOperator(client) && tempChan->getTopicOpOnly())   
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
    // |MODE <#channelPARAM> +i| or - change invite-only channel status
    // |MODE <#channelPARAM> +t| or - topic access protection
    // |MODE <#channelPARAM> +k <key or password>| or - change keypass
    // |MODE <#channelPARAM> +o <nick>| or - modify a users operator state
    // |MODE <#channelPARAM> +l <limit>| or - manage channel user limit

    std::istringstream iss(line.substr(MODE_DELIM));
    if (iss.rdbuf()->in_avail() == 0) 
    {
        std::string errorRPL = std::string(ERR_NEEDMOREPARAMS) + client->getNick() + " MODE :Not enough parameters\r\n";
        send(client->getFd(), errorRPL.c_str(), errorRPL.size(), 0); return;
    }
    std::string channelPARAM; iss >> channelPARAM; if (channelPARAM.at(0) == '#') { channelPARAM.erase(0, 1); } else return;// removing '#' in front of channel if he s here
    Channel *tempChan = findChannel(channelPARAM);
    if (iss.rdbuf()->in_avail() == 0) 
    {
        std::string errorRPL = std::string(ERR_NEEDMOREPARAMS) + client->getNick() + " MODE :Not enough parameters\r\n";
        send(client->getFd(), errorRPL.c_str(), errorRPL.size(), 0); return;
    }
    if (tempChan == NULL)   
    {
        // check no such chann
        std::string errorRPL = std::string(ERR_NOSUCHCHANNEL) + client->getNick() + " #" + channelPARAM + " :No such channel\r\n";
        send(client->getFd(), errorRPL.c_str(), errorRPL.size(), 0); return;
    }
    else if (!tempChan->isOnChannel(client))
    {
        // check not on chann
        std::string errorRPL = std::string(ERR_NOTONCHANNEL) + client->getNick() + " #" + channelPARAM + " :You're not on that channel\r\n";
        send(client->getFd(), errorRPL.c_str(), errorRPL.size(), 0); return;
    }
    if (iss.rdbuf()->in_avail() == 0)
    {
        std::string RPL = std::string(RPL_CHANNELMODEIS) + client->getNick() + " #" + channelPARAM + " " + tempChan->getModes() + "\r\n";
        send(client->getFd(), RPL.c_str(), RPL.size(), 0); return;
    }
    if (!tempChan->isOperator(client))
    {
        // check if operator
        std::string errorRPL = std::string(ERR_CHANOPRIVSNEEDED) + client->getNick() + " #" + channelPARAM + " :You're not channel operator\r\n";
        send(client->getFd(), errorRPL.c_str(), errorRPL.size(), 0); return;
    }
    std::string flagPARAM; iss >> flagPARAM;
    for (size_t i = 1; i < flagPARAM.size(); i++)
    {
        switch (flagPARAM[i])
        {
            case I:
                if (flagPARAM[0] == '+')
                    tempChan->setInviteOnly(true);
                else
                    tempChan->setInviteOnly(false);
                tempChan->setModes(flagPARAM[0] == '+', flagPARAM[i]);
                break;
            case T: 
                if (flagPARAM[0] == '+')
                    tempChan->setTopicChOnly(true);
                else
                    tempChan->setTopicChOnly(false);
                tempChan->setModes(flagPARAM[0] == '+', flagPARAM[i]);
                break;
            case K:
            {
                if (iss.rdbuf()->in_avail() == 0)
                {
                    // check if no params left
                    std::string errorRPL = std::string(ERR_NEEDMOREPARAMS) + client->getNick() + " MODE :Not enough parameters\r\n";
                    send(client->getFd(), errorRPL.c_str(), errorRPL.size(), 0); return;
                }
                std::string newPassKey; iss >> newPassKey;
                if (!tempChan->getPassKey().empty())
                {
                    // check if passkey already set
                    std::string errorRPL = std::string(ERR_KEYSET) + client->getNick() + " #" + channelPARAM + " :Channel key already set\r\n";
                    send(client->getFd(), errorRPL.c_str(), errorRPL.size(), 0); return;
                }
                if (flagPARAM[0] == '+')
                    tempChan->setPassKey(newPassKey); 
                else
                    tempChan->setPassKey("");
                tempChan->setModes(flagPARAM[0] == '+', flagPARAM[i]);
                break;
            }
            case O: 
            {
                if (iss.rdbuf()->in_avail() == 0)
                {
                    // check if no params left
                    std::string errorRPL = std::string(ERR_NEEDMOREPARAMS) + client->getNick() + " MODE :Not enough parameters\r\n";
                    send(client->getFd(), errorRPL.c_str(), errorRPL.size(), 0); return;
                }
                std::string nickToUpd; iss >> nickToUpd;
                Client *tempCli = findClient(nickToUpd);
                if (tempCli == NULL)
                {
                    std::string errorRPL = std::string(ERR_NOSUCHNICK) + client->getNick() + " " + nickToUpd + " :No such nick/channel\r\n";
                    send(client->getFd(), errorRPL.c_str(), errorRPL.size(), 0); return;
                }
                if (!tempChan->isOnChannel(tempCli)) // a la place du null mettre un get du user dans la liste de clients du server
                {
                    // check si le nick en parametre est dans le channel
                    std::string errorRPL = std::string(ERR_USERNOTINCHANNEL) + client->getNick() + " " + nickToUpd + " #" + channelPARAM + " :User not in channel\r\n";
                    send(client->getFd(), errorRPL.c_str(), errorRPL.size(), 0); return;
                }
                tempChan->addOperator(tempCli);
                tempChan->setModes(flagPARAM[0] == '+', flagPARAM[i]);
                break;
            }
            case L: 
            {
                if (flagPARAM[0] == '+')
                {
                    if (iss.rdbuf()->in_avail() == 0)
                    {
                        // check if no params left
                        std::string errorRPL = std::string(ERR_NEEDMOREPARAMS) + client->getNick() + " MODE :Not enough parameters\r\n";
                        send(client->getFd(), errorRPL.c_str(), errorRPL.size(), 0); return;
                    }
                    int newUserLimit; iss >> newUserLimit; 
                    tempChan->setUserLimit(newUserLimit);
                }
                else
                    tempChan->setUserLimit(NO_USER_LIMIT); // vu que la userlimit peut pas etre negative alors on la met a -1 pour indiquer l absence de limit
                tempChan->setModes(flagPARAM[0] == '+', flagPARAM[i]);
                break;
            }
            default:
            {
                std::string errorRPL = std::string(ERR_UNKNOWNMODE) + client->getNick() + " " + flagPARAM[i] + " :is unknown mode char to me\r\n";
                send(client->getFd(), errorRPL.c_str(), errorRPL.size(), 0);
                break;
            }

        }
    }
    std::string finalFlags; finalFlags += flagPARAM[0];
    for (size_t i = 0; i < flagPARAM.size(); i++)
        if (std::string("itkol").find(flagPARAM[i]) != std::string::npos)
            finalFlags += flagPARAM[i];
    std::string RPL = ":" + client->getNick() + "!" + client->getUser() + "@localhost MODE #" + channelPARAM + " :" + finalFlags + "\r\n";
    send(client->getFd(), RPL.c_str(), RPL.size(), 0);
}

void Server::handleInvite(Client *client, const std::string &line)
{
    std::istringstream iss(line.substr(7));
    std::string target;
    std::string channel;
    iss >> target >> channel;

    if(channel[0] == '#')
        channel = channel.substr(1);
    //gere si le chan existe
    Channel *chan = findChannel(channel);
    if (!chan)
    {
        std::string errorReply = std::string(ERR_NOSUCHCHANNEL) + client->getNick() + " " + channel + " :No such channel\r\n";
        send(client->getFd(), errorReply.c_str(), errorReply.size(), 0); return;
    }
    //gere si le client qui invite existe
    if(!chan->isOnChannel(client))
    {
        std::string errorReply = std::string(ERR_NOTONCHANNEL) + client->getNick() + " " + channel + " :You're not on that channel\r\n";
        send(client->getFd(), errorReply.c_str(), errorReply.size(), 0); return;
    }
     std::vector<Client*> targets_client;
    for (size_t i = 0; i < _clients.size(); i++)
    {
        if (_clients[i]->getNick() == target)
            targets_client.push_back(_clients[i]);
    }
    //client existe pas
    if (targets_client.empty())
    {
        std::string errorReply = std::string(ERR_NOSUCHNICK) + client->getNick() + " " + target + " :No such nick\r\n";
        send(client->getFd(), errorReply.c_str(), errorReply.size(), 0);
        return;
    }

    // Utiliser le premier client trouvé et verif si deja dans le channel
    Client* targetClient = targets_client[0];
    if (chan->isOnChannel(targetClient))
    {
        std::string errorReply = std::string(ERR_USERONCHANNEL) + client->getNick() + " " + target + " " + channel + " :is already on channel\r\n";
        send(client->getFd(), errorReply.c_str(), errorReply.size(), 0);
        return;
    }

    std::string inviteMsg = ":" + client->getNick() + "!" + client->getUser() + "@localhost INVITE " + target + " :#" + channel + "\r\n";
    send(targetClient->getFd(), inviteMsg.c_str(), inviteMsg.size(), 0);

    std::string confirmMsg = std::string(RPL_INVITING) + client->getNick() + " " + target + " " + channel + "\r\n";
    send(client->getFd(), confirmMsg.c_str(), confirmMsg.size(), 0);
    chan->addUser(targetClient);
}


void Server::handleQuit(Client* client, const std::string &line)
{
    std::string reason = line.substr(6);
    std::string cmd = ":" + client->getNick() + "!" + client->getUser() + "@localhost QUIT :" + reason + "\r\n";

    // Obtenir une copie locale des channels pour éviter les problèmes d'itérateurs
    std::vector<Channel*> joinedChannels = client->getJoinedChannels();
    
    // Envoyer le message QUIT à tous les channels où le client était présent
    for (std::vector<Channel*>::iterator it = joinedChannels.begin(); it != joinedChannels.end(); it++)
    {
        (*it)->eraseUser(client);
        (*it)->sendToUsersCommand(cmd);
    }
    eraseUserServer(client);
    close(client->getFd());

}

void Server::handlePart(Client *client, const std::string &line)
{
    size_t pos = line.find('#');
    if(pos == std::string::npos)
        return ;
    std::string channel_part = line.substr(pos + 1);
    Channel *chan = this->findChannel(channel_part);
    
    if(!chan)
    {
        std::string errorReply = std::string(ERR_NOSUCHCHANNEL) + client->getNick() + " " + channel_part + " :No such channel\r\n";
        send(client->getFd(), errorReply.c_str(), errorReply.size(), 0);
        return;
    }

    bool found = false;
    std::vector<Client*> users = chan->getUsers();
    for(size_t i = 0; i < users.size(); i++) {
        if(users[i] == client) {
            found = true;
            break;
        }
    }
    if(!found)
    {
        std::string errorReply = std::string(ERR_NOTONCHANNEL) + client->getNick() + " " + channel_part + " :You're not on that channel\r\n";
        send(client->getFd(), errorReply.c_str(), errorReply.size(), 0);
        return;
    }

    std::string re;
    size_t rpos = line.find(':', pos);
    if(rpos != std::string::npos)
        re = line.substr(rpos + 1);
    else
    {
        std::string msg = ":" + client->getNick() + "!" + client->getUser() + "@localhost PART #" + channel_part + "\r\n";
        chan->sendToUsersCommand(msg);
        chan->eraseUser(client); return;
    }
    //else
    //    re = "SALAM3LIKOM";

    std::string msg = ":" + client->getNick() + "!" + client->getUser() + "@localhost PART #" + channel_part + " :" + re + "\r\n";
    chan->sendToUsersCommand(msg);
    chan->eraseUser(client);
}
