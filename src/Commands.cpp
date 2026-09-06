/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oozsipah <oozsipah@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/16 22:46:35 by oozsipah          #+#    #+#             */
/*   Updated: 2026/09/06 22:37:25 by oozsipah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Commands.hpp"
#include <sstream>

Channel *createChannel(std::string &name, std::string &key)
{

    Channel *dummy =new Channel(name, key);
    return (dummy);
}

void    addChannel(Channel *chnl, Client *client, TManager<std::string, Channel *> channels)
{

    chnl->addMember(client);
    channels.add(chnl->getName(), chnl);
}

void    removeChannel(Channel *chnl, TManager<std::string, Channel *> channels)
{
    channels.remove(chnl->getKey());
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



void        cmdJoin(Client *client, struct Command cmd, TManager<int, Client *> clients, TManager<std::string, Channel *> channels)
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
    for (int i = 0; i < targetChannels.size(); i++)
    {
        std::string channelName = targetChannels[i];
        std::string channelKey = i < targetChannelKeys.size() ? targetChannelKeys[i] : "";

        if (channelName.empty() || ((channelName[0] != '#') && (channelName[0] != '&')))
        {
            errNoSuchChannel(client, channelName);
            continue;
        }
        Channel *chnl = channels.get(channelName);
        if (!chnl)
        {
            chnl = createChannel(channelName, channelKey);
            chnl->addMember(client);
            chnl->addOperator(client);
            channels.add(channelName, chnl);
            continue;
        }
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
        if (chnl->getUserLimit() == chnl->getMemberList().size())
        {
            errChannelIsFull(client, channelName);
            continue;
        }
        if (client->getChannelCount() == MAX_CHANNEL_COUNT)
        {
            errTooManyChannels(client, channelName);
            continue;
        }
        chnl->addMember(client);
        client->incrementChannelCount();
        std::string chnlTopic = chnl->getTopic();
        if (chnlTopic.empty())
            rplNoTopic(client, channelName);
        else
            rplTopic(client, channelName, chnlTopic);
    }
}

void        cmdKick(Client *client, struct Command cmd, TManager<int, Client *> clients, TManager<std::string, Channel *> channels)
{
    if (cmd.params.empty())
    {
        errNeedMoreParams(client, cmd.type);
        return ;
    }
    std::vector<std::string> targetChannels = splitString(*(cmd.params.begin()), ',');
    std::vector<std::string> targets;
    if (cmd.params.size() > 1)
    {
        targets = splitString(*(cmd.params.begin() + 1), ',');   
    }
    for (int i = 0; i < targets.size(); i++)
    {
        
        std::string channelName = targetChannels[i];
        std::string targetName = targets[i];
        if (channelName.empty() || (channelName[0] != '#' && channelName[0] != '&'))
        {
            errNoSuchChannel(client, channelName);
            return ;
        }
        Channel *chnl = channels.get(channelName);
        std::map<int, Client *>allClients = clients.getAll();
        Client  *target;
        for (std::map<int, Client *>::iterator it = allClients.begin(); it != allClients.end(); it++)
        {
            target = it->second;
            if (target->getUsername() == targetName)
                break ;
        }
        if (!chnl->isMember(client))
        {
            errNotOnChannel(client, channelName);
            return ;
        }
        if (!chnl->isOperator(client))
        {
            errChanOprivsNeeded(client, channelName);
            return ;
        }
        if (!chnl->isMember(target))
        {
            errUserNotInChannel(client, targetName, channelName);
            return ;
        }
        std::string kickerMask = client->getNickname() + "!~" + client->getUsername() + "@127.0.0.1";
        std::string comment;
        if (cmd.params.size() > 2 && !cmd.params[2].empty())
            comment = *(cmd.params.begin() + 2);
        kickMsg(chnl, kickerMask, targetName, comment);
        client->decrementChannelCount();
        chnl->removeMember(target);
    }
}


// komutu kullanan channel'da değilse
// OP değilse
// böyle bir nick yoksa (user yoksa)
// user channel'daysa
// başarı olduysa RPL_INVITE

void        cmdInvite(Client *client, struct Command cmd, TManager<int, Client *> clients, TManager<std::string, Channel *> channels)
{
    if (!getChannel(*(cmd.params.begin() + 1))->isMember(client)) // check if OP on the channel
        return ; // need to return ERR_NOTONCHANNEL
    if (!getChannel(*(cmd.params.begin() + 1))->isOperator(client)) // check if OP
        return ; // need to return ERR_CHANOPRIVSNEEDED
    // check if param username is exits for ERR_NOSUCHNICK
    if (getChannel(*(cmd.params.begin() + 1))->isMember())
}