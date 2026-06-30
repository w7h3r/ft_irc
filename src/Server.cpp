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
#include "../inc/Exception/Exception.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <exception>
#include <unistd.h>

Server::Server(int port, const std::string& password) : _port(port),
                                                        _socket(-1),
                                                        _password(password),
                                                        _running(false)
{
  std::cout << "Server Constructor Called" << std::endl;
}

Server::~Server()
{
    std::cout << "Destrcutor Called" << std::endl;
    if (_socket != -1)
        close(_socket);
}

void	Server::server_start()
{
	int	opt = 1;
	_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (_socket == -1)
		std::cout << "Error Socket Not Open" << std::endl;
	else
		std::cout << "Socket Success open" << std::endl;
	std::cout << _socket << std::endl;

	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw (std::runtime_error("Error socket option:SO_REUSEADDR not set");

	if (setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)) < 0)
		throw(std::runtime_error("Error socket option: TCP_NODELAY not set"));
	
	struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(_port);

if (bind(_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
        throw std::runtime_error("Error: Bind do not success. Ports already in use");

    if (listen(_socket, SOMAXCONN) < 0) //bu kısımda bulunan somaxconn değerini 42 bilgisayarlarına göre güncelle zaten cap işlemi var
        throw std::runtime_error("Error: listen not working ");

    int flags = fcntl(_socket, F_GETFL, 0);
    if (flags == -1 || fcntl(_socket, F_SETFL, flags | O_NONBLOCK) == -1)
        throw std::runtime_error("Error: Non-blocking not set");
}




















