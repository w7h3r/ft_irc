/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: muokcan <muokcan@student.42kocaeli.com.tr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/24 21:16:05 by muokcan           #+#    #+#             */
/*   Updated: 2026/05/24 21:16:06 by muokcan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server/Server.hpp"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
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

void	Server::_readerClient(int fd)
{
	char	buffer[1024];

	ssize_t	contentByte = recv(fd, buffer, sizeof(buffer) - 1, 0);
	if (contentByte <= 0)
		_refuseClient(fd);
	else
	{
		Client	*evClient = _clients.get(fd);
		evClient->appendToReadBuffer(buffer);
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

	std::cout << "|========[Accepted New Client Connection]========|" << std::endl;
	std::cout << ">" << clientFd << ":" << inet_ntoa(clientAdress.sin_addr) << std::endl;

	//========================Her Client'ı temsilen yeni bir Client Objesi oluşturulur
	
	Client	*serverMember = new	Client(clientFd, inet_ntoa(clientAdress.sin_addr));
	_clients.add(clientFd, serverMember);
}

void	Server::_refuseClient(int fd);
{
	std::cout << "refusing connect" << std::endl;
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

            if (triggeredFd == _socket) {
                _acceptClient();
            }
            else {
                _readerClient(triggeredFd);
            }
        }
	}
}

