/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TManager.tpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oozsipah <oozsipah@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/18 14:16:33 by alermi            #+#    #+#             */
/*   Updated: 2026/08/17 00:19:41 by oozsipah         ###   ########.fr       */
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

template <typename K, typename V> // -omer
std::vector<V> TManager<K, V>::getAllValues()
{
    std::vector<V> values;
    std::map<K, V>::const_iterator it = _data.begin();
    for (it; it < _data.end(); it++)
        values.std::push_back(it->second);
    return (values);
}

#endif
