/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: muokcan <muokcan@student.42kocaeli.com.tr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/24 21:16:09 by muokcan           #+#    #+#             */
/*   Updated: 2026/05/24 21:16:09 by muokcan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Client/Client.hpp"

Client::Client() { }

Client::Client(const Client& variant) { }

Client &Client::operator=(const Client& other) { return (*this); }

Client::Client(int fd, const std::string &ip) :
	_fd(fd), _ip(ip)
{
	_name = "";
	_nickname = "";
	_password = "";
	_connState = CONN_UNKNOWN;
	_parseState = PARSE_UNKNOWN;
	_writeBuffer = "";
	_readBuffer = "";
	_OP = false;
}

Client::~Client() { }

int				Client::getFd() const { return (_fd); }

std::string		Client::getIp() const { return (_ip); }

connectionState	Client::getConnState() const { return (_connState); }
void			Client::setConnState(connectionState state) { _connState = state; }

parseState		Client::getParseState() const {return ( _parseState); }
void			Client::setParseState(parseState state) { _parseState = state; }

std::string		Client::getNickname() const {return ( _nickname); }
void			Client::setNickname(const std::string& nick) { _nickname = nick; }

std::string		Client::getUsername() const {return (_name);}
void			Client::setUsername(const std::string& userName) { _name = userName; }

void			Client::appendToReadBuffer(const std::string& data) { _readBuffer += data; }
void			Client::appendToWriteBuffer(const std::string& data) { _writeBuffer += data; }

std::string&	Client::getWriteBuffer() { return (_writeBuffer); }
void			Client::clearWriteBuffer() { _writeBuffer.clear(); }

bool			Client::hasCompleteCommand() const { return (_readBuffer.find("\r\n") != std::string::npos); }

std::string		Client::extractCommand()
{
	size_t	pos = _writeBuffer.find("\n\r");

	if (pos == std::string::npos)
		return ("");

	std::string	cmd = _writeBuffer.substr(0, pos);
	_writeBuffer = _writeBuffer.substr(pos + 2);

	return (cmd);
}
