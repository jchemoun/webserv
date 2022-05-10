/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 13:16:09 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/11 14:26:03 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <sys/types.h>
# include <iostream>
# include <map>
# include <sstream>

class Request
{
public:
	typedef std::map<std::string, std::string>	Header;
private:
	// parsed elements
	std::string		method;
	std::string		location;
	std::string		protocol;
	std::string		body;
	Header			header;
	// internal attributes
	std::string		_raw_str;               // unparsed request
	size_t			_max_body_size;
	size_t			_content_length;
	bool			_complete_request_line;
	bool			_complete_header;
	bool			_complete_body;
	bool			_invalid_request;
	size_t			_index;
	std::string		_tmp_key;
	// std::string							error;
public:
	Request();
	~Request();

	// Getters
	std::string const	&get_method() const;
	std::string const	&get_location() const;
	std::string const	&get_protocol() const;
	std::string const	&get_body() const;
	Header const		&get_header() const;

	void				parse_request();

	// Public Utils
	void				append_unparsed_request(char *buffer, ssize_t len);
	bool				is_complete() const;
	bool				is_invalid() const;
	void				reset();

private:
	void	_parse_request_line();
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
