/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alermi <alermi@student.42kocaeli.com.tr>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/18 14:16:49 by alermi            #+#    #+#             */
/*   Updated: 2026/05/24 21:28:52 by muokcan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	SERVER_HPP
# define SERVER_HPP

# include "../Channel/Channel.hpp"
# include <iostream>

# define LIVE 1
# define DEAD 0

/*
 * Completely random values defined below. These are just placeholders.
 */

# define DEFAULT_PORT 4242
# define DEFAULT_PASSWORD "gebze"

class	Server
{
	private:
		int					_port;
		int					_socket;
		std::string			_password;
		bool				_running;

		void				handleClient(int client_socket);
		void				broadcastMessage(const std::string& message, int sender_socket);
		void				createSocket();

	public:
		Server();
		~Server();

		void				start();
		void				stop();
};

#endif
