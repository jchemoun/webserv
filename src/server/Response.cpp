/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/30 14:02:37 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/24 09:52:41 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "color.hpp"
#include "utils.hpp"

Response::Uri::Uri(const std::string &path, Config::Server const &serv):
	path(path),
	root(&serv.root),
	indexes(&serv.index),
	error_pages(&serv.error_pages),
	allow_methods(&serv.allow_methods)
{ }

void Response::Uri::resolve(Config::Server const &serv) {
	root = &serv.root;
	indexes = &serv.index;
	error_pages = &serv.error_pages;
	allow_methods = &serv.allow_methods;
	for (size_t i = 0; i < serv.locations.size(); ++i) {
		Config::Location const &location = serv.locations.at(i);
		std::string const &location_path = location.location_path;
		if (!strncmp(location_path.c_str(), path.c_str(), location_path.size())) {
			if (!location.root.empty())
				root = &location.root;
			if (!location.index.empty())
				indexes = &location.index;
			if (!location.error_pages.empty())
				error_pages = &location.error_pages;
			if (!location.allow_methods.empty())
				allow_methods = &location.allow_methods;
		}
	}
	full_path = file::join(*root, path);
}

/*
** ============================= Public methods ============================= **
*/

Response::Response(Request const &req, Config::Connection const &client_info):
	_header			(),
	_body			(),
	_full_response	(),
	_code			(req.get_status_code()),
	_autoindex		(req.current_server->autoindex),
	_request_uri	(req.get_request_uri()),
	_query_string	(req.get_query_string()),
	_serv			(*req.current_server),
	_uri			(req.get_uri(), _serv),
	_client_info	(client_info),
	_req			(req),
	is_large_file	(0),
	size_file		(0)
{
	if (_req.is_invalid()) {
		_read_error_page(_code);
	}
	else {
		_uri.resolve(_serv);
		if (_methods.find(req.get_method()) != _methods.end())
			(this->*_methods.at(req.get_method()))();
		else
			_read_error_page(http::MethodNotAllowed);
	}
	_set_header();
	_set_full_response();
}

const char *Response::c_str() const { return (_full_response.c_str()); }

size_t		Response::size()  const { return (_full_response.size());  }

/*
** ============================ Private Methods ============================= **
*/

/*
** @brief list every folder then file, except hidden ones
*/
void	Response::_create_auto_index_page()
{
	std::ostringstream			oss;
	std::ostringstream			oss_file;
	std::vector<std::string>	entries;
	DIR							*dir;
	struct dirent				*ent;

	// probably error need check if exist && exec perm
	if (file::has_read_perm(_uri.full_path) == false)
		return (_read_error_page(http::Forbidden));
	if ((dir = opendir(_uri.full_path.c_str())) == NULL)
		return (_read_error_page(http::NotFound));
	oss <<
		"<html>\r\n"
		"<head><title>Index of " << _uri.path << "</title></head>\r\n"
		"<body>\r\n"
		"<h1>Index of " << _uri.path << "</h1><hr><pre><a href=\"../\">../</a>\r\n";
	while ((ent = readdir(dir)) != NULL)
		if (ent->d_name[0] != '.')
			entries.push_back(ent->d_name);
	std::sort(entries.begin(), entries.end());
	for (std::vector<std::string>::const_iterator cit = entries.begin(); cit != entries.end(); cit++)
	{
		if (file::get_type(_uri.full_path + *(cit)) == file::FT_DIR)
			oss <<
				"<a href=\"" << *(cit) << "/\">" << *(cit) << "/</a>\t" << file::time_last_change(_uri.full_path + *(cit)) << "\t-\r\n";
		else
			oss_file <<
				"<a href=\"" << *(cit) << "\">" << *(cit) << "</a>\t" << file::time_last_change(_uri.full_path + *(cit)) << '\t' << file::size(_uri.full_path + *(cit)) << "\r\n";
	}
	oss << oss_file.str() <<
		"</pre><hr></body>\r\n"
		"</html>\r\n";
	_code = http::Ok;
	_body = oss.str();
	_header_map["Content-Type"] = "text/html";
	closedir(dir);
}

void	Response::_read_file()
{
	std::stringstream	buf;
	file::e_type		ft = file::get_type(_uri.full_path);

	if (ft == file::FT_DIR)
	{
		if (*(_uri.full_path.rbegin()) != '/')
		{
			_uri.path += '/';
			_uri.resolve(_serv);
			return (_read_error_page(http::MovedPermanently));
		}
		for (size_t i = 0; i < _uri.indexes->size(); ++i)
		{
			std::string	const &index_candidate =_uri.indexes->at(i);
			if (file::get_type(file::join(_uri.full_path, index_candidate)) == file::FT_FILE) {
				std::cout << color::bold << "Index found: " << color::magenta << index_candidate << color::reset << '\n';
				_uri.path = file::join(_uri.path, index_candidate);
				_uri.resolve(_serv);
				return (_read_file());
			}
		}
		if (_autoindex)
			return (_create_auto_index_page());
		else
			return (_read_error_page(http::Forbidden));
	}
	else if (file::get_type(_uri.full_path) == file::FT_FILE)
	{
		if (file::has_read_perm(_uri.full_path) == false)
			return (_read_error_page(http::Forbidden));
		file.open(_uri.full_path.c_str(), std::ifstream::in);
		if (file.is_open() == false)
			return (_read_error_page(http::NotFound));
		if ((size_file = file::size(_uri.full_path)) + 200 >= BUFFER_SIZE)
		{
			is_large_file = 1;
			_code = http::Ok;
			return ;
		}
		buf << file.rdbuf();
		file.close();
		_body = buf.str();
	}
	else
		return (_read_error_page(http::NotFound));
	_code = http::Ok;
}

// read file for error pages
void		Response::_read_error_page(http::code error_code)
{
	std::ofstream		file;
	std::stringstream	buf;

	_code = error_code;
	std::cout << "Current status: " << color::red << _code << color::reset << "\n";

	std::string const	*error_page_ptr = utils::get(*_uri.error_pages, _code);
	if (error_page_ptr)
	{
		std::string	error_page = file::join(*_uri.root, *error_page_ptr);
		if ((file::get_type(error_page) == file::FT_FILE) && file::has_read_perm(error_page)) // nginx actually search index if it is a folder
		{
			file.open(error_page.c_str(), std::ifstream::in);
			if (file.is_open()) {
				buf << file.rdbuf();
				file.close();
				_body = buf.str();
				_header_map["Content-Type"] = _serv.get_mime(error_page);
				return ;
			}
		}
	}
	_build_error_page();
}

void		Response::_run_cgi() {
	try {
		Cgi	cgi(_req, _client_info);
		cgi.run();
		std::swap(cgi.body, _body);
		for (Cgi::Header::const_iterator cit = cgi.header.begin(); cit != cgi.header.end(); ++cit) {
			if (cit->first == "Status")
				_cgi_status = cit->second;
			else
				_header_map[cit->first] = cit->second;
		}
	} catch (http::code error_code) { // InternalServerError
		_read_error_page(error_code);
	}
}

void		Response::_getMethod()
{
	if (_is_a_cgi())
		_run_cgi();
	else
		_read_file();
}

bool		Response::_is_a_cgi() const {
	return (_uri.full_path.find("/cgi-bin/") != std::string::npos);
}

void		Response::_postMethod()
{
	_getMethod();
}

void		Response::_deleteMethod()
{
	if (file::get_type(_uri.full_path) == file::FT_UNKOWN)
		_read_error_page(http::NotFound);
	else if (file::has_write_perm(_uri.full_path) == false)
		_read_error_page(http::Forbidden);
	else if (remove(_uri.full_path.c_str()) != -1)
		_code = http::NoContent;
	else
	{
		std::cerr << "error delete\n";
		_read_error_page(http::InternalServerError);
	}
}

void		Response::_putMethod()
{
	std::string		updir;
	std::ofstream	new_file;
	bool			existing;

	updir = _uri.full_path.substr(0, _uri.full_path.rfind('/'));
	existing = false;
	if (updir == _uri.full_path || updir.empty())
	{
		_read_error_page(http::BadRequest);
		return ;
	}
	if (file::get_type(updir) != file::FT_DIR)
	{
		_read_error_page(http::NotFound);
		return ;
	}
	if (file::get_type(_uri.full_path) == file::FT_DIR)
	{
		_read_error_page(http::Forbidden);
		return ;
	}
	if (file::get_type(_uri.full_path) == file::FT_FILE)
	{
		existing = true;
		if (file::has_write_perm(_uri.full_path) == false)
		{
			_read_error_page(http::Forbidden);
			return ;
		}
	}
	new_file.open(_uri.full_path.c_str());
	if (chmod(_uri.full_path.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) == -1)
	{
		_read_error_page(http::InternalServerError);
		return ;
	}
	if (new_file.is_open() == false)
	{
		_read_error_page(http::InternalServerError);
		return ;
	}
	new_file.write(_req.get_body().c_str(), _req.get_body().length());
	if (new_file.bad() || new_file.fail())
	{
		_read_error_page(http::InternalServerError);
		return ;
	}
	new_file.close();
	if (existing)
		_code = http::NoContent;
	else
		_code = http::Created;
	return ;
}

void	Response::_build_error_page()
{
	std::ostringstream	oss;

	oss <<
		"<html>\r\n"
		"<head><title>" << _code << ' ' << http::status.at(_code) << "</title></head>\r\n"
		"<body>\r\n"
		"<center><h1>" << _code << ' ' << http::status.at(_code) << "</h1></center>\r\n"
		"<hr><center>" SERVER_SOFTWARE "</center>\r\n"
		"</body>\r\n"
		"</html>\r\n";

	_body = oss.str();
	_header_map["Content-Type"] = "text/html";
}

Response::MethodMap		Response::_init_method_map()
{
	MethodMap	methods;

	methods["GET"]		= &Response::_getMethod;
	methods["POST"]		= &Response::_postMethod;
	methods["DELETE"]	= &Response::_deleteMethod;
	methods["PUT"]		= &Response::_putMethod;
	return (methods);
}

const Response::MethodMap	Response::_methods = Response::_init_method_map();

void	Response::_set_header_map()
{
	_header_map["Server"]         = SERVER_SOFTWARE;
	_header_map["Content-Length"] = (is_large_file ? utils::to_str(size_file) : utils::to_str(_body.size()));
	_header_map["Connection"]     = "keep-alive";

	if (_header_map.find("Content-Type") == _header_map.end())
		_header_map["Content-Type"] = _serv.get_mime(_uri.path);

	if (_code == http::MovedPermanently)
		_header_map["Location"] = _uri.path; // better: add scheme and host
}

void		Response::_set_header()
{
	_set_header_map();

	std::ostringstream oss;

	// Request line
	oss << "HTTP/1.1 ";
	if (!_cgi_status.empty())
		oss << _cgi_status;
	else
		oss << _code << " " << http::status.at(_code);
	oss << "\r\n";

	// Headers
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
