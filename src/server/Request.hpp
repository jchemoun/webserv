/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 13:16:09 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/16 14:51:38 by mjacq            ###   ########.fr       */
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

class Request
{
public:
	typedef std::map<std::string, std::string>	Header;

private:
	std::string		_method;
	std::string		_request_uri;
	std::string		_protocol;
	std::string		_body;
	Header			_header;
	int				_status_code;
	// decompose request_uri in uri and query_string
	std::string		_uri;
	std::string		_query_string;

	// internal attributes
	std::string		_raw_str;               // unparsed request
	size_t			_max_body_size;
	size_t			_content_length;
	bool			_complete_request_line;
	bool			_complete_header;
	bool			_complete_body;
	size_t			_index;
	std::string		_tmp_key;

public:
	Request();
	~Request();

	// Getters
	std::string const	&get_method() const;
	std::string const	&get_request_uri() const;
	std::string const	&get_protocol() const;
	std::string const	&get_body() const;
	Header const		&get_header() const;
	int					get_status_code() const;

	std::string const	&get_uri() const;
	std::string const	&get_query_string() const;

	void				parse_request();

	// Public Utils
	void				append_unparsed_request(char *buffer, ssize_t len);
	bool				is_complete() const;
	bool				is_invalid() const;
	void				reset();

private:
	void	_parse_request_line();
	void	_parse_request_uri();
	void	_parse_protocol();
	void	_parse_header();
	void	_parse_body();
	void	_parse_content_length(std::string const &value);
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
};

#endif
