/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oozsipah <oozsipah@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/18 14:16:49 by alermi            #+#    #+#             */
/*   Updated: 2026/08/17 00:19:20 by oozsipah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	SERVER_HPP
# define SERVER_HPP

# include "../Channel/Channel.hpp"
# include "../templates/TManager.hpp"
# include "../Client/Client.hpp"
# include <sys/epoll.h>

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

		static	Server*		_instance;
		static	void		_signalHandler(int signum);

		TManager<int, Client *>	_clients;
		TManager<std::string, Channel *> _channel;
		
		Server();
		Server(const Server& variant);
		Server& operator=(const Server& other);

		void	_initSocket();
		void	_initEpoll();

		void	_acceptClient();
		void	_kickClient(int fd);
		void	_bannClient(int fd);

		void	_readerClient(int fd);
		void	_writerClient(int fd);

		void	_processCommand(Client* client, const std::string& rawCommand);
		void	_modifyEpoll(int fd, int events);
	
		void	_deleteClientFromAllChannels(Client *client);
	public:
		Server(int port, const std::string& password);
		~Server();

		Client	*getClientByUsername(std::string &username); // -omer
		std::vector<Client *> getAllClients();

	
		void				_refuseClient(int fd);
		static	Server		*getInstance() { return _instance; }
		void				enableWriteEvent(int fd);
		int					getPort() const;
		std::string			getPassword() const;
		bool				running();
		void				server_start();
		void				stop();

};

#endif
