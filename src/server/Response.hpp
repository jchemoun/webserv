/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/30 14:02:03 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/12 18:37:45 by mjacq            ###   ########.fr       */
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

class Response
{
public:
	typedef std::map<int, std::string>					StatusMap;
	typedef std::map<std::string, void (Response::*)()>	MethodMap;
	typedef std::map<std::string, std::string>			HeaderMap;
private:
	HeaderMap				header_map;
	std::string				header;
	std::string				body;
	std::string				full_response;
	int						code;
	bool					_autoindex;
	std::string				location;
	std::string				full_location;
	Config::Server const	&_serv;

	static const StatusMap	status_header;
	static const MethodMap	methods;
	Request const			&_req;
public:
	Response(Config::Server const &serv, Request const &req);
	~Response();
	const char *c_str() const;
	size_t		size() const;

private:
	static StatusMap	init_status_header();
	static MethodMap	init_method_map();

	size_t		create_auto_index_page();
	size_t		read_file();
	size_t		read_error_page();

	void		getMethod();
	void		postMethod();
	void		deleteMethod();

	std::string	build_error_page();

	void		set_header_map();
	void		set_header();
	void		set_full_response();
};

#endif
