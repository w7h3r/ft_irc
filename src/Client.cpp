/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oozsipah <oozsipah@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/24 21:16:09 by muokcan           #+#    #+#             */
/*   Updated: 2026/09/06 22:37:06 by oozsipah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Client/Client.hpp"
#include <unistd.h>
#include <vector>
#include <sstream>

Client::Client() { }

Client::Client(const Client& variant) {(void)variant; }

Client &Client::operator=(const Client& other) {(void)other; return (*this); }

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
	_channelCount = 0;
}

Client::~Client()
{
	if (_fd != -1)
		close(_fd);
}

int				Client::getFd() const { return (_fd); }

std::string		Client::getIp() const { return (_ip); }

connectionState	Client::getConnState() const { return (_connState); }
void			Client::setConnState(connectionState state) { _connState = state; }

bool			Client::isOP() const { return (_OP); }
void			Client::setOP(bool isOp) { _OP = isOp; }

bool			Client::isRegistered() const { return (_connState == ACCEPT); }
bool			Client::isDisconnected() const { return (_connState == DISCONNECT); }
bool			Client::isRefused() const { return (_connState == REFUSED); }
bool			Client::isWaitingPass() const { return (_connState == WAITING_PASS); }
bool			Client::isWaitingNick() const { return (_connState == WAITING_NICK); }
bool			Client::isWaitingInfo() const { return (_connState == WAITING_INFO); }

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

void			Client::incrementChannelCount() {
	if (_channelCount < MAX_CHANNEL_COUNT)
		_channelCount++;
	return ;
}
void			Client::decrementChannelCount()
{
	if (_channelCount > 0)
		_channelCount--;
	return ;
}
int				Client::getChannelCount() const { return(_channelCount); }

// This fuction is almost useless
// then why do u even declare it ? -omer
std::string		Client::extractCommand()
{
	std::string	cmd;
	size_t		pos = _readBuffer.find("\r\n");
	if (pos != std::string::npos)
	{
		cmd = _readBuffer.substr(0, pos);
		_readBuffer.erase(0, pos + 2);
	}
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

// resizing the message to 512, cause maximum lenght of a message is 512 bytes defined by RFC 1459(or something like that)

Command	Client::parseMessage(const std::string& rawMessage)
{
	std::string	message = rawMessage;
	Command		command;

	if (message.length() > 512)
		message = message.substr(0, 512);
	if (message.length() >= 2 && message.substr(message.length() - 2) == "\r\n")
		message = message.substr(0, message.length() - 2);
	size_t	pos = message.find(" :");
	if (pos != std::string::npos)
	{
		command.message = message.substr(pos + 2);
		message = message.substr(0, pos);
	}
	std::vector<std::string>	words = splitWords(message);
	if (!words.empty())
	{
		command.type = words[0];
		for (size_t i = 1; i < words.size(); ++i)
			command.params.push_back(words[i]);
	}
	return (command);
}

static	bool	isValidNickname(const std::string& str)
{
	
	if (str.empty())
		return (false);
	for (size_t i = 0; i < str.length(); ++i)
	{
		if (!isalnum(str[i]) && str[i] != '-' && str[i] != '_')
			return (false);
	}
	return (true);
}

static	bool	isValidUsername(const std::string& str)
{
	if (str.empty())
		return (false);
	for (size_t i = 0; i < str.length(); ++i)
	{
		if (!isalnum(str[i]) && str[i] != '-' && str[i] != '_')
			return (false);
	}
	return (true);
}

static	bool	isValidPassword(const std::string& str)
{
	if (str.empty())
		return (false);
	for (size_t i = 0; i < str.length(); ++i)
	{
		if (!isalnum(str[i]) && str[i] != '-' && str[i] != '_')
			return (false);
	}
	return (true);
}

static	bool	hasRegistered(const Client* client)
{
	return (client->isRegistered());
}

bool	Client::hasValidCredentials() const
{
	return (isValidNickname(_nickname) && isValidUsername(_name) && isValidPassword(_password) && hasRegistered(this));
}

