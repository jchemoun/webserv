/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/30 14:02:37 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/12 13:49:56 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

/*
** ============================= Public methods ============================= **
*/

Response::Response(Config::Server const &serv, Request const &req):
	header(), body(), full_response(),
	code(0),
	_serv(serv), _req(req)
{
	_autoindex = serv.autoindex;
	std::string	full_location = file::join(serv.root, _req.get_location()); // warning : need to adjust in case of redirection
	if (_req.is_invalid()) {
		code = 400;
		read_error_page();
	}
	else if (methods.find(req.get_method()) != methods.end())
		(this->*methods.at(req.get_method()))(full_location);
	else
	{
		code = 405;
		read_error_page();
	}
	set_header(full_location);
	set_full_response();
}

const char *Response::c_str() const { return (full_response.c_str()); }

size_t		Response::size()  const { return (full_response.size());  }

/*
** ============================ Private Methods ============================= **
*/


size_t	Response::create_auto_index_page(std::string &location)
{
	std::ostringstream			oss;
	std::ostringstream			oss_file;
	std::vector<std::string>	ff_vector;
	DIR							*dir;
	struct dirent				*ent;

	if (*(location.rbegin()) != '/')
	{
		code = 301;
		location += '/';
		return (read_error_page());
	}
	// probably error need check if exist && exec perm
	if (file::has_read_perm(location) == false)
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
		if (ent->d_name[0] != '.') // need check hidden files
			ff_vector.push_back(ent->d_name);
	std::sort(ff_vector.begin(), ff_vector.end());
	for (std::vector<std::string>::const_iterator cit = ff_vector.begin(); cit != ff_vector.end(); cit++)
	{
		if (file::get_type(location + *(cit)) == file::FT_DIR)
			oss << "<a href=\"" << *(cit) << "/\">" << *(cit) << "/</a>\t" << file::time_last_change(location + *(cit)) << "\t-\n";
		else
			oss_file << "<a href=\"" << *(cit) << "\">" << *(cit) << "</a>\t" << file::time_last_change(location + *(cit)) << '\t' << file::size(location + *(cit)) << '\n';
	}
	oss << oss_file.str() << "</pre><hr></body>\n</html>" << std::endl;
	code = 200;
	body = oss.str();
	header_map["Content-Type"] = "text/html";
	closedir(dir);
	return (body.length());
}

size_t	Response::read_file(std::string &location)
{
	std::ofstream		file;
	std::stringstream	buf;
	file::e_type			ft = file::get_type(location);

	if (ft == file::FT_DIR)
	{
		for (size_t i = 0; i < _serv.index.size(); ++i)
		{
			std::string	index_candidate = location + '/' + _serv.index.at(i);
			std::cout << "INDEX CANDIDATE" << index_candidate << '\n';
			if (file::get_type(index_candidate) == file::FT_FILE)
				return (read_file(index_candidate));
		}
		if (_autoindex)
			return (create_auto_index_page(location));
		else
		{
			std::cout << "WTF IS THIS\n";
			code = 403;
			return (read_error_page());
		}
		//body = some_error_page; // probably a 403 because autoindex off mean it's forbidden

	}
	else if (file::get_type(location) == file::FT_FILE)
	{
		// read file
		//std::cout << "123\n";
		if (file::has_read_perm(location) == false)
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
	std::map<int, std::string>::const_iterator	error_page_it = _serv.error_pages.find(code);
	if (error_page_it == _serv.error_pages.end())
		body = build_error_page();
	else {
		location = error_page_it->second;
		std::cout << location << '\n';
		if (file::get_type(location) == file::FT_DIR)
		{
			body = build_error_page();
			return (body.length());
		}
		if (file::has_read_perm(location) == false)
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
	}
	return (body.length()); // not used
}

void		Response::getMethod(std::string &full_location)
{
	read_file(full_location);
	// cgi
}

void		Response::postMethod(std::string &full_location)
{
	read_file(full_location);
	//cgi;
}

void		Response::deleteMethod(std::string &full_location)
{
	//(void)full_location;
	if (file::get_type(full_location) == file::FT_UNKOWN)
	{
		code = 404;
		read_error_page();
	}
	else if (file::has_write_perm(full_location) == false)
	{
		code = 403;
		read_error_page();
	}
	else if (remove(full_location.c_str()) != -1)
	{
		code = 204; // or 200 and return something;
		//body = "";
	}
	else
	{
		std::cerr << "error delete\n";
		code = 500;
		read_error_page();
	}
}

std::string	Response::build_error_page()
{
	std::ostringstream	oss;

	oss << "<html>\n<head><title>";
	oss << code << ' ' << status_header.at(code);
	oss << "</title></head>\n<body>\n<center><h1>";
	oss << code << ' ' << status_header.at(code);
	oss << "</h1></center>\n<hr><center>";
	oss << "webserv/0.1"; // to replace with actual serv name
	oss << "</center>\n</body>\n</html>";
	oss << std::endl;

	header_map["Content-Type"] = "text/html";
	return (oss.str());
}

Response::StatusMap		Response::init_status_header()
{
	StatusMap	status;
	status[100] = "Continue";
	status[200] = "OK";
	status[201] = "Created";
	status[204] = "No Content";
	status[301] = "Moved Permanently";
	status[308] = "Permanent Redirect";
	status[400] = "Bad Request";
	status[403] = "Forbidden";
	status[404] = "Not Found";
	status[405] = "Method Not Allowed";
	status[413] = "Payload Too Large";
	status[500] = "Internal Server Error";
	return (status);
}

Response::MethodMap		Response::init_method_map()
{
	MethodMap	methods;

	methods["GET"]    = &Response::getMethod;
	methods["POST"]   = &Response::postMethod;
	methods["DELETE"] = &Response::deleteMethod;
	return (methods);
}

const Response::StatusMap	Response::status_header = Response::init_status_header();
const Response::MethodMap	Response::methods       = Response::init_method_map();

std::string	Response::get_content_type(std::string const &location) const {
	std::string	extension = file::get_extension(location);
	std::string	mime_type = _serv.default_type;
	if (!extension.empty()) {
		HeaderMap::const_iterator	it = _serv.mime_types->find(extension);
		if (it != header_map.end())
			mime_type = it->second;
	}
	return (mime_type);
}

void	Response::set_header_map(std::string const &location) {
	header_map["Server"]         = "wevserv/0.1 (ubuntu)";
	header_map["Content-Length"] = utils::to_str(body.size());
	header_map["Connection"]     = "keep-alive";
	if (header_map.find("Content-Type") == header_map.end())
		header_map["Content-Type"]   = get_content_type(location);
	if (code == 301)
		header_map["location"] = location.substr(_serv.root.length()); // todo fill host + location
}

void		Response::set_header(std::string &location)
{
	set_header_map(location);

	std::ostringstream oss;

	oss << "HTTP/1.1 " << code << " " << status_header.at(code) << "\r\n";

	for (HeaderMap::const_iterator it = header_map.begin(); it != header_map.end(); ++it)
		oss << it->first << ": " << it->second << "\r\n";

	oss << "\r\n";

	header = oss.str();
}

void		Response::set_full_response()
{
	full_response.append(header);
	full_response.append(body);
}

Response::~Response() {
}
