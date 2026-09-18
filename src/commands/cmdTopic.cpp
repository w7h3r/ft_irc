/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdTopic.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alermi <alermi@student.42kocaeli.com.tr>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 15:06:26 by alermi            #+#    #+#             */
/*   Updated: 2026/09/18 15:06:27 by alermi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Client/Client.hpp"
#include "../../inc/Channel/Channel.hpp"
#include "../../inc/Server/Server.hpp"
#include "../../inc/Commands.hpp"
#include <climits>
#include <cstdlib>
#include <sys/socket.h>

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
