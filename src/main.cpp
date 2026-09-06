/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oozsipah <oozsipah@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/24 21:17:43 by muokcan           #+#    #+#             */
/*   Updated: 2026/08/16 05:40:26 by oozsipah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Client/Client.hpp"
#include <map>
#include <iostream>
#include "../inc/Server/Server.hpp"
#include "../inc/Exception/Exception.hpp"
#include "../inc/templates/TManager.hpp"

#include <cstdlib>
#include <exception>
int main(int argc, char **argv)
{
	if (argc != 3)
	{
		return (1);
	}
	int		port = std::atoi(argv[1]);
	std::string	password = argv[2];

	try {

		Server	server(port, password);
		server.server_start();
	}
	catch (const std::exception& e) {
		std::cout << "Server internal Error:" << e.what() << std::endl;
		return (1);
	}
	
	return (0);
}
