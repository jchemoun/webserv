/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/12 11:27:23 by mjacq             #+#    #+#             */
/*   Updated: 2022/05/13 16:42:39 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <sstream>

namespace utils {

	template <class T>
		std::string	to_str(T streamable_obj) {
			std::ostringstream	oss;
			oss << streamable_obj;
			return (oss.str());
		}

	void	epoll_ctl(int epfd, int operation, int fd, int events);
}

#endif
