/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/12 11:55:57 by mjacq             #+#    #+#             */
/*   Updated: 2022/05/19 18:19:17 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILE_HPP
# define FILE_HPP

# include <string>

namespace file {

	enum	e_type { FT_UNKOWN, FT_DIR, FT_FILE };

	e_type		get_type(std::string const &path);
	bool		has_read_perm(std::string const &path);
	bool		has_write_perm(std::string const &path);
	bool		has_exec_perm(std::string const &path);
	std::string	time_last_change(std::string const &file);
	long		size(std::string const &file);
	std::string get_extension(std::string const &path);
	std::string	join(std::string const &head, std::string const &tail);

}

// class File {
// public:
// 	File(void);
// 	File(File const &copy);
// 	File	&operator=(File const &copy);
// 	virtual ~File(void);
// };

#endif
