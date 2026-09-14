#ifndef COMMAND_H
#define COMMAND_H

#include "Channel/Channel.hpp"
#include "templates/TManager.hpp"
#include <sstream>



Channel *getChannel(std::string &chnl_name);
void    addChannel(Channel *chnl);
void    removeChannel(Channel *chnl);


void	cmdJoin(Client *client, struct Command cmd, TManager<std::string, Channel *> &channels);
void	cmdKick(Client *client, struct Command cmd, TManager<int, Client *> &clients, TManager<std::string, Channel *> &channels);
void    cmdInvite(Client *client, struct Command cmd, TManager<int, Client *> &clients, TManager<std::string, Channel *> &channels);
void    cmdPrivMsg(Client *client, struct Command cmd, TManager<int, Client *> &clients, TManager<std::string, Channel *> &channels);
void        cmdList(TManager<int, Client *> clients);


void    cmdTopic(Client *client, struct Command cmd, TManager<int, Client *> &clients, TManager<std::string, Channel *> &channels);  
// void    cmdMode(Client *client, struct Command cmd, TManager<int, Client *> &clients, TManager<std::string, Channel *> &channels);  


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

static inline void  errNoSuchNick(Client *client, const std::string &middle)
{
    sendNumericReply(client, 401, middle, "No such nick/channel");
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

static inline void  errUserOnChannel(Client *client, const std::string &middle1, const std::string &middle2)
{
    sendNumericReply(client, 443, middle1 + " " + middle2, "is already on channel");
}

static inline void  errChanOprivsNeeded(Client *client, const std::string &middle)
{
    sendNumericReply(client, 482, middle, "You're not channel operator");
}

static inline void  errTooManyChannels(Client *client, const std::string &middle)
{
    sendNumericReply(client, 405, middle, "You have joined too many channels");
}

static inline void  errNoRecipient(Client *client, const std::string &middle)
{
    sendNumericReply(client, 411, "", "No recipient given (" + middle + ')');
}

static inline void  errCannotSendToChan(Client *client, const std::string &middle)
{
    sendNumericReply(client, 404, middle, "Cannot send to channel");
}

static inline void  errTooManyTargets(Client *client, const std::string &middle)
{
    sendNumericReply(client, 407, middle, "Duplicate recipients. No message \\");
}

static inline void  errNoTextToSend(Client *client)
{
    sendNumericReply(client, 412, "", "");
}

static inline void  rplInviting(Client *client, std::string &middle1, std::string &middle2)
{
    sendNumericReply(client, 341, middle1 + " " + middle2, "");
}

static inline void  kickMsg(Channel *chnl ,const std::string &kickerMask, const std::string &targetNick, const std::string &comment)
{
    std::string kickMsgStr = ":" + kickerMask + " KICK " + chnl->getName() + " " + targetNick + " :" + comment + "\r\n";
    chnl->broadcast(kickMsgStr);
}

#endif
