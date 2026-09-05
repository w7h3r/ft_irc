/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oozsipah <oozsipah@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/24 21:16:05 by muokcan           #+#    #+#             */
/*   Updated: 2026/08/17 00:22:33 by oozsipah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server/Server.hpp"
#include <iostream>
#include <stdexcept>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <asm-generic/socket.h>
#include <cerrno>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdexcept>
#include <sys/types.h>
#include <string>
#include <unistd.h>

Server::Server(int port, const std::string& password) : 
    _port(port),
    _socket(-1),
    _password(password),
    _running(false),
    _epollFd(-1)
{
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

void	Server::_writerClient(int fd)
{
		Client	*newClient = _clients.get(fd);
		if (newClient->getWriteBuffer().empty())
			return ;
		if (newClient)
		{
			std::string	&output = newClient->getWriteBuffer();
			ssize_t	byteCount = send(fd, output.c_str(), output.size(), 0);
			if (byteCount > 0)
				output.erase(0, byteCount);
			else if (byteCount <= 0)
				_refuseClient(fd);
			else
			 	std::runtime_error("Error: _writerClient");
		}
		else
			std::runtime_error("Error: _writerClient");
}

/* Added Temporarily */

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


void	cmdJoin(Client *client, struct Command cmd, TManager<int, Client *> clients, TManager<std::string, Channel *> channels)
{
	(void)client;
	(void)cmd;
	(void)clients;
	(void)channels;
	std::cout << "Processing JOIN Command" << std::endl;
}
void	cmdKick(Client *client, struct Command cmd, TManager<int, Client *> clients, TManager<std::string, Channel *> channels)
{
	(void)client;
	(void)cmd;
	(void)clients;
	(void)channels;
	std::cout << "Processing KICK Command" << std::endl;
}
void    cmdInvite(Client *client, struct Command cmd, TManager<int, Client *> clients, TManager<std::string, Channel *> channels)
{
	(void)client;
	(void)cmd;
	(void)clients;
	(void)channels;
	std::cout << "Processing INVITE Command" << std::endl;
}
void    cmdTopic(Client *client, struct Command cmd, TManager<int, Client *> clients, TManager<std::string, Channel *> channels)
{
	(void)client;
	(void)cmd;
	(void)clients;
	(void)channels;
	std::cout << "Processing TOPIC Command" << std::endl;
}
void    cmdMode(Client *client, struct Command cmd, TManager<int, Client *> clients, TManager<std::string, Channel *> channels)
{
	(void)client;
	(void)cmd;
	(void)clients;
	(void)channels;
	std::cout << "Processing MODE Command" << std::endl;
}

static void	decideCommand(Client *client, struct Command cmd, TManager<int, Client *> clients, TManager<std::string, Channel *> channels)
{
	
	if (cmd.type == "JOIN")
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
		_refuseClient(fd);
	else
	{
		buffer[contentByte] = '\0';
		std::cout << "> " << fd << ":" << buffer << std::endl; //DEBUG
		Client	*newClient = _clients.get(fd);
		if (newClient)
		{
			newClient->appendToReadBuffer(buffer);
			while (newClient->hasCompleteCommand())
			{
				std::string	rawCommands = newClient->extractCommand();
				decideCommand(newClient, newClient->parseMessage(rawCommands), _clients, _channel);
				std::cout << "Processing Command: " << rawCommands << std::endl; //DEBUG
			}
		}
	};
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
	_clients.add(clientFd, serverMember);

	std::cout << "|========[Accepted New Client Connection]========|" << std::endl;
	std::cout << ">" << clientFd << ":" << inet_ntoa(clientAdress.sin_addr) << std::endl;
}

void	Server::_refuseClient(int fd)
{
	std::cout << "[Disconnected Client Connection]" << std::endl;
	
	epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL);
	close (fd);

	std::cout << "epoll table removing finish" << std::endl;
	Client	*delClient = _clients.get(fd);
	if (delClient)
	{
		std::cout << ">" << fd << ":" << delClient->getIp() << std::endl;
		delete delClient;
		_clients.remove(fd);
	}
	std::cout << " DEBUG: ERROR => Client removing not possible cache updated(please leakcheck)" << std::endl;
}

void	Server::server_start()
{
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
				else if (EPOLLOUT & _events[i].events)
					_writerClient(triggeredFd);
			}
        }
	}
}

