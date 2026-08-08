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

		Channel();
		Channel(const Channel& variant);
		Channel& operator=(const Channel& other);
	public:
		Channel(const std::string& name);
		~Channel();

		std::string	getName() const;
		std::string	getTopic() const;
		std::string	getKey() const;

		bool		isInviteOnly()		const;
		bool		isTopicRestricted()	const;
		size_t		getUserLimit()		const;

		const std::vector<Client *>	getMemberList() const;
		const std::vector<Client *>	getOperators() const;


		void	setName(const std::string& name);
		void	setTopic(const std::string& topic);
		void	setKey(const std::string& key);

		void	setInviteOnly(bool status) const;
		void	setTopicRestricted(bool status) const;
		void	setUserLimit(size_t avarage) const;

		void	addMember(Client *client);
		void	removeMember(Client *client);
		bool	isMember(Client *client) const;

		void	addOperator(Client *client);
		void	removeOperator(Client *client);
		bool	isOperator(Client *client);

		void	addInvite(Client *client);
		void	removeInvite(Client *client);
		bool	isInvite(Client *client);

		void	broadcast(const std::string& message, Client *exclude = NULL);
};

#endif
