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

}




















