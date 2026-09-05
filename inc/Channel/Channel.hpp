/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oozsipah <oozsipah@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/18 14:16:40 by alermi            #+#    #+#             */
/*   Updated: 2026/09/05 23:22:32 by oozsipah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	CHANNEL_HPP
# define CHANNEL_HPP

#include <vector>
#include <iostream>
#include <string>

#include "../Client/Client.hpp"

class	Channel
{
	private:
		std::string	_name;
		std::string	_topic;
		std::string	_key;
	
		bool	_isInviteOnly;
		bool	_isTopicRestricted;
		bool	_isUserLimit;

		size_t	_userLimit;
		
		std::vector<Client *>	_members;
		std::vector<Client *>	_operators;
		std::vector<Client *>	_inviteUsers;

		Channel(const Channel& variant);
		Channel& operator=(const Channel& other);
	public:
		Channel();
		Channel(const std::string& name, const std::string &key);
		~Channel();

		std::string	getName() const;
		std::string	getTopic() const;
		std::string	getKey() const;

		bool		isInviteOnly()		const;
		bool		isTopicRestricted()	const;
		bool		isUserLimit()		const;
		size_t		getUserLimit()		const;

		const std::vector<Client *>	getMemberList() const;
		const std::vector<Client *>	getOperators() const;


		void	setName(const std::string& name);
		void	setTopic(const std::string& topic);
		void	setKey(const std::string& key);

		void	setInviteOnly(bool& status);
		void	setTopicRestricted(bool& status);
		void	setUserLimit(size_t& limit);

		Client	*getMember(std::string Name);
		void	addMember(Client *client);
		void	removeMember(Client *client);
		bool	isMember(Client *client) const;
		
		void	addOperator(Client *client);
		void	removeOperator(Client *client);
		bool	isOperator(Client *client) const;

		void	addInvite(Client *client);
		void	removeInvite(Client *client);
		bool	isInvite(Client *client) const;

		
		void	broadcast(const std::string& message, Client *exclude = NULL);

};

#endif
