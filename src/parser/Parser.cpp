/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 20:29:10 by mjacq             #+#    #+#             */
/*   Updated: 2022/04/26 18:14:24 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include <iostream>
#include <map>
// #include <cstdlib> // std::atoi

void	Parser::_init_parsers() {
	_server_parsers["listen"] = &Parser::_parse_listen;
	_server_parsers["server_name"] = &Parser::_parse_server_name;
	_server_parsers["index"] = &Parser::_parse_index;
	_server_parsers["location"] = &Parser::_parse_location;
	_server_parsers["root"] = &Parser::_parse_root;
	_server_parsers["error_page"] = &Parser::_parse_error_page;

	_location_parsers["root"] = &Parser::_parse_root;
	_location_parsers["index"] = &Parser::_parse_index;
	_location_parsers["error_page"] = &Parser::_parse_error_page;
}

Parser::Parser(std::string filename): _lexer(filename) {
	_init_parsers();

	while (_lexer.peek().get_type() != Token::type_eof)
		_parse_server();
}

Parser::~Parser(void) {
}

Config const	&Parser::get_config() const {
	return (_config);
}

/*
** ============================= Parsing Utils ============================== **
*/

Token const	&Parser::_current_token() const {
	return (_lexer.peek());
}

void	Parser::_eat(Token::token_type type) {
	if (!_current_token().expect(type))
		throw ParsingError(std::string("Near token `") + _current_token().get_value() + "'");
	_lexer.next();
}

void	Parser::_eat(Token::token_type type, Token::token_value value) {
	if (!_current_token().expect(type, value))
		throw ParsingError(std::string("Near token `") + _current_token().get_value() + "'");
	_lexer.next();
}

/*
** =========================== Parsing functions ============================ **
*/

/*
**  Syntax:	server { ... }
**  Default:	—
**  Context:	http
*/
void	Parser::_parse_server() {
	_eat(Token::type_word, "server");
	_eat(Token::type_special_char, "{");
	Config::Server	server;

	while (!_current_token().expect(Token::type_special_char, "}")) {
		if (_current_token().expect(Token::type_eof))
			throw ParsingError("server: missing `}'");
		(this->*_get_directive_parser(_server_parsers))(server);
	}
	_config.servers.push_back(server);
	_eat(Token::type_special_char, "}");
}

/*
**  Syntax:	server_name name ...;
**  Default:	server_name "";
**  Context:	server
*/
void	Parser::_parse_server_name(Config::Server &server) {
	if (!_lexer.peek_next().expect(Token::type_word))
		throw ParsingError("server_name: missing value");
	while (_lexer.next().expect(Token::type_word))
		server.server_names.push_back(_current_token().get_value());
	_eat(Token::type_special_char, ";");
}

/*
** Syntax:
** ⨯ listen address[:port] [default_server] [ssl] [http2 | spdy] [proxy_protocol] [setfib=number] [fastopen=number] [backlog=number] [rcvbuf=size] [sndbuf=size] [accept_filter=filter] [deferred] [bind] [ipv6only=on|off] [reuseport] [so_keepalive=on|off|[keepidle]:[keepintvl]:[keepcnt]];
**  listen port ⨯ [default_server] [ssl] [http2 | spdy] [proxy_protocol] [setfib=number] [fastopen=number] [backlog=number] [rcvbuf=size] [sndbuf=size] [accept_filter=filter] [deferred] [bind] [ipv6only=on|off] [reuseport] [so_keepalive=on|off|[keepidle]:[keepintvl]:[keepcnt]];
** ⨯ listen unix:path [default_server] [ssl] [http2 | spdy] [proxy_protocol] [backlog=number] [rcvbuf=size] [sndbuf=size] [accept_filter=filter] [deferred] [bind] [so_keepalive=on|off|[keepidle]:[keepintvl]:[keepcnt]];
** ⨯ Default:	listen *:80 | *:8000;
**  Context:	server
*/
void	Parser::_parse_listen(Config::Server &server) {
	if (!_lexer.next().expect(Token::type_word))
		throw ParsingError("listen: missing argument");
	try { server.listen = stoi(_current_token().get_value().c_str(), 0); }
	catch (const std::exception &err) { throw ParsingError(std::string("listen: port: ") + err.what()); }
	_lexer.next();
	_eat(Token::type_special_char, ";");
}

/*
**  Syntax:	index file ...;
** ⨯ Default:	index index.html;
**  Context:	(http, -> no need) server, location
*/
template <class Context>
void	Parser::_parse_index(Context &server) {
	if (!_lexer.peek_next().expect(Token::type_word))
		throw ParsingError("index: missing value");
	while (_lexer.next().expect(Token::type_word))
		server.index.push_back(_current_token().get_value());
	_eat(Token::type_special_char, ";");
}

/*
** Syntax:
**  location [ = | ~ | ~* | ^~ ] uri { ... }  -> we will do only standard prefix location
** ⨯ location @name { ... }
**  Default:	—
**  Context:	server(, location)
*/
void	Parser::_parse_location(Config::Server &server) {
	_lexer.next();
	if (_current_token().get_type() != Token::type_word)
		throw ParsingError("location: missing path");
	Config::Location	location;
	location.location_path = _current_token().get_value();
	_lexer.next();
	_eat(Token::type_special_char, "{");
	while (!_current_token().expect(Token::type_special_char, "}")) {
		if (_current_token().expect(Token::type_eof))
			throw ParsingError("server: missing `}'");
		(this->*_get_directive_parser(_location_parsers))(location);
	}
	_eat(Token::type_special_char, "}");
	server.locations.push_back(location);
}

/*
**  Syntax:	root path;
** ⨯ Default:	root html;
**  Context:	(http,) server, location, if in location
*/
template <class Context>
void	Parser::_parse_root(Context &context) {
	_lexer.next();
	if (_current_token().get_type() != Token::type_word)
		throw ParsingError("root: missing path");
	context.root = _current_token().get_value();
	_lexer.next();
	_eat(Token::type_special_char, ";");
}

/*
**  Syntax:	error_page code ... (⨯ [=[response]]) uri;
**  Default:	—
**  Context:	(http,) server, location, if in location
*/
template <class Context>
void	Parser::_parse_error_page(Context &config) {
	_lexer.next();
	if (_current_token().get_type() != Token::type_word)
		throw ParsingError("error_page: missing code");
	if (_lexer.peek_next().get_type() != Token::type_word)
		throw ParsingError("error_page: missing uri");
	std::vector<int>	codes;
	while (_lexer.peek_next().get_type() == Token::type_word) {
		try { codes.push_back(stoi(_current_token().get_value().c_str(), 0, 527)); }
		catch (std::exception &e) { throw ParsingError(std::string("error_page: code: ") + e.what());}
		_lexer.next();
	}
	for (size_t i = 0; i < codes.size(); i++)
		config.error_pages[codes.at(i)] = _current_token().get_value();
	_lexer.next();
	_eat(Token::type_special_char, ";");
}

/*
** =============================== Get parser =============================== **
*/

template <class parser_type>
parser_type	Parser::_get_directive_parser(std::map<std::string, parser_type> &parsers) const {
	if (!_current_token().expect(Token::type_word))
		throw std::runtime_error("Parsing error in server block");

	std::string const &directive(_current_token().get_value());

	try {
		return (parsers.at(directive));
	}
	catch (std::out_of_range const &err) {
		throw ParsingError(std::string("server: Unknown directive: `") + directive + "'");
	}
}

/*
** ============================== ParsingError ============================== **
*/

Parser::ParsingError::ParsingError(std::string reason)
	: std::runtime_error(std::string("\e[31;1m💀 Parsing error: \e[22m") + reason + "\e[0m") {
	}

Parser::ParsingError::ParsingError(const ParsingError &err)
	: std::runtime_error(err) {
	}

Parser::ParsingError::~ParsingError() throw() { }

/*
** ============================== Static utils ============================== **
*/

int	Parser::stoi(std::string const &s, int min, int max) {
	long	n = 0;
	size_t	i = 0;
	char	sign = 1;

	if (s[0] == '+') ++i; else if (s[0] == '-') { sign = -1; ++i; }

	if (!s[i]) throw std::runtime_error("missing value");

	while (s[i] >= '0' && s[i] <= '9') {
		n *= 10;
		n += s[i] - '0';
		if (sign * n < min || sign *n > max) {
			if (sign * n < 0 && min == 0)
				throw std::runtime_error(std::string("should not be negative: ") + s);
			else
				throw std::runtime_error(std::string("out of range: ") + s);
		}
		++i;
	}
	if (s[i])
		throw std::runtime_error(std::string("not an integer: ") + s);
	return (sign * n);
}
