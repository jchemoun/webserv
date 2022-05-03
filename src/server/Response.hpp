/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/30 14:02:03 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/03 07:36:00 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <cstddef>
# include <string>
# include <iostream>
# include <fstream>
# include <sstream>
# include <sys/stat.h>
# include <sys/types.h>
# include <unistd.h>

class Response
{
private:
	std::string	header;
	std::string	body;
	std::string	full_response;
public:
	Response(std::string const &path);
	~Response();
	const char *c_str() const;
	size_t		size() const;

private:
	int			check_path(std::string const &path) const;
	bool		check_read_perm(std::string const &path) const;
	std::string	create_auto_index_page(std::string const &location);
	size_t		read_file(std::string const &location);

	void		set_header();
	void		set_full_response();
};

#endif