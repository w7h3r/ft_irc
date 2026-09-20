/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmdKick.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alermi <alermi@student.42kocaeli.com.tr>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 15:06:58 by alermi            #+#    #+#             */
/*   Updated: 2026/09/18 15:06:59 by alermi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Client/Client.hpp"
#include "../../inc/Channel/Channel.hpp"
#include "../../inc/Commands.hpp"
#include "../../inc/utils.hpp"
#include <climits>
#include <cstdlib>
#include <sys/socket.h>

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

	if (targetChannels.empty() || targets.empty())
	{
		errNeedMoreParams(client, cmd.type);
		return ;
	}
	if (targetChannels.size() > 1 && targets.size() > 1 && targetChannels.size() != targets.size())
	{
		errNeedMoreParams(client, cmd.type);
		return ;
	}

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
            continue ;
        }
        Channel *chnl;
        try
        {
            chnl = channels.get(channelName);
        }
        catch(const std::exception& e)
        {
            errNoSuchChannel(client, channelName);
            continue;
        }
        Client  *target = getClient(targetName, clients);
		if (!target)
		{
			errNoSuchNick(client, targetName);
			continue ; 
		}
        if (!chnl->isMember(client))
        {
            errNotOnChannel(client, channelName);
            continue ;
        }
        if (!chnl->isOperator(client))
        {
            errChanOprivsNeeded(client, channelName);
            continue ;
        }
        if (!chnl->isMember(target))
        {
            errUserNotInChannel(client, targetName, channelName);
            continue ;
        }
			if (chnl->isOperator(target))
			{
				chnl->removeOperator(target);
				if (chnl->getMemberList().size() > 1 && chnl->getOperators().empty())
				{
					Client *opTarget;
					if (*chnl->getMemberList().begin() == target)
						opTarget = *(chnl->getMemberList().begin() + 1);
					else
						opTarget = *chnl->getMemberList().begin();
					std::string opMsg = ":" + client->getMask() + " MODE " + chnl->getName() + " +o " + opTarget->getNickname() + "\r\n";
				chnl->broadcast(opMsg);
				chnl->addOperator(opTarget);
			
			}

		}
        std::string kickerMask = client->getNickname() + "!~" + client->getUsername() + "@" + client->getIp();
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
