#ifndef COMMAND_H
#define COMMAND_H

#include "Channel/Channel.hpp"
#include "templates/TManager.hpp"
#include <sstream>



Channel *getChannel(std::string &chnl_name);
void    addChannel(Channel *chnl);
void    removeChannel(Channel *chnl);


void	cmdJoin(Client *client, struct Command cmd, TManager<int, Client *> clients, TManager<std::string, Channel *> channels);
void	cmdKick(Client *client, struct Command cmd, TManager<int, Client *> clients, TManager<std::string, Channel *> channels);
void    cmdInvite(Client *clien, struct Command cmd, TManager<int, Client *> clients, TManager<std::string, Channel *> channels);  
void    cmdTopic(Client *clien, struct Command cmd, TManager<int, Client *> clients, TManager<std::string, Channel *> channels);  
void    cmdMode(Client *clien, struct Command cmd, TManager<int, Client *> clients, TManager<std::string, Channel *> channels);  


static inline void  sendNumericReply(Client *client, int code, const std::string &middle, const std::string &trailing)
{
    std::stringstream ss;
    ss << ":server " << code << " " << client->getNickname() << " " << middle + " :" + trailing + "\r\n";
    client->appendToWriteBuffer(ss.str());
}

static inline void  errNeedMoreParams(Client *client, const std::string &middle)
{
    sendNumericReply(client, 461, middle, "Not enough parameters");
}

static inline void  errNoSuchChannel(Client *client, const std::string &middle)
{
    sendNumericReply(client, 403, middle, "No such channel");
}

static inline void  errBadChannelKey(Client *client, const std::string &middle)
{
    sendNumericReply(client, 475, middle, "Cannot join channel (+k)");
}

static inline void  errInviteOnlyChan(Client *client, const std::string &middle)
{
    sendNumericReply(client, 473, middle, "Cannot join channel (+i)");
}

static inline void  errChannelIsFull(Client *client, const std::string &middle)
{
    sendNumericReply(client, 471, middle, "Cannot join channel (+l)");
}

static inline void  rplNoTopic(Client *client, const std::string &middle)
{
    sendNumericReply(client, 331, middle, "No topic is set");
}

static inline void  rplTopic(Client *client, const std::string &middle, const std::string trailing)
{
    sendNumericReply(client, 332, middle, trailing);
}

static inline void  errNotOnChannel(Client *client, const std::string &middle)
{
    sendNumericReply(client, 442, middle, "You're not on that channel");
}

static inline void  errUserNotInChannel(Client *client, const std::string &middle1, const std::string &middle2)
{
    sendNumericReply(client, 441, middle1 + " " + middle2, "They aren't on that channel");
}

static inline void  errChanOprivsNeeded(Client *client, const std::string &middle)
{
    sendNumericReply(client, 482, middle, "You're not channel operator");
}

#endif
