/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/30 14:02:37 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/03 08:42:59 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <cstddef>
#include <sstream>
#include <string>

Response::Response(Config::Server const &serv, Request const &req)
	: header(), body(), full_response(), _serv(serv), _req(req) {
	_autoindex = true; // TODO: use parsing to check autoindex
	std::string	full_location = serv.root + '/' + req.get_location();
	read_file(full_location);
	set_header();
	set_full_response();
}

const char *Response::c_str() const {
	return (full_response.c_str());
}

size_t	Response::size() const {
	return (full_response.size());
}

Response::e_filetype	Response::check_path(std::string const &path) const {
	struct stat	s;

	if (stat(path.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFDIR)
			return (FT_DIR);
		else if (s.st_mode & S_IFREG)
			return (FT_FILE);
		else
			return (FT_UNKOWN);
	}
	return (FT_UNKOWN);
}

bool	Response::check_read_perm(std::string const &path) const {
	struct stat	s;

	if (stat(path.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IROTH)
			return (true);
	}
	return (false);
}

std::string	Response::create_auto_index_page(std::string const &location)
{
	(void)location;
	return ("auto_index not implemented yet\n");
}

size_t	Response::read_file(std::string const &location)
{
	std::ofstream		file;
	std::stringstream	buf;
	e_filetype			ft = check_path(location);

	if (ft == FT_DIR) {
		for (size_t i = 0; i < _serv.index.size(); ++i) {
			std::string	index_candidate = location + '/' + _serv.index.at(i);
			if (check_path(index_candidate) == FT_FILE)
				return read_file(index_candidate);
		}
		 if (_autoindex)
			body = create_auto_index_page(location);
		// else
		//  	body = some_error_page;

	}
	else if (check_path(location) == FT_FILE)
	{
		// read file
		//std::cout << "123\n";
		if (check_read_perm(location) == false)
		{
			//std::cout << "403\n";
			//403
			return (0);
		}
		file.open(location.c_str(), std::ifstream::in);
		if (file.is_open() == false)
		{
			// 404
			return (0);
		}
		buf << file.rdbuf();
		file.close();
		body = buf.str();
	}
	else
	{
		//std::cout << "404\n";
		// 404
		return (0);
	}
	std::cout << "body: \e[33m" << body << "\e[0m";
	return (body.length()); // not used
}

// read file for error pages

void		Response::set_header() {
	// TODO: set header according to the response
	std::ostringstream oss;
	oss << "HTTP/1.1 200 OK" << std::endl;
	oss << "Server: webserv/0.1 (Ubuntu)" << std::endl;
	oss << "Content-Length: " << body.size() << std::endl;
	oss << "Content-Type: text/plain" << std::endl;
	oss << "Connection: keep-alive" << std::endl;
	oss << std::endl;
	header = oss.str();
}

void		Response::set_full_response()
{
	full_response.append(header);
	full_response.append(body);
	// full_response.append("\n");
}

Response::~Response() {
}