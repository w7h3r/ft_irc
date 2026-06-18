/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TManager.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alermi <alermi@student.42kocaeli.com.tr>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/18 14:16:24 by alermi            #+#    #+#             */
/*   Updated: 2026/06/17 17:57:26 by alermi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef TMANAGER_HPP
# define TMANAGER_HPP

# include <map>
# include <stdexcept>

template <typename  K, typename V>

class	TManager
{
	private:
		std::map<K, V> _data;
	public:

		//---------------------------------
		//						OCF METHODS
		//---------------------------------
		TManager();
		~TManager();
		TManager(const TManager& variant);
		TManager&	operator=(const TManager& other);

		//---------------------------------
		//						BASE METHOD
		//---------------------------------
		void	add(const K& key, const V& value);
		void	remove(const K& key, const V& value);
		V		get(const K& key) const;
		void	exist(const K& key) const;
		const	std::map<K, V>& getAll() const;
		void	clearAll();

};


# include "TManager.ipp"
# include "TManager.tpp"

#endif
