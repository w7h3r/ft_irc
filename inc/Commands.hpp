#ifndef COMMAND_H
#define COMMAND_H

#include "Channel/Channel.hpp"
#include "templates/TManager.hpp"

Channel *getChannel(std::string &chnl_name);
void    addChannel(Channel *chnl);
void    removeChannel(Channel *chnl);


void	cmdJoin(Client *client, struct Command cmd, TManager<int, Client *> clients, TManager<std::string, Channel *> channels);
void	cmdKick(Client *client, struct Command cmd, TManager<int, Client *> clients, TManager<std::string, Channel *> channels);
void    cmdInvite(Client *clien, struct Command cmd, TManager<int, Client *> clients, TManager<std::string, Channel *> channels);  
void    cmdTopic(Client *clien, struct Command cmd, TManager<int, Client *> clients, TManager<std::string, Channel *> channels);  
void    cmdMode(Client *clien, struct Command cmd, TManager<int, Client *> clients, TManager<std::string, Channel *> channels);  

#endif