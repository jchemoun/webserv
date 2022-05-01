/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchemoun <jchemoun@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 13:17:12 by jchemoun          #+#    #+#             */
/*   Updated: 2022/04/30 15:04:14 by jchemoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(/* args */): unparsed_request()
{
}

bool	Request::parse_request()
{
	size_t	start;
	size_t	end;

	start = 0;
	end = unparsed_request.find_first_of(' ', 0);
	method = unparsed_request.substr(0, end);
	std::cout << unparsed_request << '\n';
	//if (method != "GET" || method != "POST" || method != "DELETE")
	//	return (false);
	std::cout << method.size() << ' ' << end << method << '\n';
	start = ++end;
	end = unparsed_request.find_first_of(' ', start);
	location = unparsed_request.substr(start, end - start);
	std::cout << location.size() << location << '\n';
	unparsed_request.clear();
	return (true);
}

void	Request::append_unparsed_request(char *buffer, ssize_t len)
{
	unparsed_request.append(buffer, len);
}

std::string	Request::get_location()
{
	return (location);
}

Request::~Request()
{
}
