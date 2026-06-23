/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TManager.ipp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alermi <alermi@student.42kocaeli.com.tr>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/18 14:16:28 by alermi            #+#    #+#             */
/*   Updated: 2026/06/23 14:35:12 by muokcan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 * These methods are related to TManager as well, however they are seperated
 * from .tpp file because of organisational and optimizational purposes.
 * */

template <typename K, typename V>
inline bool TManager<K, V>::exists(const K& key) const {
    return (_data.find(key) != _data.end());
}

template <typename K, typename V>
inline const std::map<K, V>& TManager<K, V>::getAll() const {
    return _data;
}
