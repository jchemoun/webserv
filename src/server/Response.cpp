/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/30 14:02:37 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/04 14:07:54 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <cstddef>
#include <sstream>
#include <string>

Response::Response(Config::Server const &serv, Request const &req)
	: header(), body(), full_response(), _serv(serv)//, _req(req)
	{
	_autoindex = true; // TODO: use parsing to check autoindex
	content_type = "text/plain";
	std::string	full_location = serv.root + '/' + req.get_location(); // warning : need to adjust in case of redirection
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
	code = 200;
	return ("auto_index not implemented yet\n");
}

size_t	Response::read_file(std::string const &location)
{
	std::ofstream		file;
	std::stringstream	buf;
	e_filetype			ft = check_path(location);

	if (ft == FT_DIR)
	{
		for (size_t i = 0; i < _serv.index.size(); ++i)
		{
			std::string	index_candidate = location + '/' + _serv.index.at(i);
			if (check_path(index_candidate) == FT_FILE)
				return (read_file(index_candidate));
		}
		if (_autoindex)
			body = create_auto_index_page(location);
		//else
		//  body = some_error_page;

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
		if (body.find("<!DOCTYPE html>") != std::string::npos)
			content_type = "text/html";
	}
	else
	{
		//std::cout << "404\n";
		// 404
		return (0);
	}
	std::cout << "body: \e[33m" << body << "\e[0m";
	code = 200;
	return (body.length()); // not used
}

// read file for error pages

void		Response::init_status_header()
{
	status_header[100] = "Continue";
	status_header[200] = "OK";
	status_header[201] = "Created";
	status_header[204] = "No Content";
	status_header[301] = "Moved Permanently";
	status_header[308] = "Permanent Redirect";
	status_header[400] = "Bad Request";
	status_header[403] = "Forbidden";
	status_header[404] = "Not Found";
	status_header[405] = "Method Not Allowed";
	status_header[413] = "Payload Too Large";
	status_header[500] = "Internal Server Error";
}

void		Response::set_header() {
	// TODO: set header according to the response
	std::ostringstream oss;
	oss << "HTTP/1.1 " << code << " " << status_header[code] << '\n';
	oss << "Server: webserv/0.1 (Ubuntu)" << std::endl; // ??? which name; all of them ? wth
	oss << "Content-Length: " << body.size() << '\n';
	oss << "Content-Type: " << content_type << '\n';
	oss << "Connection: keep-alive" << '\n';
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