/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alermi <alermi@student.42kocaeli.com.tr>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/18 14:16:40 by alermi            #+#    #+#             */
/*   Updated: 2026/05/24 21:28:58 by muokcan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	CHANNEL_HPP
# define CHANNEL_HPP

#include <vector>
#include <iostream>

#include "../Client/Client.hpp"

class	Channel
{
	private:
		std::string	_name;
		std::string	_topic;
		std::string	_key;

		bool	_isInviteOnly;
		bool	_topicRestricted;
		bool	_userLimit;

		std::vector<Client *>	_members;
		std::vector<Client *>	_operator;
		std::vector<Client *>	_inviteUser;
	public:
};

#endif
