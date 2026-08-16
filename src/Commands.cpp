/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oozsipah <oozsipah@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/16 22:46:35 by oozsipah          #+#    #+#             */
/*   Updated: 2026/08/16 23:50:17 by oozsipah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Commands.hpp"


Commands::Commands()
{
}

Commands::~Commands()
{
}

Channel *createChannel(std::string &name, std::string key)
{
    Channel *dummy =new Channel(name, key);
    return (dummy);
}

Channel *Commands::getChannel(std::string &chnl_name) const
{
    for (std::vector<Channel *>::const_iterator it = _channels.begin(); it < _channels.end(); it++)
    {
        if ((*it)->getName() == chnl_name)
            return (*it);
    }
    return (nullptr);
}

void    Commands::addChannel(Channel *chnl) { _channels.push_back(chnl); }

void    Commands::removeChannel(Channel *chnl)
{
    for (std::vector<Channel *>::iterator it = _channels.begin(); *it != chnl;)
        (*it == chnl) ? _channels.erase(it) : it++;
}

void        Commands::cmdJoin(Client *client, struct Command cmd)
{
    Channel *chnl = getChannel(*(cmd.params.begin()));
    
    if (chnl == nullptr)
    {
        addChannel(createChannel(*(cmd.params.begin()), *(cmd.params.begin() + 1)));
        (*_channels.end())->addMember(client);
        return ;
    }
    if (chnl->getUserLimit() == chnl->getMemberList().size() + 1)
        return ; // need to return proper errno
    if (*(cmd.params.begin() + 1) != chnl->getKey())
        return ; // need to return proper errno
    chnl->addMember(client);
}

void        Commands::cmdKick(Client *client, struct Command cmd)
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

void        Commands::cmdInvite(Client *client, struct Command cmd)
{
    if (!getChannel(*(cmd.params.begin() + 1))->isMember(client)) // check if OP on the channel
        return ; // need to return ERR_NOTONCHANNEL
    if (!getChannel(*(cmd.params.begin() + 1))->isOperator(client)) // check if OP
        return ; // need to return ERR_CHANOPRIVSNEEDED
    // check if param username is exits for ERR_NOSUCHNICK
    if (getChannel(*(cmd.params.begin() + 1))->isMember())
}