/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oozsipah <oozsipah@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/16 22:46:35 by oozsipah          #+#    #+#             */
/*   Updated: 2026/09/16 03:54:41 by oozsipah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Commands.hpp"
#include "../inc/Server/Server.hpp"
#include <sstream>
#include <climits>
#include <cstdlib>
#include <sys/socket.h>

Channel *createChannel(std::string &name, std::string &key)
{

    Channel *dummy =new Channel(name, key);
    return (dummy);
}

void    addChannel(Channel *chnl, Client *client, TManager<std::string, Channel *> &channels)
{

    chnl->addMember(client);
    channels.add(chnl->getName(), chnl);
}

void    removeChannel(Channel *chnl, TManager<std::string, Channel *> &channels)
{
    channels.remove(chnl->getName());
}

std::vector<std::string> splitString(const std::string &str, char delimiter)
{
    std::vector<std::string> strs;
    std::string buff;
    std::istringstream TokenStream(str);
    
    while (std::getline(TokenStream, buff, delimiter))
    {
        if (!buff.empty())
            strs.push_back(buff);
    }
    return (strs);
}

Client      *getClient(std::string targetName, TManager<int, Client *> &clients)
{
    std::map<int, Client *> allClients = clients.getAll();
    Client *target = NULL;
    for (std::map<int, Client *>::iterator it = allClients.begin(); it != allClients.end(); it++)
    {
        target = it->second;
        if  (target->getNickname() == targetName) 
            return (target);
    }
    return (NULL);
}


void    cmdJoin(Client *client, struct Command cmd, TManager<std::string, Channel *> &channels)
{
    if (cmd.params.empty())
    {
        errNeedMoreParams(client, cmd.type);
        return ;
    }

    std::vector<std::string> targetChannels = splitString(*cmd.params.begin(), ',');
    std::vector<std::string> targetChannelKeys;
    if (cmd.params.size() > 1)
        targetChannelKeys = splitString(*(cmd.params.begin() + 1), ',');

    for (size_t i = 0; i < targetChannels.size(); i++)
    {
        std::string channelName = targetChannels[i];
        std::string channelKey = i < targetChannelKeys.size() ? targetChannelKeys[i] : "";

        if (channelName.empty() || ((channelName[0] != '#') && (channelName[0] != '&')))
        {
            errNoSuchChannel(client, channelName);
            continue;
        }

        Channel *chnl;
        if (!channels.exists(channelName))
        {
            if (client->getChannelCount() >= MAX_CHANNEL_COUNT)
            {
                errTooManyChannels(client, channelName);
                continue;
            }

            chnl = createChannel(channelName, channelKey);
            chnl->addMember(client);
            chnl->addOperator(client);
            client->incrementChannelCount();
            channels.add(channelName, chnl);
        }
        else
        {
            chnl = channels.get(channelName);

            if (chnl->isMember(client))
                continue;

            if (!chnl->getKey().empty() && (chnl->getKey() != channelKey))
            {
                errBadChannelKey(client, channelName);
                continue;
            }
            if (chnl->isInviteOnly() && !chnl->isInvite(client))
            {
                errInviteOnlyChan(client, channelName);
                continue;
            }
            if (chnl->isUserLimit() && (chnl->getUserLimit() == chnl->getMemberList().size()))
            {
                errChannelIsFull(client, channelName);
                continue;
            }
            if (client->getChannelCount() >= MAX_CHANNEL_COUNT)
            {
                errTooManyChannels(client, channelName);
                continue;
            }

            chnl->addMember(client);
            client->incrementChannelCount();
        }

        std::string userPrefix = ":" + client->getNickname() + "!~" + client->getUsername() + "@" + client->getIp();
        
        std::string joinMsg = userPrefix + " JOIN :" + channelName + "\r\n";
        chnl->broadcast(joinMsg);

        std::string chnlTopic = chnl->getTopic();
        if (chnlTopic.empty())
            rplNoTopic(client, channelName);
        else
            rplTopic(client, channelName, chnlTopic);

        std::string namesList = "";
        std::vector<Client*> members = chnl->getMemberList();
        for (size_t k = 0; k < members.size(); k++)
        {
            if (chnl->isOperator(members[k]))
                namesList += "@";
                
            namesList += members[k]->getNickname();
            
            if (k != members.size() - 1)
                namesList += " ";
        }

        std::stringstream ss;
        ss << ":server 353 " << client->getNickname() << " = " << channelName << " :" << namesList << "\r\n";
        ss << ":server 366 " << client->getNickname() << " " << channelName << " :End of /NAMES list.\r\n";
        
        client->appendToWriteBuffer(ss.str());

        if (Server::getInstance() != NULL)
            Server::getInstance()->enableWriteEvent(client->getFd());    
    }
}

void        cmdPrivMsg(Client *client, struct Command cmd, TManager<int, Client *> &clients, TManager<std::string, Channel *> &channels)
{
    if (cmd.params.size() < 1)
    {
        errNoRecipient(client, cmd.type);
        return ;
    }
    if (cmd.message.empty())
    {
        errNoTextToSend(client);
        return ;
    }
    Channel *chnl = NULL;
    Client  *target = NULL;

    std::vector<std::string> targets = splitString(cmd.params[0], ',');

    for (std::vector<std::string>::iterator it = targets.begin(); it < targets.end(); it++)
    {
        for (std::vector<std::string>::iterator it_2 = it + 1; it_2 < targets.end(); it_2++)
        {
            if (*it == *it_2)
            {
                errTooManyTargets(client, *it);
                return ;
            }
        }
    }
    
    for (std::vector<std::string>::iterator it = targets.begin(); it < targets.end(); it++)
    {
		chnl = NULL;
		target = NULL;
        if ((*it)[0] == '#' || (*it)[0] == '&')
        {
            try
            {
                chnl = channels.get((*it));
                if (!chnl->isMember(client))
                {
                    errCannotSendToChan(client, chnl->getName());
                    continue;
                }
                goto send_msg;
            }
            catch (const std::exception &e)
            {
                errNoSuchChannel(client, *it);
                continue ;
            }
        }
        target = getClient((*it), clients);
        if (!target)
        {
            errNoSuchNick(client, *it);
            continue;
        }

        send_msg:
        std::string msg = ":" + client->getMask() + " " + cmd.type + " " + ((chnl == NULL) ? target->getNickname() : chnl->getName()) + " " + cmd.message + "\r\n";
        
        if (chnl != NULL)
        {
            chnl->broadcast(msg, client);
            continue;
        }
        else
            target->appendToWriteBuffer(msg);
        if (Server::getInstance() != NULL)
            Server::getInstance()->enableWriteEvent(target->getFd());
    }
}


void cmdTopic(Client *client, struct Command cmd, TManager<std::string, Channel *> &channels)
{
    if (cmd.params.size() < 1)
    {
        errNeedMoreParams(client, cmd.type);
        if (Server::getInstance() != NULL)
            Server::getInstance()->enableWriteEvent(client->getFd());
        return;
    }

    std::string channelName = cmd.params[0];
    Channel *target = NULL;

    try
    {
        target = channels.get(channelName); 
    }
    catch (const std::exception &e)
    {
        errNoSuchChannel(client, channelName);
        if (Server::getInstance() != NULL)
            Server::getInstance()->enableWriteEvent(client->getFd());
        return;
    }
    if (!target->isMember(client))
    {
        errNotOnChannel(client, channelName);
        if (Server::getInstance() != NULL)
            Server::getInstance()->enableWriteEvent(client->getFd());
        return;
    }

    bool isViewing = (cmd.params.size() == 1 && cmd.message.empty());

    if (isViewing)
    {
        if (target->getTopic().empty())
            rplNoTopic(client, channelName);
        else
            rplTopic(client, channelName, target->getTopic());
        
        if (Server::getInstance() != NULL)
            Server::getInstance()->enableWriteEvent(client->getFd());
    }
    else
    {
        std::string newTopic = "";
        if (!cmd.message.empty())
            newTopic = cmd.message;
        else if (cmd.params.size() > 1)
            newTopic = cmd.params[1];
        
        if (target->isTopicRestricted() && !target->isOperator(client))
        {
            errChanOprivsNeeded(client, channelName);
            if (Server::getInstance() != NULL)
                Server::getInstance()->enableWriteEvent(client->getFd());
            return;
        }
    

        target->setTopic(newTopic);
        std::string senderMask = client->getNickname() + "!~" + client->getUsername() + "@" + client->getIp();
        std::string broadcastMsg = ":" + senderMask + " TOPIC " + channelName + " :" + newTopic + "\r\n";

        target->broadcast(broadcastMsg, NULL);
    }
}

void cmdQuit(Client *client, struct Command cmd, TManager<std::string, Channel *> &channels)
{
    std::string reason = "Client exited";
    if (!cmd.message.empty())
        reason = cmd.message;
    else if (cmd.params.size() > 0)
        reason = cmd.params[0];

    std::string senderMask = client->getNickname() + "!~" + client->getUsername() + "@" + client->getIp();
    std::string quitMsg = ":" + senderMask + " QUIT :" + reason + "\r\n";

    std::map<std::string, Channel*> &allChannels = channels.getAll();
    for (std::map<std::string, Channel*>::iterator it = allChannels.begin(); it != allChannels.end(); ++it)
    {
        Channel *chnl = it->second;

        if (chnl->isMember(client))
        {
            chnl->broadcast(quitMsg, client);
            
            std::vector<Client*> members = chnl->getMemberList();
            for (size_t i = 0; i < members.size(); i++)
            {
                if (members[i]->getFd() != client->getFd() && Server::getInstance() != NULL)
                {
                    Server::getInstance()->enableWriteEvent(members[i]->getFd());
                }
            }
        }
    }
	Channel::deleteClientFromAllChannels(client, channels);
}

void        cmdList(TManager<int, Client *> &clients)
{
    std::map<int, Client *> allClients = clients.getAll();
    for (std::map<int, Client *>::iterator it = allClients.begin(); it != allClients.end(); it++)
    {
        Client *target = it->second;
        std::cout << "Nickname = " + target->getNickname() << " FD = " << target->getFd() << std::endl;
    }
}

void        cmdKick(Client *client, struct Command cmd, TManager<int, Client *> &clients, TManager<std::string, Channel *> &channels)
{
	if (cmd.params.size() < 2)
	{
		errNeedMoreParams(client, cmd.type);
		return ;
	}
    std::vector<std::string> targetChannels = splitString(*(cmd.params.begin()), ',');
    std::vector<std::string> targets;
	if (cmd.params.size() > 1)
        targets = splitString(*(cmd.params.begin() + 1), ',');   

	if (targetChannels.size() != targets.size() && targetChannels.size() != 1 && targets.size() != 1)
	{
		errNeedMoreParams(client, cmd.type);
		return ;
	}

	size_t	loopIter = (targetChannels.size() > targets.size()) ? targetChannels.size() : targets.size();

    for (size_t i = 0; i < loopIter; i++)
    {
        
        std::string channelName = (targetChannels.size() == 1) ? targetChannels[0] : targetChannels[i];
        std::string targetName = (targets.size() == 1) ? targets[0] : targets[i];
        if (channelName.empty() || (channelName[0] != '#' && channelName[0] != '&'))
        {
            errNoSuchChannel(client, channelName);
            continue; ;
        }
        Channel *chnl;
        try
        {
            chnl = channels.get(channelName); // verdiği format doğru olabilir ama channel olmayabilir.
        }
        catch(const std::exception& e)
        {
            errNoSuchChannel(client, channelName);
            continue;
        }

        Client  *target = getClient((targets.size() == 1) ? targets[0] : targets[i], clients);
        if (!chnl->isMember(client))
        {
            errNotOnChannel(client, channelName);
            continue; ;
        }
        if (!chnl->isOperator(client))
        {
            errChanOprivsNeeded(client, channelName);
            continue; ;
        }
        if (!chnl->isMember(target))
        {
            errUserNotInChannel(client, targetName, channelName);
            continue; ;
        }
        std::string kickerMask = client->getNickname() + "!~" + client->getUsername() + "@127.0.0.1";
        std::string comment;
        if (!cmd.message.empty())
            comment = cmd.message;
        kickMsg(chnl, kickerMask, targetName, comment);
		target->decrementChannelCount();
        chnl->removeMember(target);

		if (chnl->getMemberList().empty())
		{
			channels.remove(channelName);
			delete chnl;
		}
    }
}

void    cmdInvite(Client *client, struct Command cmd, TManager<int, Client *> &clients, TManager<std::string, Channel *> &channels)
{
    if (cmd.params.size() != 2)
    {
        errNeedMoreParams(client, cmd.type);
        return ;
    }

    if (!channels.exists(cmd.params[1]))
    {
        errNoSuchChannel(client, cmd.params[1]);
        return ;
    }
    
    Channel *chnl = channels.get(cmd.params[1]);
    Client  *target = getClient(cmd.params[0], clients);

    if (!target)
    {
        errNoSuchNick(client, cmd.params[0]);
        return ;
    }
    if (!chnl->isMember(client))
    {
        errNotOnChannel(client, chnl->getName());
        return ;
    }
    if (!chnl->isOperator(client))
    {
        errChanOprivsNeeded(client, chnl->getName());
        return ;
    }
    if (chnl->isMember(target))
    {
        errUserOnChannel(client, target->getNickname(), chnl->getName());
        return ;
    }

    chnl->addInvite(target);

    std::string targetNick = target->getNickname();
    std::string chnlName = chnl->getName();
    rplInviting(client, targetNick, chnlName);

    // std::string senderMask = client->getNickname() + "!~" + client->getUsername() + "@" + client->getIp();
    std::string inviteMsg = ":" + client->getMask() + " INVITE " + target->getNickname() + " " + chnl->getName() + "\r\n";
    target->appendToWriteBuffer(inviteMsg);

	if (Server::getInstance() != NULL) {
	    Server::getInstance()->enableWriteEvent(target->getFd());
	}
}


static void    channelModeInvite(Client *client, Channel *chnl, bool setFlag)
{
    bool before = chnl->isInviteOnly();
    
    if (setFlag == true)
        chnl->setInviteOnly(true);
    else
        chnl->setInviteOnly(false);
    if (before != chnl->isInviteOnly())
    {
        if (setFlag == true)
            chnl->addMode('i');
        else
            chnl->removeMode('i');
        std::string modeMsg = ":" + client->getMask() + " MODE " + chnl->getName() + " " + ((setFlag == true) ? "+" : "-") + "i\r\n";
        chnl->broadcast(modeMsg);
    }
}

static void    channelModeOp(Client *client, std::string param, Channel *chnl, TManager<int, Client *> &clients, bool setFlag)
{
    Client *target = getClient(param, clients);
    
    if (!target)
    {
        errNoSuchNick(client, param);
        return ;
    }
    if (!chnl->isMember(target))
    {
        errUserNotInChannel(client, target->getNickname(), chnl->getName());
        return ;
    }
    if (setFlag)
        chnl->addOperator(target);
    else
        chnl->removeOperator(target);
    std::string opMsg = ":" + client->getMask() + " MODE " + chnl->getName() + " " + ((setFlag == true) ? "+" : "-") + target->getNickname() + "o\r\n";
    chnl->broadcast(opMsg);
}

void    channelModeKey(Client *client, std::string &param, Channel *chnl, bool setFlag)
{
    std::string keyMsg;
    if (!setFlag)
    {
        keyMsg = ":" + client->getMask() + " MODE " + chnl->getName() + " -k " + chnl->getKey() + "\r\n"; 
        chnl->setKey("");
        chnl->removeMode('k');
    }
    else
    {
        if (!chnl->getKey().empty())
        {
            errKeySet(client, chnl->getName());
            return ;
        }
        chnl->setKey(param);
        chnl->addMode('k');
        keyMsg = ":" + client->getMask() + " MODE " + chnl->getName() + " +k " + chnl->getKey() + "\r\n"; 
    }
    chnl->broadcast(keyMsg);
}

void    channelModeLimit(Client *client, std::string &param , Channel *chnl, bool setFlag)
{
    std::string limitMsg;
    
    if (setFlag)
    {
        size_t limit = std::strtoul(param.c_str(), NULL, 10);
        if (!chnl->isUserLimit())
            chnl->addMode('l');
        chnl->setUserLimit(limit);
        limitMsg = ":" + client->getMask() + " MODE " + chnl->getName() + " +l " + param + "\r\n"; 
    }
    else
    {
        if (chnl->isUserLimit())
            chnl->removeMode('l');
        chnl->setUserLimit(SIZE_MAX);
        limitMsg = ":" + client->getMask() + " MODE " + chnl->getName() + " -l " + "\r\n";
    }
    chnl->broadcast(limitMsg);
}

void    channelModeTopic(Client *client, Channel *chnl, bool setFlag)
{
    std::string topicMsg = ":" + client->getMask() + " MODE " + chnl->getName() + " +t\r\n";
    chnl->setTopicRestricted(true);
    chnl->addMode('t');
    if (!setFlag)
    {
        topicMsg = ":" + client->getMask() + " MODE " + chnl->getName() + " -t\r\n";
        chnl->setTopicRestricted(false);
        chnl->removeMode('t');
    }
    chnl->broadcast(topicMsg);
}

void    cmdMode(Client *client, struct Command cmd, TManager<int, Client *> &clients, TManager<std::string, Channel *> &channels)
{
    if (cmd.params.size() == 1)
    {
        try
        {
            Channel *chnl = channels.get(cmd.params[0]);
            rplChannelModeIs(client, chnl->getName(), chnl->getModes(), chnl->getModeParams());
        }
        catch(const std::exception& e)
        {
            return ;
        }
        
        return ;
    }
    Channel *chnl;
    try
    {
        chnl = channels.get(cmd.params[0]);
    }
    catch (const std::exception &e)
    {
        errNoSuchChannel(client, cmd.params[0]);
        return ;
    }
    if (!chnl->isMember(client))
    {
        errNotOnChannel(client, chnl->getName());
        return ;
    }
    if (!chnl->isOperator(client))
    {
        errChanOprivsNeeded(client, chnl->getName());
        return ;
    }
    bool setFlag = true;
    size_t argIndex = 2;
    std::string dummy = "";
    for (size_t i = 0; i < cmd.params[1].length(); i++)
    {
        char op = cmd.params[1][i];
        
        if (op == '+' || op == '-') 
        {
            setFlag = (op == '+');
        }
        else if (op == 'i' || op == 't') 
        {
            if (op == 'i')
                channelModeInvite(client, chnl, setFlag);
            if (op == 't')
                channelModeTopic(client, chnl, setFlag);
        }
        else if (op == 'o' || op == 'k' || (op == 'l' && setFlag)) 
        {
            if (argIndex < cmd.params.size())
            {
                if (op == 'o')
                    channelModeOp(client, cmd.params[argIndex], chnl, clients, setFlag);
                else if (op == 'k')
                    channelModeKey(client, cmd.params[argIndex], chnl, setFlag);
                else if (op == 'l')
                    channelModeLimit(client, cmd.params[argIndex], chnl, setFlag);
                argIndex++;
            }
            else
            {
                errNeedMoreParams(client, cmd.type);
                continue;
            }        
        }
        else if (op == 'l' && !setFlag)
            channelModeLimit(client, dummy,chnl, setFlag);
        else
            errUnknownMode(client, op);
    }
}

void    cmdPart(Client *client, struct Command cmd, TManager<std::string, Channel *> &channels)
{
    if (cmd.params.size() < 1)
    {
        errNeedMoreParams(client, cmd.type);
        if (Server::getInstance() != NULL)
            Server::getInstance()->enableWriteEvent(client->getFd());
		return ;
    }
    std::vector<std::string> targets = splitString(cmd.params[0], ',');   

    for (std::vector<std::string>::iterator it = targets.begin(); it < targets.end(); it++)
    {
        Channel *chnl;
        try 
        {
            chnl = channels.get(*it);
        } 
        catch (const std::exception &e) 
        {
            errNoSuchChannel(client, *it);
            continue;
        }
        if (!chnl->getMember(client->getNickname()))
        {
            errNotOnChannel(client, *it);
            continue;
        }
        std::string senderMask = client->getNickname() + "!~" + client->getUsername() + "@" + client->getIp();
        std::string partMsg = ":" + senderMask + " PART " + *it + "\r\n"; 
        chnl->broadcast(partMsg);
        chnl->removeMember(client);
		client->decrementChannelCount();
		if (chnl->getMemberList().empty())
		{
			channels.remove(*it);
			delete chnl;
		}
    }
}
