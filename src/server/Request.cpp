/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 13:17:12 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/11 14:25:59 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "Parser.hpp"
#include <cstring>

/*
** ======================== Constructor / Destructor ======================== **
*/

Request::Request():
	_max_body_size(4242), _content_length(0), // TODO: parse _max_body_size in config
	_complete_request_line(false),
	_complete_header(false),
	_complete_body(false),
	_invalid_request(false),
	_index(0)
{ }

Request::~Request() { }

/*
** ================================ Getters ================================= **
*/

std::string     const &Request::get_method()   const { return (method);   }
std::string     const &Request::get_location() const { return (location); }
std::string     const &Request::get_protocol() const { return (protocol); }
std::string     const &Request::get_body()     const { return (body);     }
Request::Header const &Request::get_header()   const { return (header);   }

/*
** ============================== Public Utils ============================== **
*/

void	Request::append_unparsed_request(char *buffer, ssize_t len) {
	_raw_str.append(buffer, len);
}

bool	Request::is_complete() const { return (_complete_body || _invalid_request); }
bool	Request::is_invalid()  const { return (_invalid_request); }

void	Request::reset() {
	_complete_request_line = false;
	_complete_header = false;
	_complete_body = false;
	_invalid_request = false;
	_raw_str.clear();
	body.clear();
	// _raw_str.erase(0, _index);  // what we should do if we implemented http pipeling (https://developer.mozilla.org/en-US/docs/Web/HTTP/Connection_management_in_HTTP_1.x#http_pipelining)
	_index = 0;
	_content_length = 0;
}

/*
** ================================ Parsing ================================= **
*/

void	Request::parse_request()
{
	std::cout << "\e[32m" << _raw_str << "\e[0m✋\n";

	try {
		if (!_complete_request_line)
			_parse_request_line();
		if (_complete_request_line && !_complete_header)
			_parse_header();
		if (_complete_header && !_complete_body)
			_parse_body();
	}
	catch (std::runtime_error const &except) {
		std::cerr << "\e[38;5;208mParsing request: " << except.what() << "\e[0m\n";
		_invalid_request = true;
	}
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
	_skip_empty_lines();
	if (!_raw_str[_index] || _raw_str.find("\n", _index) == std::string::npos) {
		_raw_str.erase(0, _index);
		_index = 0;
		return ;
	}
	_eat_word(method);		std::cout << "> method: " << method << "\n";
	_eat_spaces();
	_eat_word(location);	std::cout << "> location: " << location << "\n";
	_eat_spaces();
	_eat_word(protocol);	std::cout << "> protocol: " << protocol << "\n";
	_eat_eol();
	_complete_request_line = true;
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
	std::cout << "\e[36m> " << _tmp_key << ": \e[0m";
}

void	Request::_eat_value() {
	std::string	value;
	_eat_word(value, "\r\n", true);
	_eat_eol();
	if (_tmp_key == "Content-Length")
		_parse_content_length(value);
	header[_tmp_key] = value;
	std::cout << "\e[36m"<< value << "\e[0m\n";
}

void	Request::_parse_body() {
	std::string body_part = _raw_str.substr(_index, _content_length);
	_index += body_part.size();
	_content_length -= body_part.size();
	body += body_part;
	if (_content_length == 0) {
		if (_raw_str[_index])
			throw std::runtime_error("body size exceeds expected content length");
		else {
			_complete_body = true;
			std::cout << "Parsed body:\n\e[38;5;218m" << body << "✋\e[0m\n";
		}
	}
}

void	Request::_parse_content_length(std::string const &value) {
	try {
		_content_length = Parser::_stoi(value, 0, _max_body_size);// TODO: better parsing for max_body_size
	}
	catch (std::exception	const &except){
		throw std::runtime_error(std::string("Content-Length: ") + except.what());
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
		throw std::runtime_error("syntax error");
	_index += size;
}

void	Request::_eat_word(std::string &s, const char *delimiter, bool allow_empty) {
	size_t	count = 0;
	char c;
	while ((c = _raw_str[_index + count]) && !strchr(delimiter, c))
		++count;
	if (count == 0 && !allow_empty)
		throw std::runtime_error("syntax error");;
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
