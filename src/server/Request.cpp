/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 13:17:12 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/25 08:00:47 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "Parser.hpp"
#include <cstring>
#include <color.hpp>
#include "utils.hpp"

/*
** ======================== Constructor / Destructor ======================== **
*/

Request::Request():
	_status_code(http::Ok),
	_content_length(0),
	_complete_request_line(false),
	_complete_header(false),
	_complete_body(false),
	_index(0),
	current_server(NULL)
{ }

Request::~Request() { }

/*
** ================================ Getters ================================= **
*/

std::string     const &Request::get_method()       const { return (_method);       }
std::string     const &Request::get_request_uri()  const { return (_request_uri);  }
std::string     const &Request::get_protocol()     const { return (_protocol);     }
std::string     const &Request::get_body()         const { return (_body);         }
Request::Header const &Request::get_header()       const { return (_header);       }
std::string     const &Request::get_uri()          const { return (_uri);          }
std::string     const &Request::get_query_string() const { return (_query_string); }
http::code             Request::get_status_code()  const { return (_status_code);  }

/*
** ============================== Public Utils ============================== **
*/

void	Request::append_unparsed_request(char *buffer, ssize_t len) {
	_raw_str.append(buffer, len);
}

bool	Request::is_complete() const { return (_complete_body || is_invalid()); }
bool	Request::is_invalid()  const { return (_status_code >= 400); }

void	Request::reset() {
	_status_code = http::Ok;
	_complete_request_line = false;
	_complete_header = false;
	_complete_body = false;
	_raw_str.clear();
	_body.clear();
	_index = 0;
	_content_length = 0;
}

/*
** ============================= Resolve server ============================= **
*/

/*
** @brief set the current server and the current server name according to the header Host
**
** Fallback to the default server in either cases:
** - Host header not found
** - Host header not matching with any server listening on the current listen fd
*/
void	Request::_resolve_server(ServerMap &serverMap, DefaultServerMap &default_server_map)
{
	std::string const	*host = utils::get(_header, std::string("Host"));

	if (host)
	{
		std::string		server_name       = host->substr(0, host->find(':'));
		NameToServMap	&name_to_serv_map = serverMap.at(listen_info->fd);
		Config::Server	**server          = utils::get(name_to_serv_map, server_name);

		if (server)
		{
			current_server      = *server;
			current_server_name = server_name;

			std::cout << "Found server name matching Host: "
				<< color::bold << color::green << server_name << color::reset << "\n";
			return ;
		}
	}
	current_server      = default_server_map.at(listen_info->fd);
	current_server_name = (current_server->server_names.empty() ? "" : current_server->server_names[0]);

	std::cout << "No server found where name is matching Host. Fallback to default server "
		<< color::bold << color::yellow << current_server_name << color::reset << "\n";
}


/*
** ================================ Parsing ================================= **
*/

void	Request::parse_request(ServerMap &serverMap, DefaultServerMap &def_server_map)
{
	std::cout << color::bold << "\nOngoing raw request:\n" << color::reset << color::green << _raw_str << color::reset << "✋\n";

	try {
		if (!_complete_request_line)
			_parse_request_line();
		if (_complete_request_line && !_complete_header) {
			_parse_header();
			_resolve_server(serverMap, def_server_map);
			_check_headers();
		}
		if (_complete_header && !_complete_body)
			_parse_body();
	}
	catch (http::code status_code) {
		std::cerr << color::red << "Parsing request error: " << status_code << color::reset << "\n";
		_status_code = status_code;
		if (!current_server)
			_resolve_server(serverMap, def_server_map);
	}
}

void	Request::parse_cgi(std::string &output) {
	std::swap(_raw_str, output);
	// _raw_str = output;
	std::cout << "Parse cgi: \n" << _raw_str;
	try {
		_parse_header();
		_body = _raw_str.substr(_index);
	} catch (http::code status_code) {
		std::cerr << color::red << "Parsing request error: " << status_code << color::reset << "\n";
		_status_code = status_code;
	}
}

void	Request::_check_headers() {
	if (_content_length > current_server->client_max_body_size)
		throw (http::PayloadTooLarge);
}

/*
** Syntax:
** Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
**
** Note:
** - uri should not contain literal space
** - a good client will end each line by \r\n, but nginx also works with single '\n'
** Resources:
** - https://www.tutorialspoint.com/http/http_requests.htm
** - https://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html
*/
void	Request::_parse_request_line() {
	std::cout << color::bold << "\nParse request line and headers:\n" << color::reset;
	_skip_empty_lines();
	if (!_raw_str[_index] || _raw_str.find("\n", _index) == std::string::npos) {
		_raw_str.erase(0, _index);
		_index = 0;
		return ;
	}
	_eat_word(_method);			std::cout << "> " << color::cyan << "method: " << color::magenta << _method << "\n" << color::reset;
	_eat_spaces();
	_eat_word(_request_uri);	std::cout << "> " << color::cyan << "request uri: " << color::magenta << _request_uri << "\n" << color::reset;
	_parse_request_uri();
	_eat_spaces();
	_eat_word(_protocol);		std::cout << "> " << color::cyan << "protocol: " << color::magenta << _protocol << "\n" << color::reset;
	_parse_protocol();
	_eat_eol();
	_complete_request_line = true;
	std::cout << "\n> " << color::cyan << "Headers:\n" << color::reset;
}

/*
** request_uri = uri[?query_string]
*/
void	Request::_parse_request_uri() {
	_uri = _request_uri.substr(0, _request_uri.find('?'));
	if (_uri != _request_uri)
		_query_string = _request_uri.substr(_uri.size() + 1);
	else
		_query_string = "";
	std::cout << "  > " << color::cyan << "uri: " << color::magenta << _uri << "\n" << color::reset;
	std::cout << "  > " << color::cyan << "query_string: " << color::magenta << _query_string << "\n" << color::reset;
}

/*
** Supported protocols: HTTP/1.0, HTTP/1.1
*/
void	Request::_parse_protocol() {
	if (_protocol != "HTTP/1.1")
		_status_code = http::HTTPVersionNotSupported;
}

/*
** syntax: "key: value"
** - more precisely: `message-header = field-name ":" [ field-value ]`
** remarks:
** - space is not mandatory (so no _eat_spaces())
** - multiple spaces allowed
** - https://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html
*/
void	Request::_parse_header() {
	while (_raw_str.find("\n", _index) != std::string::npos) {
		if (_skip_empty_line()) {
			_complete_header = true;
			return ;
		}
		_eat_key();
		_eat_value();
	}
}

void	Request::_eat_key() {
	_eat_word(_tmp_key, ":\r\n");
	_eat(":");
	while (_raw_str[_index] == ' ')
		++_index;
	std::cout << "  >> " << color::cyan << _tmp_key << color::reset << ": ";
}

void	Request::_eat_value() {
	std::string	value;
	_eat_word(value, "\r\n", true);
	_eat_eol();
	if (_tmp_key == "Content-Length")
		_parse_content_length(value);
	_header[_tmp_key] = value;
	std::cout << color::magenta << value << color::reset << "\n";
}

void	Request::_parse_body() {
	std::string body_part = _raw_str.substr(_index, _content_length);
	_index += body_part.size();
	_content_length -= body_part.size();
	_body += body_part;
	if (_content_length == 0) {
		if (_raw_str[_index])
			throw (http::BadRequest); //std::runtime_error("body size exceeds expected content length");
		else {
			_complete_body = true;
			std::cout << color::bold << "\nParsed body:\n" << color::reset << color::blue << _body << color::reset << "✋\n";
		}
	}
}

void	Request::_parse_content_length(std::string const &value) {
	try {
		_content_length = Parser::_stoi(value, 0, Config::_overflow_body_size - 1);
	}
	catch (std::exception	const &except){
		throw (http::BadRequest); // std::runtime_error(std::string("Content-Length: ") + except.what());
	}
}
/*
** ============================= Parsing Utils ============================== **
*/

/*
** remark: Using const char * instead of std::string is a bit more efficient
** and parsing request efficiency is critical (way more than parsing config)
*/
void	Request::_eat(const char *s) {
	size_t	size = std::strlen(s);
	if (_raw_str.substr(_index, size) != s)
		throw (http::BadRequest); // std::runtime_error("syntax error");
	_index += size;
}

void	Request::_eat_word(std::string &s, const char *delimiter, bool allow_empty) {
	size_t	count = 0;
	char c;
	while ((c = _raw_str[_index + count]) && !strchr(delimiter, c))
		++count;
	if (count == 0 && !allow_empty)
		throw (http::BadRequest); // std::runtime_error("syntax error");;
	s = _raw_str.substr(_index, count);
	_index += s.size();
}

void	Request::_eat_word(std::string &s) {
	static const char* word_delimiter = " \r\n";
	_eat_word(s, word_delimiter);
}

void	Request::_eat_eol() {
	if (_raw_str[_index] == '\r')
		++_index;
	_eat("\n");
}

void	Request::_eat_spaces() {
	_eat(" ");
	while (_raw_str[_index] == ' ')
		++_index;
}

bool	Request::_skip_empty_line() {
	if (_raw_str[_index] == '\r' && _raw_str[_index + 1] == '\n')
		_index += 2;
	else if (_raw_str[_index] == '\n')
		++_index;
	else
		return (false);
	return (true);
}

void	Request::_skip_empty_lines() {
	while (_skip_empty_line());
}
