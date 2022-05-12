/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/12 11:27:23 by mjacq             #+#    #+#             */
/*   Updated: 2022/05/12 11:41:16 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <sstream>

namespace utils {

	std::string get_file_extension(std::string const &path);

	template <class T>
		std::string	to_str(T streamable_obj) {
			std::ostringstream	oss;
			oss << streamable_obj;
			return (oss.str());
		}
}

#endif
