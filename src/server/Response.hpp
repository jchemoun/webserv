/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/30 14:02:03 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/11 14:59:55 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <cstddef>
# include <string>
# include <iostream>
# include <fstream>
# include <sstream>
# include <algorithm>
# include <sys/stat.h>
# include <sys/types.h>
# include <unistd.h>
# include <dirent.h>
# include "Config.hpp"
# include "Request.hpp"

class Response
{
public:
	typedef std::map<int, std::string>						StatusMap;
	typedef std::map<std::string, void (Response::*)(std::string&)>	MethodMap;
private:
	std::string				header;
	std::string				body;
	std::string				full_response;
	std::string				content_type;
	int						code;
	Config::Server const	&_serv;
	bool					_autoindex;

	static const StatusMap	status_header;
	static const MethodMap	methods;
	Request const			&_req;
public:
	Response(Config::Server const &serv, Request const &req);
	~Response();
	const char *c_str() const;
	size_t		size() const;

private:
	enum	e_filetype { FT_UNKOWN, FT_DIR, FT_FILE };

	static StatusMap	init_status_header();
	static MethodMap	init_method_map();

	e_filetype	check_path(std::string const &path) const;
	bool		check_read_perm(std::string const &path) const;
	bool		check_write_perm(std::string const &path) const;
	std::string	time_last_change(std::string file);
	long		size_file(std::string file);

	size_t		create_auto_index_page(std::string &location);
	size_t		read_file(std::string &location);
	size_t		read_error_page();

	void		getMethod(std::string &full_location);
	void		postMethod(std::string &full_location);
	void		deleteMethod(std::string &full_location);

	std::string	build_error_page();

	void		set_header(std::string &location);
	void		set_full_response();
};

#endif
