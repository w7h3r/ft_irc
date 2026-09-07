/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oozsipah <oozsipah@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/24 21:16:11 by muokcan           #+#    #+#             */
/*   Updated: 2026/09/06 22:36:39 by oozsipah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Channel/Channel.hpp"
#include <sys/socket.h>

Channel::Channel(const std::string &name, const std::string &key)
{
    _name = name;
    _key = key;
    _isInviteOnly = 0;
    _isUserLimit = 0;
    _isTopicRestricted = 0;
    _userLimit = 0;
}

Channel::~Channel()
{
}

std::string Channel::getName() const { return (_name); }
std::string Channel::getTopic() const { return (_topic); }
std::string Channel::getKey() const { return (_key); }

bool        Channel::isInviteOnly() const { return (_isInviteOnly); }
bool        Channel::isTopicRestricted() const { return (_isTopicRestricted); }
bool        Channel::isUserLimit() const { return (_isUserLimit); }

size_t      Channel::getUserLimit() const { return ((_isUserLimit == 1) ? _userLimit : __SIZE_MAX__); }

const   std::vector<Client *> Channel::getMemberList() const { return (_members); }
const   std::vector<Client *> Channel::getOperators() const {return (_operators); }

void        Channel::setName(const std::string &name) { _name = name; }
void        Channel::setTopic(const std::string &topic) { _topic = topic; }
void        Channel::setKey(const std::string &key) { _key = key; }

void        Channel::setInviteOnly(bool& status) { _isInviteOnly = status; }
void        Channel::setTopicRestricted(bool& status) { _isTopicRestricted = status; }
void        Channel::setUserLimit(size_t& limit) { _userLimit = limit; }
// Member control stuff
void        Channel::addMember(Client *client) { _members.push_back(client); }

void        Channel::removeMember(Client *client)
{
    for (std::vector<Client *>::iterator it = _members.begin(); it != _members.end();)
        (*it == client) ? _members.erase(it) : it++;
}

bool        Channel::isMember(Client *client) const
{
    for (std::vector<Client *>::const_iterator it = _members.begin(); it != _members.end(); it++)
    {
        if (*it == client)
            return (1);
    }
    return (0);
}
// Operator control stuff
void        Channel::addOperator(Client *client) { _operators.push_back(client); }
void        Channel::removeOperator(Client *client)
{
    for (std::vector<Client *>::iterator it = _operators.begin(); it != _operators.end();)
        (*it == client) ? _operators.erase(it) : it++;
}
bool        Channel::isOperator(Client *client) const
{
    for (std::vector<Client *>::const_iterator it = _operators.begin(); it < _operators.end(); it++)
    {
        if (*it == client)
            return (1);
    }
    return (0);
}

// Invite control stuff (what the hell is invite ?)

void        Channel::addInvite(Client *client) { _inviteUsers.push_back(client); }
void        Channel::removeInvite(Client *client)
{
    for (std::vector<Client *>::iterator it = _inviteUsers.begin(); it < _inviteUsers.end();)
        (*it == client) ? _inviteUsers.erase(it) : it++;
}
bool        Channel::isInvite(Client *client) const
{
    for (std::vector<Client *>::const_iterator it = _inviteUsers.begin(); it < _inviteUsers.end(); it++)
    {
        if (*it == client)
            return (1);
    }
    return (0);
}

void        Channel::deleteClientFromAllChannels(Client *client, TManager<std::string, Channel *> &channels)
{
    std::map<std::string, Channel *> allChannels = channels.getAll();

    for (std::map<std::string, Channel *>::iterator it = allChannels.begin(); it != allChannels.end(); it++)
    {
        Channel *chnl = it->second;
        if (chnl->isMember(client))
            chnl->removeMember(client);
        if (chnl->isOperator(client))
            chnl->removeOperator(client);
    }
    delete client;
}

void        Channel::broadcast(const std::string &message, Client *exclude)
{
    for (std::vector<Client *>::iterator it = _members.begin(); it < _members.end(); it++)
    {
        if (*it == exclude)
            continue;
        (*it)->appendToWriteBuffer(message);
        send((*it)->getFd(), message.c_str(), message.size(), 0);
    }
};
