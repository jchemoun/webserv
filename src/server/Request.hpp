/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 13:16:09 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/09 11:52:13 by mjacq            ###   ########.fr       */
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
private:
	std::string							_raw_str;     // unparsed request
	std::string							method;
	std::string							location;
	std::string							protocol;
	std::map<std::string, std::string>	header;
	bool								_is_complete; // when empty line is reached
	size_t								_index;       // used in parsing
	std::string							_tmp_key;     // used in parsing
	// std::string							error;
public:
	Request();
	~Request();

	void	parse_request();
	void	append_unparsed_request(char *buffer, ssize_t len);
	std::string const	&get_location() const;
	bool	is_complete() const;

private:
	// Parsing utils
	void	_eat(const char *s);
	void	_eat_word(std::string &s);
	void	_eat_word(std::string &s, const char *delimiter);
	void	_eat_eol();
	void	_eat_spaces();
	void	_eat_key();
	void	_eat_value();
	void	_skip_empty_lines();
};

#endif
