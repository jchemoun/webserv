/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchemoun <jchemoun@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 13:16:09 by jchemoun          #+#    #+#             */
/*   Updated: 2022/04/30 15:04:08 by jchemoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <sys/types.h>
# include <iostream>

class Request
{
private:
	std::string	unparsed_request;
	std::string	method;
	std::string	location;
public:
	Request(/* args */);

	bool	parse_request();
	void	append_unparsed_request(char *buffer, ssize_t len);

	std::string	get_location();

	~Request();
};

#endif