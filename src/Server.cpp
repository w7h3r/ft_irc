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
#include <unistd.h>

Server::Server()
{
	init();
}

Server::~Server()
{
	if (_socket != -1)
		close(_socket);
}

void	Server::init()
{
	_port = DEFAULT_PORT;
	_socket = -1;
	_password = DEFAULT_PASSWORD;
	_running = false;
}

/*
 * Server::start() -apperantly-starts the server's loop for handling connections and messages.
 * Empty fot now.
 */

void	Server::start()
{
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket < 0)
		throw std::runtime_error("Failed to create 'Server' socket");
	_running = true;
	std::cout << "Server started on port " << _port << std::endl;

	while (_running)
	{

	}
}
