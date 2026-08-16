/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oozsipah <oozsipah@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/18 14:16:44 by alermi            #+#    #+#             */
/*   Updated: 2026/08/16 05:35:26 by oozsipah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	CLIENT_HPP
# define CLIENT_HPP

#include <string>
#include <vector>

enum	parseState
{
	PARSE_UNKNOWN = 0,
	PARSE_RECEIVED,
	PARSE_DELIMITER,
	PARSE_EXTRACT,
	PARSE_EXECUTE,
	PARSE_ERROR,
};

enum	connectionState
{
	CONN_UNKNOWN = 0,
	CONNECT,
	WAITING_PASS,
	WAITING_NICK,
	WAITING_INFO,
	ACCEPT,
	DISCONNECT,
	REFUSED,
};

struct	Command
{
	std::string					type;
	std::vector<std::string>	params;
	std::string					message;
	
};

class	Client
{
	private:
		int					_fd;
		std::string			_ip;

		std::string			_name;
		std::string			_nickname;
		std::string			_password;

		connectionState		_connState;
		parseState			_parseState;

		std::string			_writeBuffer;
		std::string			_readBuffer;
		bool				_OP;

		Client(const Client& variant);
		Client& operator=(const Client& other);
		
	public:
		Client();

		Client(int fd, const std::string& ip);
		~Client();

		int				getFd() const;
		std::string		getIp() const;
		bool			isOP() const;
		void			setOP(bool op);

		bool			isRegistered() const;

		bool			isDisconnected() const;
		bool			isRefused() const;
		bool			isWaitingPass() const;
		bool			isWaitingNick() const;
		bool			isWaitingInfo() const;

		connectionState	getConnState() const;
		void			setConnState(connectionState state);

		parseState		getParseState() const;
		void			setParseState(parseState state);
		
		std::string		getNickname() const;
		void			setNickname(const std::string& nick);
		
		std::string		getUsername() const;
		void			setUsername(const std::string& user);

		void			appendToReadBuffer(const std::string& data);
		void			appendToWriteBuffer(const std::string& data);

		bool			hasValidCredentials() const;

		std::string&	getWriteBuffer();
		void			clearWriteBuffer();

		bool			hasCompleteCommand() const;
		std::string		extractCommand();

		Command			parseMessage(const std::string& message);
};

#endif
