/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/30 14:02:03 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/16 10:19:45 by mjacq            ###   ########.fr       */
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
# include "utils.hpp"
# include "file.hpp"
# include "Webserv.hpp"

class Response
{
public:
	typedef std::map<int, std::string>					StatusMap;
	typedef std::map<std::string, void (Response::*)()>	MethodMap;
	typedef std::map<std::string, std::string>			HeaderMap;

private:
	HeaderMap				_header_map;
	std::string				_header;
	std::string				_body;
	std::string				_full_response;
	int						_code;
	bool					_autoindex;
	std::string				_location;
	std::string				_full_location;
	Config::Server const	&_serv;

	static const StatusMap	_status_header;
	static const MethodMap	_methods;
	Request const			&_req;

public:
	Response(Config::Server const &serv, Request const &req);
	~Response();

	bool			is_large_file;
	long			size_file;
	std::ifstream	file;

	const char		*c_str() const;
	size_t			size() const;

private:
	static StatusMap	_init_status_header();
	static MethodMap	_init_method_map();

	size_t		_create_auto_index_page();
	size_t		_read_file();
	size_t		_read_error_page();

	void		_getMethod();
	void		_postMethod();
	void		_deleteMethod();

	std::string	_build_error_page();

	void		_set_header_map();
	void		_set_header();
	void		_set_full_response();
};

#endif
