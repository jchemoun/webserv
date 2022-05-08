/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/29 13:17:12 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/08 22:51:40 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include <cstring>

Request::Request(): _is_complete(false), _index(0)
{
}

/*
** Syntax:
** Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
**
** Note: uri should not contain literal space
** Resources:
** - https://www.tutorialspoint.com/http/http_requests.htm
** - https://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html
*/
void	Request::parse_request()
{
	if (_raw_str.find("\r\n\r\n") != std::string::npos || _raw_str.find("\n\n") != std::string::npos) {
		std::cout << "\e[32mComplete request!\e[0m\n";
		std::cout << "\e[32m" << _raw_str << "\e[0m✋\n";
		_is_complete = true;
	}
	else {
		std::cout << "\e[33mIncomplete request...\e[0m\n";
		return ;
	}

	while (_raw_str[_index] == '\r' && _raw_str[_index + 1] == '\n')
		_index += 2;
	_eat_word(method);		std::cout << "> method: " << method << "\n";
	_eat(" ");
	_eat_word(location);	std::cout << "> location: " << location << "\n";
	_eat(" ");
	_eat_till_eol(protocol);	std::cout << "> protocol: " << protocol << "\n";
	_eat("\r\n");
	while (_raw_str.find("\r\n", _index) != std::string::npos) {
		if (_raw_str.substr(_index, 2) == "\r\n")
			return ;
		_eat_key();
		_eat_value();
	}
	_raw_str.erase(0, _index);
	_index = 0;
}

void	Request::append_unparsed_request(char *buffer, ssize_t len)
{
	_raw_str.append(buffer, len);
}

bool	Request::is_complete() { return (_is_complete); }

std::string const	&Request::get_location() const {
	return (location);
}

Request::~Request() { }

/*
** ============================= Parsing Utils ============================== **
*/

/*
** Using const char * instead of std::string is a bit more efficient
** and parsing request efficiency is critical (way more than parsing config)
*/
void	Request::_eat(const char *s) {
	size_t	size = std::strlen(s);
	if (_raw_str.substr(_index, size) != s)
		throw std::runtime_error("parse_request error");
	_index += size;
}

void	Request::_eat_word(std::string &s) {
	if (!_raw_str[_index] || std::strchr(" \r\n", _raw_str[_index]))
		throw std::runtime_error("parse_request error");;
	s = _raw_str.substr(
			_index,
			_raw_str.find(" ", _index) - _index
			);
	_index += s.size();
}

void	Request::_eat_till_eol(std::string &s) {
	if (!_raw_str[_index] || std::strchr(" \r\n", _raw_str[_index]))
		throw std::runtime_error("parse_request error");;
	size_t	count = 0;
	while (_raw_str[_index + count] != '\r' && _raw_str[_index + count != '\n'])
		++count;
	s = _raw_str.substr(
			_index,
			count
			);
	_index += s.size();
}

void	Request::_eat_key() {
	_tmp_key = _raw_str.substr(
			_index,
			_raw_str.find(":", _index) - _index
			);
	_index += _tmp_key.size();
	_eat(": ");
	std::cout << "\e[36m> " << _tmp_key << ": \e[0m";
}

void	Request::_eat_value() {
	std::string	value = _raw_str.substr(
			_index,
			_raw_str.find("\r\n", _index) - _index
			);
	_index += value.size();
	_eat("\r\n");
	header[_tmp_key] = value;
	std::cout << "\e[36m"<< value << "\e[0m\n";
}
