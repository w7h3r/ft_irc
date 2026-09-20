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
#include <iostream>
#include <climits>
#include <cstdlib>
#include <sys/socket.h>

void Server::printServerState()
{
    Server *inst = Server::getInstance();
    if (inst == NULL)
    {
        std::cout << "[ERROR] Server instance is not active!" << std::endl;
        return;
    }

    std::cout << "\n================= SERVER STATE =================" << std::endl;
    
    std::cout << "[+] CLIENTS LIST:" << std::endl;
    std::map<int, Client *> &allClients = inst->_clients.getAll();
    if (allClients.empty())
        std::cout << "    (No active clients)" << std::endl;
    else
    {
        for (std::map<int, Client *>::iterator it = allClients.begin(); it != allClients.end(); ++it)
        {
            Client *c = it->second;
            std::cout << "    -> FD: " << c->getFd() 
                      << " | Nick: " << c->getNickname() 
                      << " | User: " << c->getUsername() 
                      << " | IP: " << c->getIp() << std::endl;
        }
    }

    std::cout << "------------------------------------------------" << std::endl;

    std::cout << "[+] CHANNELS LIST:" << std::endl;
    std::map<std::string, Channel *> &allChannels = inst->_channel.getAll();
    if (allChannels.empty())
        std::cout << "    (No active channels)" << std::endl;
    else
    {
        for (std::map<std::string, Channel *>::iterator it = allChannels.begin(); it != allChannels.end(); ++it)
        {
            Channel *ch = it->second;
            std::cout << "    -> Channel: " << ch->getName() 
                      << " | Members Count: " << ch->getMemberList().size() << std::endl;
        }
    }
    
    std::cout << "================================================\n" << std::endl;
}

void cmdQuit(Client *client, struct Command cmd, TManager<std::string, Channel *> &channels)
{
	Server::getInstance()->printServerState();
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

    if (Server::getInstance() != NULL)
        Server::getInstance()->_deleteClientFromAllChannels(client); 
    std::string errMsg = ":" + (std::string)SERVER_NAME + " ERROR :Closing Link: " + client->getNickname() + " (" + reason + ")\r\n";
    client->appendToWriteBuffer(errMsg);

    if (Server::getInstance() != NULL)
        Server::getInstance()->enableWriteEvent(client->getFd());

    client->setConnState(DISCONNECT);
	Server::getInstance()->printServerState();
}
