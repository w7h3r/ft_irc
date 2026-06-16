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

class	Client
{
	private:
		int					_socket;
		std::string			_nickname;
		std::string			_username;
		std::string			_realname;

	public:
		Client(int socket);
		~Client();

		int					getSocket() const;
		std::string			getNickname() const;
		std::string			getUsername() const;
		std::string			getRealname() const;
		void				setNickname(const std::string& nickname);
		void				setUsername(const std::string& username);
		void				setRealname(const std::string& realname);

};

#endif
