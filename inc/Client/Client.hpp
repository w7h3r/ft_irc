/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alermi <alermi@student.42kocaeli.com.tr>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/18 14:16:44 by alermi            #+#    #+#             */
/*   Updated: 2026/05/24 21:28:48 by muokcan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <string>

enum	parseState
{
	PARSE_RECEIVED,
	PARSE_DELIMITER,
	PARSE_EXTRACT,
	PARSE_EXECUTE,
	PARSE_ERROR,
};

enum	connectionState
{
	CONNECT,
	WAITING_PASS,
	WAITING_NICK,
	WAITING_INFO,
	ACCEPT,
	DISCONNECT,
	REFUSED,
};

class	Client
{
	private:
		int					_fd;
		std::string			_Ip;

		std::string			_name;
		std::string			_nickname;
		std::string			_password;

		connectionState		_connState;
		parseState			_parseState;

		std::string			_writeBuffer;
		std::string			_readBuffer;
		bool				_OP;

		Client();
		Client(const Client& variant);
		Client& operator=(const Client& other);

	public:

		Client(int fd, const std::string& ip);
		~Client();

		//---------------------------------------------
		//									   GETTTERS
		//---------------------------------------------
		int				getFd() const;
		std::string		getIp() const;

		connectionState	getConnState() const;
		void			setConnState(connectionState state);

		parseState		getParseState() const;
		void			setParseState(parseState state);
		
		std::string		getNickname() const;
		void			setNickname(const std::string& nick);
		
		std::string		getUsername() const;
		void			setUsername(const std::string& user);

		void			setRealname(const std::string& real);
		void			appendToReadBuffer(const std::string& data);
		void			appendToWriteBuffer(const std::string& data);

		std::string&	getWriteBuffer();
		void			clearWriteBuffer();

		bool			hasCompleteCommand() const;
		std::string		extractCommand();};

#endif
