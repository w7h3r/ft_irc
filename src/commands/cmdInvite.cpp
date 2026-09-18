
#include "../../inc/Client/Client.hpp"
#include "../../inc/Channel/Channel.hpp"
#include "../../inc/Server/Server.hpp"
#include "../../inc/Commands.hpp"
#include "../../inc/utils.hpp"
#include <climits>
#include <cstdlib>
#include <sys/socket.h>


void    cmdInvite(Client *client, struct Command cmd, TManager<int, Client *> &clients, TManager<std::string, Channel *> &channels)
{
    if (cmd.params.size() != 2)
    {
        errNeedMoreParams(client, cmd.type);
        return ;
    }

    if (!channels.exists(cmd.params[1]))
    {
        errNoSuchChannel(client, cmd.params[1]);
        return ;
    }
    
    Channel *chnl = channels.get(cmd.params[1]);
    Client  *target = getClient(cmd.params[0], clients);

    if (!target)
    {
        errNoSuchNick(client, cmd.params[0]);
        return ;
    }
    if (!chnl->isMember(client))
    {
        errNotOnChannel(client, chnl->getName());
        return ;
    }
    if (!chnl->isOperator(client))
    {
        errChanOprivsNeeded(client, chnl->getName());
        return ;
    }
    if (chnl->isMember(target))
    {
        errUserOnChannel(client, target->getNickname(), chnl->getName());
        return ;
    }

    chnl->addInvite(target);

    std::string targetNick = target->getNickname();
    std::string chnlName = chnl->getName();
    rplInviting(client, targetNick, chnlName);

    std::string inviteMsg = ":" + client->getMask() + " INVITE " + target->getNickname()  + " :" + chnl->getName() + "\r\n";
    target->appendToWriteBuffer(inviteMsg);

	if (Server::getInstance() != NULL) {
	    Server::getInstance()->enableWriteEvent(target->getFd());
	}
}
