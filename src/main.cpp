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

int	main(void)
{
	Server server(4242, "gebze");

	try 
	{
		server.start();
	}
	catch (const std::exception& e) 
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}
}
