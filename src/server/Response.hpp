/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/30 14:02:03 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/03 06:56:13 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

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
	Response(/* args */);

	int			check_path(std::string path);
	bool		check_read_perm(std::string path);
	std::string	create_auto_index_page(std::string location);
	size_t		read_file(std::string location);

	void		set_header();
	void		set_full_response();

	std::string	get_full_response() const;

	~Response();
};

#endif