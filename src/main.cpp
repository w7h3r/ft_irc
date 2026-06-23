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

#include "../inc/Client/Client.hpp"
#include <map>
#include <iostream>

int	main(void)
{
	Client	a;

	cmd b = a.parseMessage("    	USER alice 0 * :Alice Smith\r\n");
	std::cout << "TYPE [" << b.type << "]" << std::endl;
	std::cout << "PARAMS [" << b.params.size() << "]" << std::endl;
	std::cout << "MESSAGE [" << b.message << "]" << std::endl;
}
