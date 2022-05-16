/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/30 14:02:37 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/16 15:09:07 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "color.hpp"

/*
** ============================= Public methods ============================= **
*/

Response::Response(Config::Server const &serv, Request const &req):
	_header(), _body(), _full_response(),
	_code(req.get_status_code()),
	_autoindex(serv.autoindex),
	_request_uri(req.get_request_uri()),
	_uri(req.get_uri()),
	_query_string(req.get_query_string()),
	_full_location(file::join(serv.root, _uri)),
	_serv(serv), _req(req),
	is_large_file(false), size_file(0)
{
	if (_req.is_invalid()) {
		_read_error_page();
	}
	else if (_methods.find(req.get_method()) != _methods.end())
		(this->*_methods.at(req.get_method()))();
	else
	{
		_code = 405;
		_read_error_page();
	}
	_set_header();
	_set_full_response();
}

const char *Response::c_str() const { return (_full_response.c_str()); }

size_t		Response::size()  const { return (_full_response.size());  }

/*
** ============================ Private Methods ============================= **
*/


size_t	Response::_create_auto_index_page()
{
	std::ostringstream			oss;
	std::ostringstream			oss_file;
	std::vector<std::string>	ff_vector;
	DIR							*dir;
	struct dirent				*ent;

	if (*(_full_location.rbegin()) != '/')
	{
		_code = 301;
		_uri += '/';
		_full_location += '/';
		return (_read_error_page());
	}
	// probably error need check if exist && exec perm
	if (file::has_read_perm(_full_location) == false)
	{
		_code = 403;
		return (_read_error_page());
	}
	if ((dir = opendir(_full_location.c_str())) == NULL)
	{
		_code = 404;
		return (_read_error_page());
	}
	oss << "<html>\n<head><title>Index of " << _uri << "</title></head>\n<body>\n";
	oss << "<h1>Index of " << _uri << "</h1><hr><pre><a href=\"../\">../</a>\n";
	// list of file, last modif, size
	while ((ent = readdir(dir)) != NULL)
		if (ent->d_name[0] != '.') // need check hidden files
			ff_vector.push_back(ent->d_name);
	std::sort(ff_vector.begin(), ff_vector.end());
	for (std::vector<std::string>::const_iterator cit = ff_vector.begin(); cit != ff_vector.end(); cit++)
	{
		if (file::get_type(_full_location + *(cit)) == file::FT_DIR)
			oss << "<a href=\"" << *(cit) << "/\">" << *(cit) << "/</a>\t" << file::time_last_change(_full_location + *(cit)) << "\t-\n";
		else
			oss_file << "<a href=\"" << *(cit) << "\">" << *(cit) << "</a>\t" << file::time_last_change(_full_location + *(cit)) << '\t' << file::size(_full_location + *(cit)) << '\n';
	}
	oss << oss_file.str() << "</pre><hr></body>\n</html>" << std::endl;
	_code = 200;
	_body = oss.str();
	_header_map["Content-Type"] = "text/html";
	closedir(dir);
	return (_body.length());
}

size_t	Response::_read_file()
{
	//std::ofstream		file;
	std::stringstream	buf;
	file::e_type		ft = file::get_type(_full_location);

	if (ft == file::FT_DIR)
	{
		for (size_t i = 0; i < _serv.index.size(); ++i)
		{
			std::string	const &index_candidate =_serv.index.at(i);
			if (file::get_type(file::join(_full_location, index_candidate)) == file::FT_FILE) {
				std::cout << color::bold << "Index found: " << color::magenta << index_candidate << color::reset << '\n';
				_uri = index_candidate;
				_full_location = file::join(_full_location, index_candidate);
				return (_read_file());
			}
		}
		if (_autoindex)
			return (_create_auto_index_page());
		else
		{
			std::cout << "WTF IS THIS\n";
			_code = 403;
			return (_read_error_page());
		}
		//body = some_error_page; // probably a 403 because autoindex off mean it's forbidden
	}
	else if (file::get_type(_full_location) == file::FT_FILE)
	{
		// read file
		//std::cout << "123\n";
		if (file::has_read_perm(_full_location) == false)
		{
			//std::cout << "403\n";
			//403
			_code = 403;
			return (_read_error_page());
		}
		file.open(_full_location.c_str(), std::ifstream::in);
		if (file.is_open() == false)
		{
			// 404
			_code = 404;
			return (_read_error_page());
		}
		if ((size_file = file::size(_uri)) + 200 >= BUFFER_SIZE)
		{
			is_large_file = true;
			_code = 200;
			return (0);
		}
		buf << file.rdbuf();
		file.close();
		_body = buf.str();
	}
	else
	{
		//std::cout << "404\n";
		// 404
		_code = 404;
		return (_read_error_page());
	}
	_code = 200;
	return (_body.length()); // not used
}

// read file for error pages
size_t		Response::_read_error_page()
{
	std::ofstream		file;
	std::stringstream	buf;
	std::string			error_page_path;

	std::cout << "in error\n" << _code << "\n";
	Config::ErrPageMap::const_iterator	error_page_it = _serv.error_pages.find(_code);
	if (error_page_it == _serv.error_pages.end())
		_body = _build_error_page();
	else {
		error_page_path = error_page_it->second;
		std::cout << error_page_path << '\n';
		if (file::get_type(error_page_path) == file::FT_DIR)
		{
			_body = _build_error_page();
			return (_body.length());
		}
		if (file::has_read_perm(error_page_path) == false)
		{
			// error but not supposed to happen;
			_body = _build_error_page();
			return (_body.length());
		}
		file.open(error_page_path.c_str(), std::ifstream::in);
		if (file.is_open() == false)
		{
			// error but not supposed to happen;
			_body = _build_error_page();
			return (_body.length());
		}
		buf << file.rdbuf();
		file.close();
		_body = buf.str();
		std::cout << "body: " << color::yellow << _body << color::reset << "✋\n";
	}
	return (_body.length()); // not used
}

void		Response::_getMethod()
{
	_read_file();
	// cgi
}

void		Response::_postMethod()
{
	_read_file();
	//cgi;
}

void		Response::_deleteMethod()
{
	//(void)full_location;
	if (file::get_type(_full_location) == file::FT_UNKOWN)
	{
		_code = 404;
		_read_error_page();
	}
	else if (file::has_write_perm(_full_location) == false)
	{
		_code = 403;
		_read_error_page();
	}
	else if (remove(_full_location.c_str()) != -1)
	{
		_code = 204; // or 200 and return something;
		//body = "";
	}
	else
	{
		std::cerr << "error delete\n";
		_code = 500;
		_read_error_page();
	}
}

std::string	Response::_build_error_page()
{
	std::ostringstream	oss;

	oss << "<html>\n<head><title>";
	oss << _code << ' ' << http::_status_header.at(_code);
	oss << "</title></head>\n<body>\n<center><h1>";
	oss << _code << ' ' << http::_status_header.at(_code);
	oss << "</h1></center>\n<hr><center>";
	oss << "webserv/0.1"; // to replace with actual serv name
	oss << "</center>\n</body>\n</html>";
	oss << std::endl;

	_header_map["Content-Type"] = "text/html";
	return (oss.str());
}

Response::MethodMap		Response::_init_method_map()
{
	MethodMap	methods;

	methods["GET"]    = &Response::_getMethod;
	methods["POST"]   = &Response::_postMethod;
	methods["DELETE"] = &Response::_deleteMethod;
	return (methods);
}

const Response::MethodMap	Response::_methods       = Response::_init_method_map();

void	Response::_set_header_map()
{
	_header_map["Server"]         = "wevserv/0.1 (ubuntu)";
	_header_map["Content-Length"] = (is_large_file ? utils::to_str(size_file) : utils::to_str(_body.size()));//utils::to_str(is_large_file ? file::size(location) : body.size());
	_header_map["Connection"]     = "keep-alive";

	if (_header_map.find("Content-Type") == _header_map.end())
		_header_map["Content-Type"] = file::get_mime(_uri, *_serv.mime_types, _serv.default_type);

	if (_code == 301)
		_header_map["location"] = _uri.substr(_serv.root.length()); // todo fill host + location
}

void		Response::_set_header()
{
	_set_header_map();

	std::ostringstream oss;

	oss << "HTTP/1.1 " << _code << " " << http::_status_header.at(_code) << "\r\n";
	for (HeaderMap::const_iterator it = _header_map.begin(); it != _header_map.end(); ++it)
		oss << it->first << ": " << it->second << "\r\n";

	oss << "\r\n";
	_header = oss.str();
}

void		Response::_set_full_response()
{
	_full_response.append(_header);
	_full_response.append(_body);
	std::cout << "\n" << color::bold << "Header to send:\n" << color::magenta << _header << color::reset << "✋\n";
	std::cout << color::bold << "Body to send:\n" << color::yellow << _body << color::reset << "✋\n";
}

Response::~Response() {
}
