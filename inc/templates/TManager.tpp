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

#ifndef	TEMPLATE_TPP
# define TEMPLATE_TPP

template <typename K, typename V>
TManager<K, V>::TManager()
{

}

template <typename K, typename V>
TManager<K, V>::~TManager()
{
	this->clearAll();
}

template <typename K, typename V>
TManager<K, V>::TManager(const TManager& variant) : _data(variant._data)
{

}

// 4. Assignment Operator
template <typename K, typename V>
TManager<K, V>& TManager<K, V>::operator=(const TManager& other) {
    if (this != &other) {
        this->_data = other._data; // std::map derin kopyalama yapar
    }
    return *this;
}

template <typename K, typename V>
void TManager<K, V>::add(const K& key, const V& value) {
    _data[key] = value; 
}

template <typename K, typename V>
void TManager<K, V>::remove(const K& key) {
    _data.erase(key);
}

template <typename K, typename V>
V TManager<K, V>::get(const K& key) const {
    typename std::map<K, V>::const_iterator it = _data.find(key);
    
    if (it != _data.end()) {
        return it->second;
    }
    
    throw std::out_of_range("TManager: Istenen anahtar (key) bulunamadi!");
}

template <typename K, typename V>
void TManager<K, V>::clearAll() {
    _data.clear();
}

#endif
