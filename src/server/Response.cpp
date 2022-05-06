/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/30 14:02:37 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/06 16:34:25 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(Config::Server const &serv, Request const &req)
	: header(), body(), full_response(), _serv(serv)//, _req(req)
	{
	_autoindex = serv.autoindex; // location autoindex?
	content_type = "text/plain";
	std::string	full_location = serv.root + (*(serv.root.rbegin()) == '/' ? "/" : "") + req.get_location(); // warning : need to adjust in case of redirection
	init_status_header();
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

std::string	Response::time_last_modif(std::string file)
{
	struct stat	s;
	struct tm	*time;
	char		buf[200];

	stat(file.c_str(), &s);
	//std::cout << s.st_mtim.tv_nsec;
	time = localtime(&(s.st_mtim.tv_sec));
	strftime(buf, sizeof(buf), "%d-%b-%Y %H:%M", time);
	std::cout << buf;
	return (buf);
}

long	Response::size_file(std::string file)
{
	struct stat	s;

	return (stat(file.c_str(), &s));
}

size_t	Response::create_auto_index_page(std::string &location)
{
	std::ostringstream	oss;
	DIR					*dir;
	struct dirent		*ent;

	if (*(location.rbegin()) != '/')
	{
		code = 301;
		location += '/';
		return (read_error_page());
	}
	if (check_read_perm(location) == false)
	{
		code = 403;
		return (read_error_page());
	}
	if ((dir = opendir(location.c_str())) == NULL)
	{
		code = 404;
		return (read_error_page());
	}
	oss << "<html>\n<head><title>Index of " << location.substr(_serv.root.length()) << "</title></head>\n<body>\n";
	oss << "<h1>Index of " << location.substr(_serv.root.length()) << "</h1><hr><pre><a href=\"../\">../</a>\n";
	// list of file, last modif, size
	while ((ent = readdir(dir)) != NULL)
	{
		if (ent->d_name[0] != '.')
		{
			oss << "<a href=\"" << ent->d_name << (check_path(location + ent->d_name) == FT_DIR ? "/" : ""); // need add / if folder
			oss << "\">" << ent->d_name << (check_path(location + ent->d_name) == FT_DIR ? "/" : ""); // need add / if folder
			oss << "</a>\t" << time_last_modif(location + ent->d_name) << '\t';
			if (check_path(location + ent->d_name) != FT_DIR)
				oss << size_file(location + ent->d_name);
			else
				oss << '-';
			oss << '\n';
		}
	}
	oss << "</pre><hr></body>\n</html>" << std::endl;
	code = 200;
	body = oss.str();
	content_type = "text/html";
	closedir(dir);
	return (body.length());
}

size_t	Response::read_file(std::string &location)
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
			return (create_auto_index_page(location));
		else
		{
			code = 403;
			return (read_error_page());
		}
		  //body = some_error_page; // probably a 403 because autoindex off mean it's forbidden

	}
	else if (check_path(location) == FT_FILE)
	{
		// read file
		//std::cout << "123\n";
		if (check_read_perm(location) == false)
		{
			//std::cout << "403\n";
			//403
			code = 403;
			return (read_error_page());
		}
		file.open(location.c_str(), std::ifstream::in);
		if (file.is_open() == false)
		{
			// 404
			code = 404;
			return (read_error_page());
		}
		buf << file.rdbuf();
		file.close();
		body = buf.str();
		//if (body.find("<!DOCTYPE html>") != std::string::npos) // to replace with parse nginx file && extension file
		//	content_type = "text/html";
	}
	else
	{
		//std::cout << "404\n";
		// 404
		code = 404;
		return (read_error_page());
	}
	std::cout << "body: \e[33m" << body << "\e[0m";
	code = 200;
	return (body.length()); // not used
}

// read file for error pages
size_t		Response::read_error_page()
{
	std::ofstream		file;
	std::stringstream	buf;
	std::string			location;

	std::cout << "in error\n" << code << "\n";
	//for (std::map<int, std::string>::const_iterator cit = _serv.error_pages.begin(); cit != _serv.error_pages.end(); cit++)
	//{
	//	std::cout << "FGH" << (*cit).second << '\n';
	//}
	location = (*(_serv.error_pages.find(code))).second;
	std::cout << location << '\n';
	if (check_path(location) == FT_DIR)
	{
		body = build_error_page();
		return (body.length());
	}
	if (check_read_perm(location) == false)
	{
		// error but not supposed to happen;
		body = build_error_page();
		return (body.length());
	}
	file.open(location.c_str(), std::ifstream::in);
	if (file.is_open() == false)
	{
		// error but not supposed to happen;
		body = build_error_page();
		return (body.length());
	}
	buf << file.rdbuf();
	file.close();
	body = buf.str();
	std::cout << "body: \e[33m" << body << "\e[0m";
	return (body.length()); // not used
}

std::string	Response::build_error_page()
{
	std::ostringstream	oss;

	oss << "<html>\n<head><title>";
	oss << code << ' ' << status_header[code];
	oss << "</title></head>\n<body>\n<center><h1>";
	oss << code << ' ' << status_header[code];
	oss << "</h1></center>\n<hr><center>";
	oss << "webserv/0.1"; // to replace with actual serv name
	oss << "</center>\n</body>\n</html>";
	oss << std::endl;

	content_type = "text/html";
	return (oss.str());
}

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
	if (code == 301)
	{
		oss << "location: "; // todo fill host + location
	}
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
