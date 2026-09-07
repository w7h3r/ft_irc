/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alermi <alermi@student.42kocaeli.com       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/24 21:16:05 by alermi            #+#    #+#             */
/*   Updated: 2026/08/17 00:22:33 by oozsipah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server/Server.hpp"
#include <iostream>
#include <stdexcept>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cerrno>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdexcept>
#include <sys/types.h>
#include <string>
#include <unistd.h>
#include <csignal>

Server	*Server::_instance = NULL;

Server::Server(int port, const std::string& password) : 
    _port(port),
    _socket(-1),
    _password(password),
    _running(false),
    _epollFd(-1)
{
	Server::_instance = this;
    std::cout << "Server Constructor Called" << std::endl;
}

Server::~Server()
{
    std::cout << "Destrcutor Called" << std::endl;
    if (_socket != -1){
        close(_socket);
	}
	if (_epollFd != -1){
		close(_epollFd);
	}
	this->_clients.clearAll();
	Server::_instance = NULL;
}

void	Server::_initSocket()
{
	int	opt = 1;
	_socket = socket(AF_INET, SOCK_STREAM , 0);
	if (_socket == -1)
		throw std::runtime_error("Error: Socket do not created:");
	else
		std::cout << "Socket Success open" << std::endl;
	std::cout << _socket << std::endl;

	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw (std::runtime_error("Error socket option:SO_REUSEADDR not set"));

	if (setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)) < 0)
		throw(std::runtime_error("Error socket option: TCP_NODELAY not set"));
	
	struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(_port);

if (bind(_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
        throw std::runtime_error("Error: Bind do not success. Ports already in use");

    if (listen(_socket, SOMAXCONN) < 0)
        throw std::runtime_error("Error: listen not working ");

    int flags = fcntl(_socket, F_GETFL, 0);
    if (flags == -1 || fcntl(_socket, F_SETFL, flags | O_NONBLOCK) == -1)
        throw std::runtime_error("Hata: Non-blocking not set");

}

void	Server::_initEpoll()
{
	_epollFd = epoll_create1(0);

	if (_epollFd == -1)
		throw (std::runtime_error("Error: epoll_Create failed"));

	struct	epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = _socket;

	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, _socket, &ev) == -1)
		throw (std::runtime_error("Error: epoll table add error"));

	std::cout << "Epoll initilize successfuly" << std::endl;
}

void	Server::_modifyEpoll(int fd, int events)
{
	struct	epoll_event ev;

	ev.events = events;
	ev.data.fd = fd;

	if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, fd, &ev))
	{
		std::cerr << "Epoll Change Mode Error" << std::endl;
	}
}

void	Server::_writerClient(int fd)
{
	Client	*newClient = _clients.get(fd);
	if (!newClient)
		return ;

	std::string	&output = newClient->getWriteBuffer();

	if (output.empty())
	{
		_modifyEpoll(fd, EPOLLIN);
		return ;
	}

	ssize_t	byteCount = send(fd, output.c_str(), output.size(), 0);
	
	if (byteCount > 0)
	{
		output.erase(0, byteCount);
		if (output.empty())
			_modifyEpoll(fd, EPOLLIN);
	}
	else if (byteCount <= 0)
		_refuseClient(fd);
}

Channel *getChannel(std::string &chnl_name)
{
	(void)chnl_name;
	return (NULL);
}
void    addChannel(Channel *chnl)
{
	(void)chnl;
}
void    removeChannel(Channel *chnl)
{
	(void)chnl;
}


void	cmdJoin(Client *client, struct Command cmd, TManager<int, Client *> &clients, TManager<std::string, Channel *> &channels)
{
	(void)client;
	(void)cmd;
	(void)clients;
	(void)channels;
	std::cout << "Processing JOIN Command" << std::endl;
}
void	cmdKick(Client *client, struct Command cmd, TManager<int, Client *> &clients, TManager<std::string, Channel *> &channels)
{
	(void)client;
	(void)cmd;
	(void)clients;
	(void)channels;
	std::cout << "Processing KICK Command" << std::endl;
}
void    cmdInvite(Client *client, struct Command cmd, TManager<int, Client *> &clients, TManager<std::string, Channel *> &channels)
{
	(void)client;
	(void)cmd;
	(void)clients;
	(void)channels;
	std::cout << "Processing INVITE Command" << std::endl;
}
void    cmdTopic(Client *client, struct Command cmd, TManager<int, Client *> &clients, TManager<std::string, Channel *> &channels)
{
	(void)client;
	(void)cmd;
	(void)clients;
	(void)channels;
	std::cout << "Processing TOPIC Command" << std::endl;
}
void    cmdMode(Client *client, struct Command cmd, TManager<int, Client *> &clients, TManager<std::string, Channel *> &channels)
{
	(void)client;
	(void)cmd;
	(void)clients;
	(void)channels;
	std::cout << "Processing MODE Command" << std::endl;
}

void cmdPass(Client *client, struct Command cmd, const std::string serverPassword)
{
	if (cmd.params.empty())
		return ((void)(std::cout << "Error: PASS command missing password parameter" << std::endl));

	if (cmd.params[0] == serverPassword)
	{
		client->setPassword(cmd.params[0]);
		client->setConnState(WAITING_NICK);
		std::cout << "Client " << client->getFd() << " provided correct password." << std::endl;
	}
	else
	{
		std::cout << "Client " << client->getFd() << " provided incorrect password." << std::endl;
		client->setConnState(REFUSED);
	}
}
void cmdNick(Client *client, struct Command cmd)
{
	if (cmd.params.empty())
		return ((void)(std::cout << "Error: NICK command missing nickname parameter" << std::endl));

	client->setNickname(cmd.params[0]);
	if (client->getConnState() == WAITING_NICK)
		client->setConnState(WAITING_INFO);
	std::cout << "Client " << client->getFd() << " set nickname to: " << cmd.params[0] << std::endl;

}
void cmdUser(Client *client, struct Command cmd)
{
	if (cmd.params.empty())
		return ((void)(std::cout << "Error: USER command missing username parameter" << std::endl));

	client->setUsername(cmd.params[0]);
	if (client->getConnState() == WAITING_INFO)
	{
		client->setConnState(ACCEPT);
		std::cout << "BAGLANDI SUKSES" << std::endl;
		std::cout << "Client " << client->getFd() << " set username to: " << cmd.params[0] << std::endl;
		std::string welcomeMessage = ":ft_irc 001 " + client->getNickname() + " :Welcome to the IRC server " + client->getNickname() + "\r\n";
		client->appendToWriteBuffer(welcomeMessage);	}
}

void cmdCap(Client *client, struct Command cmd)
{
    if (!cmd.params.empty() && cmd.params[0] == "LS")
    {
        std::string capResponse = ":ft_irc CAP * LS :\r\n";
        client->appendToWriteBuffer(capResponse);
        std::cout << "Client " << client->getFd() << " requested CAP LS. Responded with: " << capResponse << std::endl;
    }
    else
        return ((void)(std::cout << "Error: CAP command missing parameters" << std::endl));
}

static void	decideCommand(Client *client, struct Command cmd, TManager<int, Client *> &clients, TManager<std::string, Channel *> &channels, const std::string& serverPassword)
{
	if (cmd.type == "PASS")
		cmdPass(client, cmd, serverPassword);
	else if (cmd.type == "NICK")
		cmdNick(client, cmd);
	else if (cmd.type == "USER")
		cmdUser(client, cmd);
	else if (cmd.type == "CAP")
		cmdCap(client, cmd);
	else if (cmd.type == "JOIN")
		cmdJoin(client, cmd, clients, channels);
	else if (cmd.type == "KICK")
		cmdKick(client, cmd, clients, channels);
	else if (cmd.type == "INVITE")
		cmdInvite(client, cmd, clients, channels);
	else if (cmd.type == "TOPIC")
		cmdTopic(client, cmd, clients, channels);
	else if (cmd.type == "MODE")
		cmdMode(client, cmd, clients, channels);
	else
		std::cout << "Unknown Command: " << cmd.type << std::endl;
}

void	Server::_readerClient(int fd)
{
	char	buffer[1024];

	ssize_t	contentByte = recv(fd, buffer, sizeof(buffer) - 1, 0);
	if (contentByte <= 0)
	{
		_refuseClient(fd);
	}
	else
	{
		buffer[contentByte] = '\0';
		std::cout << "> " << fd << " (RECV): " << buffer << std::endl;

		Client	*newClient = _clients.get(fd);
		if (newClient)
		{
			newClient->appendToReadBuffer(buffer);

			while (newClient->hasCompleteCommand())
			{
				std::string	rawCommands = newClient->extractCommand();
				Command cmd = newClient->parseMessage(rawCommands);
				
				std::cout << "Processing Command: " << rawCommands << std::endl; // DEBUG
				
				decideCommand(newClient, cmd, _clients, _channel, _password);

				for (std::vector<std::string>::iterator it = cmd.params.begin(); it != cmd.params.end(); ++it)
				{
					std::cout << "debug cmds: " << *it << std::endl;
				}
			}

			if (!newClient->getWriteBuffer().empty())
			{
				_modifyEpoll(fd, EPOLLIN | EPOLLOUT);
			}
		}
	}
}

void	Server::_acceptClient()
{
	struct	sockaddr_in	clientAdress;
	socklen_t	clientLen	= sizeof(clientAdress);

	int	clientFd = accept(this->_socket, (struct sockaddr*)&clientAdress, &clientLen);
	if (clientFd == -1)
	{
		std::cerr << "Error: Client Not Accept" << std::endl;
		return ;
	}

	int	flags	= fcntl(clientFd, F_GETFL, 0);
	fcntl(clientFd, F_SETFL, flags | O_NONBLOCK);

	struct	epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = clientFd;
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, clientFd, &ev) == -1)
	{
		close(clientFd);
		return ;
	}
	
	Client	*serverMember = new	Client(clientFd, inet_ntoa(clientAdress.sin_addr));
	serverMember->setConnState(WAITING_PASS);
	_clients.add(clientFd, serverMember);

	std::cout << "|========[Accepted New Client Connection]========|" << std::endl;
	std::cout << ">" << clientFd << ":" << inet_ntoa(clientAdress.sin_addr) << std::endl;
}

void        Server::deleteClientFromAllChannels(Client *client)
{
	std::map<std::string, Channel *>	&allChannels = _channel.getAll();

    for (std::map<std::string, Channel *>::iterator it = allChannels.begin(); it != allChannels.end(); )
    {
        Channel *chnl = it->second;
        if (chnl)
        {
            if (chnl->isMember(client))
                chnl->removeMember(client);
            if (chnl->isOperator(client))
                chnl->removeOperator(client);

            if (chnl->getMemberList().empty())
            {
                delete chnl;
				allChannels.erase(it++);
                continue;
            }
        }
        ++it;
    }
}

void	Server::_refuseClient(int fd)
{
	std::cout << "[Disconnected Client Connection]" << std::endl;
	struct	epoll_event	dummy;

	epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, &dummy);

	std::cout << "epoll table removing finish" << std::endl;
	Client	*delClient = _clients.get(fd);
	if (delClient)
	{
		std::cout << ">" << fd << ":" << delClient->getIp() << std::endl;
		deleteClientFromAllChannels(delClient);
		delete delClient;
		_clients.remove(fd);
	}
	fd = -1;
}

void	Server::_signalHandler(int signum)
{
	(void)signum;
	std::cout << "SIGNAL HANDLED" << std::endl;

	if (Server::_instance)
	{
		Server::_instance->_running = false;
		if (Server::_instance->_socket != -1)
		{
			close(Server::_instance->_socket);
			Server::_instance->_socket = -1;
		}
	}
}

void	Server::server_start()
{
	signal(SIGINT, Server::_signalHandler);
	signal(SIGTERM, Server::_signalHandler);
	_initSocket();
    _initEpoll();
	int	eventCount;

	this->_running = true;
	while (_running)
	{
		eventCount = epoll_wait(_epollFd, _events, MAX_EVENTS, -1);
		if (eventCount < 0) {
            if (errno == EINTR) continue;
            throw std::runtime_error("Hata: epoll_wait basarisiz.");
        }

        for (int i = 0; i < eventCount; ++i) {
            int triggeredFd = _events[i].data.fd;

			if (triggeredFd == _socket)
                _acceptClient();
			else
			{
				if (EPOLLIN & _events[i].events)
					_readerClient(triggeredFd);
				if (EPOLLOUT & _events[i].events)
					_writerClient(triggeredFd);
			}
        }
	}
}

