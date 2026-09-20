/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oozsipah <oozsipah@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/16 22:46:35 by oozsipah          #+#    #+#             */
/*   Updated: 2026/09/18 15:52:39 by alermi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Commands.hpp"
#include "../inc/Server/Server.hpp"
#include "../inc/utils.hpp"
#include <iostream>
#include <climits>
#include <cstdlib>
#include <sys/socket.h>

#define NO_R (void)

void    cmdPart(Client *client, struct Command cmd, TManager<std::string, Channel *> &channels)
{
    if (cmd.params.size() < 1)
    {
        errNeedMoreParams(client, cmd.type);
        if (Server::getInstance() != NULL)
            Server::getInstance()->enableWriteEvent(client->getFd());
		return ;
    }
    std::vector<std::string> targets = splitString(cmd.params[0], ',');   

    for (std::vector<std::string>::iterator it = targets.begin(); it < targets.end(); it++)
    {
        Channel *chnl;
        try 
        {
            chnl = channels.get(*it);
        } 
        catch (const std::exception &e) 
        {
            errNoSuchChannel(client, *it);
            continue;
        }
        if (!chnl->getMember(client->getNickname()))
        {
            errNotOnChannel(client, *it);
            continue;
        }
        std::string senderMask = client->getNickname() + "!~" + client->getUsername() + "@" + client->getIp();
        std::string partMsg = ":" + senderMask + " PART " + *it + "\r\n"; 
        chnl->broadcast(partMsg);
        chnl->removeMember(client);
		client->decrementChannelCount();
		if (chnl->getMemberList().empty())
		{
			channels.remove(*it);
			delete chnl;
		}
    }
}

void cmdPass(Client *client, struct Command cmd, const std::string serverPassword)
{
	if (cmd.params.empty() || cmd.params[0].empty())
		return (NO_R(errNeedMoreParams(client, cmd.type)));

	if (client->getConnState() != WAITING_PASS)
		return (NO_R(errAlreadyRegistered(client)));

	if (cmd.params[0] != serverPassword)
	{
		std::cout << "Password mismatch for client " << client->getFd() << std::endl;
		client->setConnState(REFUSED);
		return (NO_R(errPasswdMismatch(client)));
	}

	client->setConnState(WAITING_NICK);
}

std::vector<Client *> Server::getAllClients()
{
	std::vector<Client *> allClients;
	std::map<int, Client *> clientsMap = _clients.getAll();
	for (std::map<int, Client *>::const_iterator it = clientsMap.begin(); it != clientsMap.end(); ++it)
		allClients.push_back(it->second);
	return allClients;
}

static Client *getClientByNickname(const std::string& nickname, const std::vector<Client *>& clients)
{
	for (std::vector<Client *>::const_iterator it = clients.begin(); it != clients.end(); ++it)
	{
		Client *client = *it;
		if (client && client->getNickname() == nickname)
			return client;
	}
	return NULL;
}

static bool	isValidName(const std::string& str)
{
	
	if (str.empty())
		return (false);
	for (size_t i = 0; i < str.length(); ++i)
	{
		if (!isalnum(str[i]) && str[i] != '-' && str[i] != '_')
			return (false);
	}
	return (true);
}

void cmdNick(Client *client, struct Command cmd)
{
	if (client->getConnState() == WAITING_PASS)
		return (NO_R(errNotRegistered(client)));
	if (cmd.params.empty() || cmd.params[0].empty())
		return (NO_R(errNoNickGiven(client)));
	if (!isValidName(cmd.params[0]))
		return (NO_R(errErroneusNickname(client, cmd.params[0])));

	std::string nickname = cmd.params[0];
	Client *existingClient = getClientByNickname(nickname, Server::getInstance()->getAllClients());
	if (existingClient && existingClient != client)
		return (NO_R(errNicknameInUse(client, nickname)));
	client->setNickname(nickname);
	if (client->getUsername().empty())
		client->setConnState(WAITING_INFO);
	else if (!client->isRegistered())
	{
		client->setConnState(ACCEPT);
		rplWelcome(client);
		rplYourHost(client);
		rplCreated(client);
		rplMyInfo(client);
	}
}

void cmdUser(Client *client, struct Command cmd)
{
	if (client->getConnState() == WAITING_PASS)
		return (NO_R(errNotRegistered(client)));

	if (cmd.params.empty() || cmd.params[0].empty())
		return (NO_R(std::cout << "Error: USER command missing username parameter" << std::endl));

	client->setUsername(cmd.params[0]);
	if (!client->getNickname().empty() && !client->isRegistered())
	{
		client->setConnState(ACCEPT);
		rplWelcome(client);
		rplYourHost(client);
		rplCreated(client);
		rplMyInfo(client);
	}
	else if (client->getNickname().empty())
		client->setConnState(WAITING_INFO);
}

void cmdCap(Client *client, struct Command cmd)
{
	if (!cmd.params.empty() && cmd.params[0] == "LS")
    {
        std::string capResponse = ":ft_irc CAP * LS :\r\n";
        client->appendToWriteBuffer(capResponse);
        std::cout << "Client " << client->getFd() << " requested CAP LS. Responded with: " << capResponse << std::endl;
    }
	else if (!cmd.params.empty() && cmd.params[0] == "END")
	{
	}
}

void	botPardus(Client *client)
{
	std::string response = "'pardus meows in turkish :3'\r\n";

	client->appendToWriteBuffer(response);
	if (Server::getInstance() != NULL)
		Server::getInstance()->enableWriteEvent(client->getFd());
}

void	decideCommand(Client *client, struct Command cmd, TManager<int, Client *> &clients, TManager<std::string, Channel *> &channels, const std::string& serverPassword)
{
	if (cmd.type == "PASS")
		return cmdPass(client, cmd, serverPassword);
	if (client->isRefused() || client->isDisconnected())
		return ;
	else if (cmd.type == "NICK")
		return cmdNick(client, cmd);
	else if (cmd.type == "USER")
		return cmdUser(client, cmd);
	else if (cmd.type == "CAP")
		return cmdCap(client, cmd);
	if (client->getConnState() != ACCEPT)
		return errNotRegistered(client);
	else if (cmd.type == "JOIN")
		cmdJoin(client, cmd, channels);
	else if (cmd.type == "PARDUS")
		botPardus(client);
	else if (cmd.type == "KICK")
		cmdKick(client, cmd, clients, channels);
	else if (cmd.type == "INVITE")
		cmdInvite(client, cmd, clients, channels);
	else if (cmd.type == "TOPIC")
		cmdTopic(client, cmd, channels);
	else if (cmd.type == "MODE")
		cmdMode(client, cmd, clients, channels);
	else if (cmd.type == "PRIVMSG")
		cmdPrivMsg(client, cmd, clients, channels);
	else if (cmd.type == "QUIT")
		cmdQuit(client, cmd, channels);
	else if (cmd.type == "PART")
		cmdPart(client, cmd, channels);
	else if (cmd.type == "WHO")
		return ;
	else
	{
		errUnknownCommand(client, cmd.type);
		std::cout << "Unknown Command: " << cmd.type << std::endl;
	}
}

