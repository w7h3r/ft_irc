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
#include <unistd.h>

Server::Server()
{
	_port = DEFAULT_PORT;
	_socket = -1;
	_password = DEFAULT_PASSWORD;
	_running = false;
}

Server::~Server()
{
	if (_socket != -1)
		close(_socket);
}

void	Server::createSocket()
{

	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket < 0)
		throw std::runtime_error("Failed to create 'Server' socket");

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = _port;

	if (bind(_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
		throw std::runtime_error("Failed to bind 'Server' socket");
	if (listen(_socket, 128) < 0)
		throw std::runtime_error("Failed to listen on 'Server' socket");
}

/*
 * Server::start() -apperantly-starts the server's loop for handling connections and messages.
 * Empty fot now.
 */

void	Server::start()
{
	createSocket();
	_running = true;
	std::cout << "Server started on port " << _port << std::endl;

	while (_running)
	{

	}
}
