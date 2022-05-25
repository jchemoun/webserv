/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 13:16:09 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/25 16:10:39 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <sys/types.h>
# include <iostream>
# include <map>
# include <sstream>
# include "http_response_codes.hpp"
# include "Config.hpp"

class Request
{
public:
	typedef std::map<std::string, Config::Server *>	NameToServMap;    // key: server_name
	typedef std::map<int, NameToServMap>			ServerMap;        // first key: listen_fd, second: server_name
	typedef std::map<int, Config::Server *>			DefaultServerMap; // key: listen_fd
	typedef std::map<std::string, std::string>		Header;
	friend class Cgi;

private:
	std::string		_method;
	std::string		_request_uri;
	std::string		_protocol;
	std::string		_body;
	Header			_header;
	http::code		_status_code;
	// decompose request_uri in uri and query_string
	std::string		_uri;
	std::string		_query_string;

	// internal attributes
	std::string		_raw_str;               // unparsed request
	size_t			_content_length;
	bool			_complete_request_line;
	bool			_complete_header;
	bool			_complete_body;
	size_t			_index;
	std::string		_tmp_key;
	bool			_chunk_encoding;
	size_t			_chunk_size;

public:
	Request();
	~Request();

	Config::Connection const	*listen_info;
	Config::Server const		*current_server;
	std::string					current_server_name;

	// Getters
	std::string const	&get_method() const;
	std::string const	&get_request_uri() const;
	std::string const	&get_protocol() const;
	std::string const	&get_body() const;
	Header const		&get_header() const;
	http::code			get_status_code() const;

	std::string const	&get_uri() const;
	std::string const	&get_query_string() const;

	void				parse_request(ServerMap &serverMap, DefaultServerMap &def_server_map);
	void				parse_cgi(std::string &output);

	// Public Utils
	void				append_unparsed_request(char *buffer, ssize_t len);
	bool				is_complete() const;
	bool				is_invalid() const;
	void				reset();

private:
	void	_resolve_server(ServerMap &serverMap, DefaultServerMap &def_server_map);
	void	_parse_request_line();
	void	_parse_request_uri();
	void	_parse_protocol();
	void	_parse_header();
	void	_parse_body();
	void	_parse_chunks();
	void	_parse_content_length(std::string const &value);
	void	_check_headers();
	// Parsing utils
	void	_eat(const char *s);
	void	_eat_word(std::string &s);
	void	_eat_word(std::string &s, const char *delimiter, bool allow_empty = false);
	void	_eat_eol();
	void	_eat_spaces();
	void	_eat_key();
	void	_eat_value();
	bool	_skip_empty_line();
	void	_skip_empty_lines();
	void	_append_to_body(size_t &size);
};

#endif
