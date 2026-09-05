/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oozsipah <oozsipah@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/16 22:46:35 by oozsipah          #+#    #+#             */
/*   Updated: 2026/09/05 23:41:30 by oozsipah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Commands.hpp"


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

void        cmdJoin(Client *client, struct Command cmd, TManager<int, Client *> clients, TManager<std::string, Channel *> channels)
{
    Channel *chnl = channels.get(*(cmd.params.begin()));
    
    if (chnl == nullptr)
    {
        Channel *newChnl = createChannel(*(cmd.params.begin()), *(cmd.params.begin() + 1));
        channels.add(*(cmd.params.begin()), newChnl);
        newChnl->addMember(client);
        newChnl->addOperator(client);
        return ;
    }
    if (chnl->getKey() != *(cmd.params.begin() + 1))
        return ; // need proper errno ERR_BADCHANNELKEY
    if (chnl->isInviteOnly())
    {
        if (chnl->isInvite(client))
            chnl->addMember(client);
        return ; // need to return/expection proper errno ERR_INVITEONLYCHAN 
    }
    if (chnl->getUserLimit() == chnl->getMemberList().size() + 1)
        return ; // need to return proper errno
    if (*(cmd.params.begin() + 1) != chnl->getKey())
        return ; // need to return proper errno
    chnl->addMember(client);
}

void        cmdKick(Client *client, struct Command cmd, TManager<int, Client *> clients, TManager<std::string, Channel *> channels)
{
    Channel *chnl = getChannel(*(cmd.params.begin()));
    if (!chnl)
        return ; // need proper errno
    if (!chnl->getMember(client->getUsername())) // check if who wants to kick in channel
        return ; // need proper errno
    if (!chnl->getMember(client->getUsername())->isOP()) // check if the user is OP
        return ; // need proper errno
    if (chnl->getMember(*(cmd.params.begin() + 1))) // check if the member who about the kicked in channel
        return ; // need proper errno
    chnl->removeMember(chnl->getMember(*(cmd.params.begin() + 1))); // kick the member from channel who given in a params + 1 
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