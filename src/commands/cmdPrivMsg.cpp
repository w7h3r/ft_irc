/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdPrivMsg.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alermi <alermi@student.42kocaeli.com.tr>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 15:06:34 by alermi            #+#    #+#             */
/*   Updated: 2026/09/18 15:06:49 by alermi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../inc/Client/Client.hpp"
#include "../../inc/Channel/Channel.hpp"
#include "../../inc/Server/Server.hpp"
#include "../../inc/Commands.hpp"
#include "../../inc/utils.hpp"
#include <climits>
#include <cstdlib>
#include <sys/socket.h>

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
        std::string msg = ":" + client->getMask() + " " + cmd.type + " " + ((chnl == NULL) ? target->getNickname() : chnl->getName()) + " :" + cmd.message + "\r\n";
        
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


