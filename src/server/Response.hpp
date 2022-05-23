/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/30 14:02:03 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/23 14:07:07 by user42           ###   ########.fr       */
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
# include <dirent.h>

# include "Config.hpp"
# include "Request.hpp"
# include "utils.hpp"
# include "file.hpp"
# include "Include.hpp"
# include "Cgi.hpp"
# include "http_response_codes.hpp"

class Response
{
public:
	typedef std::map<std::string, void (Response::*)()>	MethodMap;
	typedef std::map<std::string, std::string>			HeaderMap;
	struct Uri {
		std::string						path;
		std::string						full_path;
		std::string const				*root;
		std::vector<std::string> const	*indexes;
		Config::ErrPageMap const		*error_pages;
		Uri(const std::string &path, Config::Server const &serv);
		void resolve(Config::Server const &serv);
	private:
		Uri();
	};

private:
	HeaderMap					_header_map;
	std::string					_header;
	std::string					_body;
	std::string					_full_response;
	http::code					_code;
	bool						_autoindex;
	std::string const			_request_uri;
	std::string					_query_string;
	Config::Server const		&_serv;
	Uri							_uri;
	Config::Connection const	&_client_info;
	std::string					_cgi_status;

	static const MethodMap		_methods;
	Request const				&_req;

public:
	Response(Request const &req, Config::Connection const &client_info);
	~Response();

	int			is_large_file;
	long			size_file;
	std::ifstream	file;

	const char		*c_str() const;
	size_t			size() const;

private:
	static MethodMap	_init_method_map();

	void		_create_auto_index_page();
	void		_read_file();
	void		_read_error_page(http::code	error_code);

	void		_getMethod();
	void		_postMethod();
	void		_deleteMethod();
	void		_putMethod();

	void		_run_cgi();

	void		_build_error_page();

	void		_set_header_map();
	void		_set_header();
	void		_set_full_response();
	bool		_is_a_cgi() const;
};

#endif
