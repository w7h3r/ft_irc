
#ifndef UTILS_HPP
#define UTILS_HPP

#include "./Client/Client.hpp"
#include "./templates/TManager.hpp"
#include "./Channel/Channel.hpp"
#include <vector>

std::vector<std::string> splitString(const std::string &str, char delimiter);
Client      *getClient(std::string targetName, TManager<int, Client *> &clients);
Channel *createChannel(std::string &name, std::string &key);
void		transferOp(Client *client, Channel *chnl, Client *target);
void    addChannel(Channel *chnl, Client *client, TManager<std::string, Channel *> &channels);
void    removeChannel(Channel *chnl, TManager<std::string, Channel *> &channels);

#endif
