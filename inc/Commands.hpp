#ifndef COMMAND_H
#define COMMAND_H

#include "Channel/Channel.hpp"
#include "Server/Server.hpp"
#include "templates/TManager.hpp"
#include <sstream>
#include <stdexcept>



Channel *getChannel(std::string &chnl_name);
void    addChannel(Channel *chnl);
void    removeChannel(Channel *chnl);


void	cmdJoin(Client *client, struct Command cmd, TManager<std::string, Channel *> &channels);
void	cmdKick(Client *client, struct Command cmd, TManager<int, Client *> &clients, TManager<std::string, Channel *> &channels);
void    cmdInvite(Client *client, struct Command cmd, TManager<int, Client *> &clients, TManager<std::string, Channel *> &channels);
void    cmdPrivMsg(Client *client, struct Command cmd, TManager<int, Client *> &clients, TManager<std::string, Channel *> &channels);
void    cmdMode(Client *client, struct Command cmd, TManager<int, Client *> &clients, TManager<std::string, Channel *> &channels);
void    cmdPart(Client *client, struct Command cmd, TManager<std::string, Channel *> &channels);
void    cmdList(TManager<int, Client *> clients);


void    cmdTopic(Client *client, struct Command cmd, TManager<std::string, Channel *> &channels);  
void	cmdQuit(Client *client, struct Command cmd, TManager<std::string, Channel *> &channels);

static inline void  sendNumericReply(Client *client, int code, const std::string &middle, const std::string &trailing)
{
    std::stringstream ss;
    ss << ":server " << code << " " << client->getNickname() << " " << middle + " :" + trailing + "\r\n";
    client->appendToWriteBuffer(ss.str());
    if (Server::getInstance() != NULL)
        Server::getInstance()->enableWriteEvent(client->getFd());
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

static inline void  errUnknownMode(Client *client, char middle)
{
    sendNumericReply(client, 472, std::string(1, middle), "is unknown mode char to me");
}

static inline void  errKeySet(Client *client, const std::string &middle)
{
    sendNumericReply(client, 467, middle, "Channel key already set");
}

static inline void  rplChannelModeIs(Client *client, const std::string &chnlName, const std::string &chnlModes, const std::string &chnlModeParams)
{
    sendNumericReply(client, 324, chnlName + " " + chnlModes + " " + chnlModeParams, "");
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

inline void	errNoNickGiven(Client *client)
{
	sendNumericReply(client, 431, "", "No nickname given");
}

inline void  errErroneusNickname(Client *client, const std::string &middle)
{
	sendNumericReply(client, 432, middle, "Erroneous nickname");
}

inline void  errNicknameInUse(Client *client, const std::string &middle)
{
	sendNumericReply(client, 433, middle, "Nickname is already in use");
}

inline void  errAlreadyRegistered(Client *client)
{
	sendNumericReply(client, 462, "", "You may not reregister");
}

inline void  errNotRegistered(Client *client)
{
	sendNumericReply(client, 451, "", "You have not registered");
}

inline void  errPasswdMismatch(Client *client)
{
	sendNumericReply(client, 464, "", "Password incorrect");
}

inline void  rplWelcome(Client *client)
{
	sendNumericReply(client, 001, "", "Welcome to the Internet Relay Network written by @oozsipah, @muokcan and @alermi " + client->getNickname() + "!" + client->getUsername() + "@" + client->getIp());
}

inline void  rplYourHost(Client *client)
{
	sendNumericReply(client, 002, "", "Your host is ft_irc, running version 0.1");
}

inline void  rplCreated(Client *client)
{
	sendNumericReply(client, 003, "", "This server was created on " + std::string(__DATE__) + " at " + std::string(__TIME__));
}

inline void  rplMyInfo(Client *client)
{
	sendNumericReply(client, 004, "", "ft_irc 0.1");
}

#endif
