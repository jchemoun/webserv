/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/12 11:27:23 by mjacq             #+#    #+#             */
/*   Updated: 2022/05/20 10:27:58 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <sstream>
# include <map>
# include "http_response_codes.hpp"
# include "color.hpp"

namespace utils {

	template <class T>
		std::string	to_str(T streamable_obj) {
			std::ostringstream	oss;
			oss << streamable_obj;
			return (oss.str());
		}

	void	epoll_ctl(int epfd, int operation, int fd, int events);

	/*
	** @brief return a pointer to the value pointed by a key in a map
	**        or NULL if not found
	*/
	template <class Key, class Value>
		Value	*get(std::map<Key, Value> &m, Key const &key) {
			typename std::map<Key, Value>::iterator it = m.find(key);
			if (it == m.end())
				return (NULL);
			else
				return (&it->second);
		}
	/*
	** @brief return a pointer to the value pointed by a key in a CONST map
	**        or NULL if not found
	*/
	template <class Key, class Value>
		Value const	*get(std::map<Key, Value> const &m, Key const &key) {
			return get(const_cast<std::map<Key, Value> &>(m), key);
		}
}

#endif
