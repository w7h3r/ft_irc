/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oozsipah <oozsipah@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/24 21:16:05 by alermi            #+#    #+#             */
/*   Updated: 2026/09/18 15:59:23 by alermi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server/Server.hpp"
#include "../inc/Commands.hpp"
#include "../inc/Client/Client.hpp"

#include <iostream>
#include <string>
#include <stdexcept>
#include <cerrno>
#include <csignal>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

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
	
	if (fcntl(_socket, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("Error: Non-blocking mode not set");
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
		std::cerr << "Epoll Change Mode Error" << std::endl;
}

void    Server::_writerClient(int fd)
{
    if (!_clients.exists(fd))
        return ;

    Client    *newClient = _clients.get(fd);
	if (!newClient)
        return ;
	if (newClient->isDisconnected() || newClient->isRefused())
	{
		_refuseClient(fd);
		return ;
	}
    std::string    &output = newClient->getWriteBuffer();
    if (output.empty())
    {
        _modifyEpoll(fd, EPOLLIN);
        return ;
    }
    ssize_t byteCount = send(fd, output.c_str(), output.size(), MSG_NOSIGNAL);
    if (byteCount > 0)
    {
        output.erase(0, byteCount);
        if (output.empty())
            _modifyEpoll(fd, EPOLLIN);
    }
    else if (byteCount <= 0)
        _refuseClient(fd);
}

void    Server::_readerClient(int fd)
{
    char    buffer[1024];

    ssize_t    contentByte = recv(fd, buffer, sizeof(buffer) - 1, 0);
    if (contentByte <= 0)
        _refuseClient(fd);
    else
    {
        buffer[contentByte] = '\0';
        std::cout << "> " << fd << " (RECV): " << buffer << std::endl;

        Client    *newClient = _clients.get(fd);
		if (newClient)
        {
            newClient->appendToReadBuffer(buffer);

            while (newClient->hasCompleteCommand())
            {
                std::string    rawCommands = newClient->extractCommand();
                Command cmd = newClient->parseMessage(rawCommands);
                
                decideCommand(newClient, cmd, _clients, _channel, _password);

                if (!_clients.exists(fd))
                {
                    std::cout << "Client " << fd << " silindi, okuma döngüsünden çıkılıyor." << std::endl;
                    return ;
                }
            }
            if (!newClient->getWriteBuffer().empty())
            {
                _modifyEpoll(fd, EPOLLIN | EPOLLOUT);
            }
        }
    }
}

void	Server::enableWriteEvent(int fd)
{
	_modifyEpoll(fd, EPOLLIN | EPOLLOUT);
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

	fcntl(clientFd, F_SETFL, O_NONBLOCK);
	
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

void        Server::_deleteClientFromAllChannels(Client *client)
{
    std::map<std::string, Channel *>    &allChannels = _channel.getAll();
    std::map<std::string, Channel *>::iterator it = allChannels.begin();

    while (it != allChannels.end())
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
			}
            else
                it++;
        }
        else
            it++;
    }
}

void    Server::_refuseClient(int fd)
{
    std::cout << "[Disconnected Client Connection]" << std::endl;
    struct    epoll_event    dummy;

    epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, &dummy);
    close(fd); 
    
	if (!_clients.exists(fd))
		return ;

    Client    *delClient = _clients.get(fd);
	std::cout << ">" << fd << ":" << delClient->getIp() << std::endl;

	_deleteClientFromAllChannels(delClient);
	delete delClient;
	_clients.remove(fd);
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
	signal(SIGPIPE, SIG_IGN);
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
