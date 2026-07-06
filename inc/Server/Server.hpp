/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alermi <alermi@student.42kocaeli.com.tr>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/18 14:16:49 by alermi            #+#    #+#             */
/*   Updated: 2026/06/18 20:20:17 by alermi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	SERVER_HPP
# define SERVER_HPP

# include "../Channel/Channel.hpp"
# include <iostream>
# include <sys/epoll.h>
# include "../templates/TManager.hpp"
# include "../Client/Client.hpp"

# define LIVE 1
# define DEAD 0

# define DEFAULT_PORT 4242
# define MAX_EVENTS	512 // MAX event size: Genellikle ağlarla ilgili TCP IP server dokümanlarında nonblock sistemlerde verimli bir event sayısı olarak kabul edilir ne sistemi çok yormayacak bir sayı ne de çok küçük bir sayı diğer bir alternatif 1024
# define DEFAULT_PASSWORD "gebze"

class	Server
{
	private:
		int					_port;
		int					_socket;
		std::string			_password;
		bool				_running;

		int					_epollFd;
		struct	epoll_event	_events[MAX_EVENTS];

		TManager<int, Client *>	_clients;
		TManager<std::string, Channel *> _channel;
		
		Server(); // BU KISMI BI TAM NETLE;
		Server(const Server& variant);
		Server& operator=(const Server& other);

		void	_initSocket();
		void	_initEpoll();

		void	_acceptClient();
		void	_refuseClient(int fd);
		void	_kickClient(int fd);
		void	_bannClient(int fd);

		void	_readerClient(int fd);
		void	_writerClient(int fd);

	public:
		Server(int port, const std::string& password);
		~Server();

		int					getPort() const;
		std::string			getPassword() const;
		bool				running();
		void				server_start();
		void				stop();
};

#endif
