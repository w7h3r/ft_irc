/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: muokcan <muokcan@student.42kocaeli.com.tr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/24 21:16:09 by muokcan           #+#    #+#             */
/*   Updated: 2026/06/23 16:34:39 by muokcan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Client/Client.hpp"
#include <vector>
#include <sstream>

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

parseState		Client::getParseState() const { return ( _parseState); }
void			Client::setParseState(parseState state) { _parseState = state; }

std::string		Client::getNickname() const { return ( _nickname); }
void			Client::setNickname(const std::string& nick) { _nickname = nick; }

std::string		Client::getUsername() const { return (_name); }
void			Client::setUsername(const std::string& userName) { _name = userName; }

void			Client::appendToReadBuffer(const std::string& data) { _readBuffer += data; }
void			Client::appendToWriteBuffer(const std::string& data) { _writeBuffer += data; }

std::string&	Client::getWriteBuffer() { return (_writeBuffer); }
void			Client::clearWriteBuffer() { _writeBuffer.clear(); }

bool			Client::hasCompleteCommand() const { return (_readBuffer.find("\r\n") != std::string::npos); }

std::string		Client::extractCommand()
{
	size_t	pos = _readBuffer.find("\r\n");

	if (pos == std::string::npos)
		return ("");

	std::string	cmd = _readBuffer.substr(0, pos);
	_readBuffer = _readBuffer.substr(pos + 2);

	return (cmd);
}

static	std::vector<std::string>	splitWords(const std::string &message)
{
	std::vector<std::string>	wordList;
	std::istringstream			ss(message);
	std::string					word;

	while(ss >> word)
		wordList.push_back(word);

	return (wordList);
}

Command	Client::parseMessage(const std::string& rawMessage)
{
	std::string		tmpMessage;
	size_t			idx;
	Command			command;

	idx = rawMessage.find(':');
	if (idx != std::string::npos)
		command.message = rawMessage.substr(idx + 1, rawMessage.length() - (idx + 3));
	else
		command.message = "";

	std::vector<std::string> wordList = splitWords(rawMessage.substr(0, idx));
	command.type = wordList[0];

	for(size_t i = 1; i < wordList.size(); i++)
		command.params.push_back(wordList[i]);

	return (command);
}

