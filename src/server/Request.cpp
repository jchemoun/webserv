/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 13:17:12 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/08 18:40:13 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(): _is_complete(false)
{
}

void	Request::parse_request()
{
	std::stringstream	stream;
	std::stringstream	header_stream;
	std::string			line;
	std::string			key;
	std::string			value;

	if (unparsed_request.find("\r\n\r\n") != std::string::npos || unparsed_request.find("\n\n") != std::string::npos) {
		std::cout << "\e[32mComplete request\e[0m\n";
		_is_complete = true;
	}
	else {
		std::cout << "\e[31mIncomplete request\e[0m\n";
		return ;
	}

	stream << unparsed_request;				std::cout << "\e[32m" << unparsed_request << "\e[0m✋\n";
	stream.seekg(0);						//std::cout << "\e[32m" << stream.str() << "\e[0m✋" << std::endl;
	std::getline(stream, method, ' ');		std::cout << "method: " << method << "\n";
	std::getline(stream, location, ' ');	std::cout << "location: " << location << "\n";
	std::getline(stream, protocol);			std::cout << "protocol: " << protocol << "\n";
	while (stream.good()) {
		header_stream.clear();
		std::getline(stream, line);
		if (header_stream.good() && !line.empty() && line != "\r") {
			header_stream << line;
			header_stream.seekg(0);
			if (header_stream.good()) {
				std::getline(header_stream, key, ':');
				std::getline(header_stream, value);
				header[key] = value;
				std::cout << "\e[36m" << key << ":" << header[key] << "\e[0m\n";
			}
		}
	}
	// if (false) {
	// 	size_t	start;
	// 	size_t	end;
	//
	// 	start = 0;
	// 	end = unparsed_request.find_first_of(' ', 0);
	// 	method = unparsed_request.substr(0, end);
	// 	std::cout << unparsed_request << '\n';
	// 	//if (method != "GET" || method != "POST" || method != "DELETE")
	// 	//	return (false);
	// 	std::cout << start << ':' << end << " size: " << method.size() << " method: " << method << '\n';
	// 	start = ++end;
	// 	end = unparsed_request.find_first_of(' ', start);
	// 	location = unparsed_request.substr(start, end - start);
	// 	// std::cout << location.size() << location << '\n';
	// 	std::cout << start << ':' << end << " size: " << location.size() << " location: " << location << '\n';
	// 	unparsed_request.clear();
	// }
}

void	Request::append_unparsed_request(char *buffer, ssize_t len)
{
	// stream.write(buffer, len);
	unparsed_request.append(buffer, len);
}

bool	Request::is_complete() { return (_is_complete); }

std::string const	&Request::get_location() const {
	return (location);
}

Request::~Request() { }
