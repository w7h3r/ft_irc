/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TManager.tpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alermi <alermi@student.42kocaeli.com.tr>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/18 14:16:33 by alermi            #+#    #+#             */
/*   Updated: 2026/06/23 14:32:27 by muokcan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 * Methods of Tmanager class template.
 */

template <typename K, typename V>
inline Tmanager<K, V>::TManager() : {}

template <typename K, typename V>
inline TManager<K, V>::~TManager()
{
	clearAll();
}

template <typename K, typename V>
inline TManager<K, V>::TManager(const TManager& variant) : _data(variant._data) {}

template <typename K, typename V>
inline TManager<K, V>& TManager<K, V>::operator=(const TManager& other)
{
	if (this != &other)
		_data = other._data;
	return *this;
}


template <typename K, typename V>
inline void TManager<K, V>::add(const K& key, const V& value)
{
	_data[key] = value;
}

template <typename K, typename V>
inline void TManager<K, V>::remove(const K& key, const V& value)
{
	(void)value;
	std::map<K, V>::iterator it = _data.find(key);
	if (it != _data.end())
		_data.erase(it);
	throw std::runtime_error("Key not found in TManager.");
}

template <typename K, typename V>
inline V TManager<K, V>::get(const K& key) const
{
	std::map<K, V>::const_iterator it = _data.find(key);
	if (it != _data.end())
		return it->second;
	throw std::runtime_error("Key not found in TManager.");
}

template <typename K, typename V>
inline void TManager<K, V>::clearAll()
{
	_data.clear();
}
