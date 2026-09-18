/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdMode.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alermi <alermi@student.42kocaeli.com.tr>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 15:52:35 by alermi            #+#    #+#             */
/*   Updated: 2026/09/18 15:52:36 by alermi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Client/Client.hpp"
#include "../../inc/Channel/Channel.hpp"
#include "../../inc/Commands.hpp"
#include "../../inc/utils.hpp"
#include <climits>
#include <cstdlib>
#include <sys/socket.h>

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
    std::string opMsg = ":" + client->getMask() + " MODE " + chnl->getName() + " " + ((setFlag == true) ? "+o" : "-o") + target->getNickname() + "\r\n";
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

