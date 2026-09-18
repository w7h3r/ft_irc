/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdQuit.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alermi <alermi@student.42kocaeli.com.tr>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 15:06:31 by alermi            #+#    #+#             */
/*   Updated: 2026/09/18 15:06:31 by alermi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Client/Client.hpp"
#include "../../inc/Channel/Channel.hpp"
#include "../../inc/Server/Server.hpp"
#include "../../inc/Commands.hpp"
#include <climits>
#include <cstdlib>
#include <sys/socket.h>

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
