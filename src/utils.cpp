#include "../inc/Channel/Channel.hpp"
#include "../inc/Client/Client.hpp"
#include "../inc/templates/TManager.hpp"
#include <sstream>
#include <iostream>

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


Client      *getClient(std::string targetName, TManager<int, Client *> &clients)
{
    std::map<int, Client *> allClients = clients.getAll();
    Client *target = NULL;
    for (std::map<int, Client *>::iterator it = allClients.begin(); it != allClients.end(); it++)
    {
        target = it->second;
        if  (target->getNickname() == targetName) 
            return (target);
    }
    return (NULL);
}

Channel *createChannel(std::string &name, std::string &key)
{

    Channel *dummy =new Channel(name, key);
    return (dummy);
}

void    addChannel(Channel *chnl, Client *client, TManager<std::string, Channel *> &channels)
{

    chnl->addMember(client);
    channels.add(chnl->getName(), chnl);
}

void    removeChannel(Channel *chnl, TManager<std::string, Channel *> &channels)
{
    channels.remove(chnl->getName());
}


