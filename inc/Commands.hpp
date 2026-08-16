#ifndef COMMAND_H
#define COMMAND_H

#include "Channel/Channel.hpp"
#include <vector>

class Commands
{
    private:
        std::vector<Channel *> _channels;
        // OCF
    public:
        Commands();
        ~Commands();
        // std::vector<Channel *> getChannels() const;
        
        Channel *getChannel(std::string &chnl_name) const;
        void    addChannel(Channel *chnl);
        void    removeChannel(Channel *chnl);
		
        
		void	cmdJoin(Client *client, struct Command cmd);
        void	cmdKick(Client *client, struct Command cmd);
        void    cmdInvite(Client *clien, struct Command cmd);        
};

#endif