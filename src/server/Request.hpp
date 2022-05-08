/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 13:16:09 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/08 18:39:40 by mjacq            ###   ########.fr       */
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
	std::string							unparsed_request;
	std::string							method;
	std::string							location;
	std::string							protocol;
	std::map<std::string, std::string>	header;
	bool								_is_complete; // when \n\n
	// std::string							error;
public:
	Request();

	void	parse_request();
	void	append_unparsed_request(char *buffer, ssize_t len);
	bool	is_complete();

	std::string const	&get_location() const;

	~Request();
};

#endif
