/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oozsipah <oozsipah@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/24 21:16:11 by muokcan           #+#    #+#             */
/*   Updated: 2026/09/20 19:34:01 by oozsipah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Channel/Channel.hpp"
#include "../inc/Server/Server.hpp"
#include <sys/socket.h>
#include <sstream>

Channel::Channel(const std::string &name, const std::string &key)
{
    _name = name;
    _key = key;
    _isInviteOnly = 0;
    _isUserLimit = 0;
    _isTopicRestricted = 1;
    _userLimit = 0;
}

Channel::~Channel()
{
}

template <typename N>
std::string intToString(N value)
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}

std::string Channel::getName() const { return (_name); }
std::string Channel::getTopic() const { return (_topic); }
std::string Channel::getKey() const { return (_key); }
std::string Channel::getModes() const { return (_modes); }
std::string Channel::getModeParams() const
{    
    std::string retVal = "";
    
    for (size_t i = 0; i < _modes.length(); i++)
    {
        char mode = _modes[i];
        
        if (mode == 'k' && !_key.empty())
        {
            retVal += _key;
            retVal += " ";
        }
        else if (mode == 'l' && _isUserLimit == true)
        {
            retVal += intToString(_userLimit);
            retVal += " ";
        }
    }
    if (!retVal.empty() && retVal[retVal.length() - 1] == ' ')
        retVal.erase(retVal.length() - 1, 1);
    return retVal;
}

void    Channel::addMode(char op)
{
    if (_modes.empty())
        _modes += '+';
    if (_modes.find(op) == std::string::npos)
        _modes += op;
}

void    Channel::removeMode(char op)
{
	if (_modes.empty())
        return ;
	size_t pos = _modes.find(op);
	if (pos == std::string::npos)
		return ;
	_modes.erase(pos, 1);
	if (_modes.length() == 1 && _modes[0] == '+')
		_modes.clear();
}

bool        Channel::isInviteOnly() const { return (_isInviteOnly); }
bool        Channel::isTopicRestricted() const { return (_isTopicRestricted); }
bool        Channel::isUserLimit() const { return (_isUserLimit); }

size_t      Channel::getUserLimit() const { return ((_isUserLimit == 1) ? _userLimit : __SIZE_MAX__); }

const   std::vector<Client *> Channel::getMemberList() const { return (_members); }
const   std::vector<Client *> Channel::getOperators() const {return (_operators); }

void        Channel::setName(const std::string &name) { _name = name; }
void        Channel::setTopic(const std::string &topic) { _topic = topic; }
void        Channel::setKey(const std::string &key) { _key = key; }

void        Channel::setInviteOnly(bool status) { _isInviteOnly = status; }
void        Channel::setTopicRestricted(bool status) { _isTopicRestricted = status; }
void        Channel::setUserLimit(size_t limit) { _userLimit = limit; (limit == 0) ? _isUserLimit = false : _isUserLimit = true; }
// Member control stuff
void        Channel::addMember(Client *client) { _members.push_back(client); }

Client      *Channel::getMember(const std::string &nick) const
{
    for (std::vector<Client *>::const_iterator it = _members.begin(); it < _members.end(); it++)
    {
        if ((*it)->getNickname() == nick)
            return *it;
    }
    return NULL;
}

void        Channel::removeMember(Client *client)
{
    for (std::vector<Client *>::iterator it = _members.begin(); it != _members.end();)
		{
		if (*it == client)
			_members.erase(it);
		else
			it++;
	}
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

void        Channel::addOperator(Client *client) { _operators.push_back(client); }
void        Channel::removeOperator(Client *client)
{
    for (std::vector<Client *>::iterator it = _operators.begin(); it != _operators.end();)
	{
		if (*it == client)
			_operators.erase(it);
		else
			it++;
	}
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

void        Channel::addInvite(Client *client) { _inviteUsers.push_back(client); }
void        Channel::removeInvite(Client *client)
{
    for (std::vector<Client *>::iterator it = _inviteUsers.begin(); it < _inviteUsers.end();)
		{
		if (*it == client)
			_inviteUsers.erase(it);
		else
			it++;
	}
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
    std::map<std::string, Channel *> &allChannels = channels.getAll();
	std::map<std::string, Channel *>::iterator it = allChannels.begin();

	while (it != allChannels.end())
    {
        Channel *chnl = it->second;
        if (chnl->isMember(client))
            chnl->removeMember(client);
        if (chnl->isOperator(client))
            chnl->removeOperator(client);
		if (chnl->getMemberList().empty())
		{
			delete chnl;
			allChannels.erase(it++);
		}
		else
			it++;
    }
}

void        Channel::broadcast(const std::string &message, Client *exclude)
{
    for (std::vector<Client *>::iterator it = _members.begin(); it < _members.end(); it++)
    {
        if (*it == exclude)
            continue;
        (*it)->appendToWriteBuffer(message);

		if (Server::getInstance() != NULL)
			Server::getInstance()->enableWriteEvent((*it)->getFd());
    }
};

