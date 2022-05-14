/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/30 14:02:37 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/13 14:53:00 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

/*
** ============================= Public methods ============================= **
*/

Response::Response(Config::Server const &serv, Request const &req):
	header(), body(), full_response(),
	code(0),
	_autoindex(serv.autoindex),
	location(req.get_location()),
	full_location(file::join(serv.root, location)),
	_serv(serv), _req(req),
	is_large_file(false), size_file(),file()
{
	if (_req.is_invalid()) {
		code = 400;
		read_error_page();
	}
	else if (methods.find(req.get_method()) != methods.end())
		(this->*methods.at(req.get_method()))();
	else
	{
		code = 405;
		read_error_page();
	}
	set_header();
	set_full_response();
}

const char *Response::c_str() const { return (full_response.c_str()); }

size_t		Response::size()  const { return (full_response.size());  }

/*
** ============================ Private Methods ============================= **
*/


size_t	Response::create_auto_index_page()
{
	std::ostringstream			oss;
	std::ostringstream			oss_file;
	std::vector<std::string>	ff_vector;
	DIR							*dir;
	struct dirent				*ent;

	if (*(full_location.rbegin()) != '/')
	{
		code = 301;
		location += '/';
		full_location += '/';
		return (read_error_page());
	}
	// probably error need check if exist && exec perm
	if (file::has_read_perm(full_location) == false)
	{
		code = 403;
		return (read_error_page());
	}
	if ((dir = opendir(full_location.c_str())) == NULL)
	{
		code = 404;
		return (read_error_page());
	}
	oss << "<html>\n<head><title>Index of " << location << "</title></head>\n<body>\n";
	oss << "<h1>Index of " << location << "</h1><hr><pre><a href=\"../\">../</a>\n";
	// list of file, last modif, size
	while ((ent = readdir(dir)) != NULL)
		if (ent->d_name[0] != '.') // need check hidden files
			ff_vector.push_back(ent->d_name);
	std::sort(ff_vector.begin(), ff_vector.end());
	for (std::vector<std::string>::const_iterator cit = ff_vector.begin(); cit != ff_vector.end(); cit++)
	{
		if (file::get_type(full_location + *(cit)) == file::FT_DIR)
			oss << "<a href=\"" << *(cit) << "/\">" << *(cit) << "/</a>\t" << file::time_last_change(full_location + *(cit)) << "\t-\n";
		else
			oss_file << "<a href=\"" << *(cit) << "\">" << *(cit) << "</a>\t" << file::time_last_change(full_location + *(cit)) << '\t' << file::size(full_location + *(cit)) << '\n';
	}
	oss << oss_file.str() << "</pre><hr></body>\n</html>" << std::endl;
	code = 200;
	body = oss.str();
	header_map["Content-Type"] = "text/html";
	closedir(dir);
	return (body.length());
}

size_t	Response::read_file()
{
	//std::ofstream		file;
	std::stringstream	buf;
	file::e_type		ft = file::get_type(full_location);

	if (ft == file::FT_DIR)
	{
		for (size_t i = 0; i < _serv.index.size(); ++i)
		{
			std::string	const &index_candidate =_serv.index.at(i);
			std::cout << "INDEX CANDIDATE: " << index_candidate << '\n';
			if (file::get_type(file::join(full_location, index_candidate)) == file::FT_FILE) {
				location = index_candidate;
				full_location = file::join(full_location, index_candidate);
				return (read_file());
			}
		}
		if (_autoindex)
			return (create_auto_index_page());
		else
		{
			std::cout << "WTF IS THIS\n";
			code = 403;
			return (read_error_page());
		}
		//body = some_error_page; // probably a 403 because autoindex off mean it's forbidden
	}
	else if (file::get_type(full_location) == file::FT_FILE)
	{
		// read file
		//std::cout << "123\n";
		if (file::has_read_perm(full_location) == false)
		{
			//std::cout << "403\n";
			//403
			code = 403;
			return (read_error_page());
		}
		file.open(full_location.c_str(), std::ifstream::in);
		if (file.is_open() == false)
		{
			// 404
			code = 404;
			return (read_error_page());
		}
		if ((size_file = file::size(location)) + 200 >= BUFFER_SIZE)
		{
			is_large_file = true;
			code = 200;
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
	std::string			error_page_path;

	std::cout << "in error\n" << code << "\n";
	//for (std::map<int, std::string>::const_iterator cit = _serv.error_pages.begin(); cit != _serv.error_pages.end(); cit++)
	//{
	//	std::cout << "FGH" << (*cit).second << '\n';
	//}
	Config::ErrPageMap::const_iterator	error_page_it = _serv.error_pages.find(code);
	if (error_page_it == _serv.error_pages.end())
		body = build_error_page();
	else {
		error_page_path = error_page_it->second;
		std::cout << error_page_path << '\n';
		if (file::get_type(error_page_path) == file::FT_DIR)
		{
			body = build_error_page();
			return (body.length());
		}
		if (file::has_read_perm(error_page_path) == false)
		{
			// error but not supposed to happen;
			body = build_error_page();
			return (body.length());
		}
		file.open(error_page_path.c_str(), std::ifstream::in);
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

void		Response::getMethod()
{
	read_file();
	// cgi
}

void		Response::postMethod()
{
	read_file();
	//cgi;
}

void		Response::deleteMethod()
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

void	Response::set_header_map()
{
	header_map["Server"]         = "wevserv/0.1 (ubuntu)";
	header_map["Content-Length"] = (is_large_file ? utils::to_str(size_file) : utils::to_str(body.size()));//utils::to_str(is_large_file ? file::size(location) : body.size());
	header_map["Connection"]     = "keep-alive";

	if (header_map.find("Content-Type") == header_map.end())
		header_map["Content-Type"] = file::get_mime(location, *_serv.mime_types, _serv.default_type);

	if (code == 301)
		header_map["location"] = location.substr(_serv.root.length()); // todo fill host + location
}

void		Response::set_header()
{
	set_header_map();

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
