/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdJoin.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alermi <alermi@student.42kocaeli.com.tr>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 15:07:10 by alermi            #+#    #+#             */
/*   Updated: 2026/09/18 15:07:12 by alermi           ###   ########.fr       */
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

			std::string	channelKeyStart = "";
            chnl = createChannel(channelName, channelKeyStart);
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
            if (chnl->isUserLimit() && (chnl->getUserLimit() <= chnl->getMemberList().size()))
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
