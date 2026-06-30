/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: muokcan <muokcan@student.42kocaeli.com.tr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/24 21:17:43 by muokcan           #+#    #+#             */
/*   Updated: 2026/05/24 21:29:37 by muokcan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Channel/Channel.hpp"
#include "../inc/Client/Client.hpp"
#include "../inc/Server/Server.hpp"
#include "../inc/Exception/Exception.hpp"

int(int argc, char **argv)
{
	if (ac != 3)
	{
		return (1);
	}
	int		port = std::atoi(argv[1]);
	std::string	password = argv[2];

	try {

		Server	server(port, password);
		server.start();
	}
	catch (const std::exception&) {
		std::cout << "Server internal Error:" << e.what() << std::endl;
	}
}
