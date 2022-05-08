/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 13:16:09 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/08 22:49:44 by mjacq            ###   ########.fr       */
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
	// std::stringstream					&stream;
	std::string							_raw_str;
	std::string							method;
	std::string							location;
	std::string							protocol;
	std::map<std::string, std::string>	header;
	bool								_is_complete; // when \n\n
	size_t								_index;
	std::string							_tmp_key;
	// std::string							error;
public:
	Request();

	void	parse_request();
	void	append_unparsed_request(char *buffer, ssize_t len);
	bool	is_complete();

	// Parsing utils
	void	_eat(const char *s);
	void	_eat_word(std::string &s);
	void	_eat_till_eol(std::string &s);
	void	_eat_key();
	void	_eat_value();

	std::string const	&get_location() const;

	~Request();
};

#endif
