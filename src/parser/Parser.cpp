/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 20:29:10 by mjacq             #+#    #+#             */
/*   Updated: 2022/05/11 09:47:24 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include <iostream>
#include <map>
#include <cstring>

void	Parser::_init_parsers() {
	_server_parsers["listen"] = &Parser::_parse_listen;
	_server_parsers["server_name"] = &Parser::_parse_server_name;
	_server_parsers["index"] = &Parser::_parse_index;
	_server_parsers["location"] = &Parser::_parse_location;
	_server_parsers["root"] = &Parser::_parse_root;
	_server_parsers["error_page"] = &Parser::_parse_error_page;
	_server_parsers["autoindex"] = &Parser::_parse_autoindex;
	_server_parsers["default_type"] = &Parser::_parse_default_type;

	_location_parsers["root"] = &Parser::_parse_root;
	_location_parsers["index"] = &Parser::_parse_index;
	_location_parsers["error_page"] = &Parser::_parse_error_page;
	_location_parsers["autoindex"] = &Parser::_parse_autoindex;
}

Parser::Parser(std::string filename, std::string mimefile): _lexer(filename) {
	_init_parsers();

	while (_lexer.peek().get_type() != Token::type_eof)
		_parse_server();

	_config.set_defaults();

	try { _lexer.tokenize_file(mimefile); }
	catch (std::exception const &) { _set_default_mime_types(); return ; }

	while (_lexer.peek().get_type() != Token::type_eof)
		_parse_types();
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
** ✓ Syntax:	server { ... }
** ✓ Default:	—
** ✓ Context:	http
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
** ✓ Syntax:	server_name name ...;
** ✓ Default:	server_name "";
** ✓ Context:	server
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
** ✓ listen address[:port] ⨯ [default_server] [ssl] [http2 | spdy] [proxy_protocol] [setfib=number] [fastopen=number] [backlog=number] [rcvbuf=size] [sndbuf=size] [accept_filter=filter] [deferred] [bind] [ipv6only=on|off] [reuseport] [so_keepalive=on|off|[keepidle]:[keepintvl]:[keepcnt]];
** ✓ listen port ⨯ [default_server] [ssl] [http2 | spdy] [proxy_protocol] [setfib=number] [fastopen=number] [backlog=number] [rcvbuf=size] [sndbuf=size] [accept_filter=filter] [deferred] [bind] [ipv6only=on|off] [reuseport] [so_keepalive=on|off|[keepidle]:[keepintvl]:[keepcnt]];
** ⨯ listen unix:path [default_server] [ssl] [http2 | spdy] [proxy_protocol] [backlog=number] [rcvbuf=size] [sndbuf=size] [accept_filter=filter] [deferred] [bind] [so_keepalive=on|off|[keepidle]:[keepintvl]:[keepcnt]];
** ✓ Default:	listen *:80 | *:8000;
** ✓ Context:	server
*/
void	Parser::_parse_listen(Config::Server &server) {
	if (!_lexer.next().expect(Token::type_word))
		throw ParsingError("listen: missing argument");
	const char *arg = _current_token().get_value().c_str();
	if (!_is_a_number(arg))
		arg = _parse_address(server, arg);
	try {
		if (*arg)
			server.listen_port = _stoi(arg, 0, std::numeric_limits<in_port_t>::max());
	}
	catch (const std::exception &err) { throw ParsingError(std::string("listen: port: ") + err.what()); }
	_lexer.next();
	_eat(Token::type_special_char, ";");
}

const char	*Parser::_parse_address(Config::Server &server, const char *s) {
	std::string	&addr_part(server.listen_string_address);
	addr_part = s;
	addr_part = addr_part.substr(0, addr_part.find(':'));
	if (s[0] == '*')
		server.listen_address = htonl(INADDR_ANY);
	else if (!std::strchr(s, '.'))
		throw ParsingError("listen: address hostname not handled: ipv4 only");
	else {
		if ( (server.listen_address = inet_addr(addr_part.c_str())) == std::numeric_limits<in_addr_t>::max())
			throw ParsingError("listen: address: bad format");
		server.listen_address = htonl(server.listen_address);
	}
	while (*s && *s != ':')
		++s;
	if (*s == ':')
		++s;
	return (s);
}

/*
** ✓ Syntax:	index file ...;
** ✓ Default:	index index.html;
** ✓ Context:	(http, -> no need) server, location
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
** ✓ Syntax:	default_type mime-type;
** ✓ Default:	default_type text/plain;
** ✓ Context:	[http,] server, [location]
**
** Defines the default MIME type of a response.
** Mapping of file name extensions to MIME types can be set with the types directive.
*/
void	Parser::_parse_default_type(Config::Server &server) {
	if (!_lexer.peek_next().expect(Token::type_word))
		throw ParsingError("default_type: missing value");
	while (_lexer.next().expect(Token::type_word))
		server.default_type = _current_token().get_value();
	_eat(Token::type_special_char, ";");
}

/*
** Syntax:
** ✓ location [ = | ~ | ~* | ^~ ] uri { ... }  -> we will do only standard prefix location
** ⨯ location @name { ... }
** ✓ Default:	—
** ✓ Context:	server(, location)
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
** ✓ Syntax:	root path;
** ✓ Default:	root html;
** ✓ Context:	(http,) server, location, if in location
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
** ✓ Syntax:	error_page code ... (⨯ [=[response]]) uri;
** ✓ Default:	—
** ✓ Context:	(http,) server, location, if in location
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
		try { codes.push_back(_stoi(_current_token().get_value().c_str(), 0, 527)); }
		catch (std::exception &e) { throw ParsingError(std::string("error_page: code: ") + e.what());}
		_lexer.next();
	}
	for (size_t i = 0; i < codes.size(); i++)
		config.error_pages[codes.at(i)] = _current_token().get_value();
	_lexer.next();
	_eat(Token::type_special_char, ";");
}

/*
** ✓ Syntax:	autoindex on | off;
** ✓ Default:	autoindex off;
** ✓ Context:	http, server, location
**
** Enables or disables the directory listing output.
*/
template<class Context>
void	Parser::_parse_autoindex(Context &context) {
	_lexer.next();
	if (_current_token().get_type() != Token::type_word)
		throw ParsingError("autoindex: missing value [on|off]");
	const std::string &val(_current_token().get_value());
	if (val == "on")
		context.autoindex = true;
	else if (val == "off")
		context.autoindex = false;
	else
		throw ParsingError(std::string("autoindex: expects:[on|off] (value `") + val + "' not allowed)" );
	_lexer.next();
	_eat(Token::type_special_char, ";");
}

/*
** ✓ Syntax:	types { ... }
** ✓ Default:
**     text/html  html;
**     image/gif  gif;
**     image/jpeg jpg;
** ✓ Context:	http[, server, location]
**
** http://nginx.org/en/docs/http/ngx_http_core_module.html#types
*/
void	Parser::_parse_types() {
	_eat(Token::type_word, "types");
	_eat(Token::type_special_char, "{");
	Config::Server	server;

	while (!_current_token().expect(Token::type_special_char, "}")) {
		if (_current_token().expect(Token::type_eof))
			throw ParsingError("server: missing `}'");
		_parse_type_line();
	}
	_eat(Token::type_special_char, "}");
}
void	Parser::_parse_type_line() {
	if (!_lexer.peek().expect(Token::type_word))
		throw ParsingError("types: missing type");
	std::string	const &type = _current_token().get_value();
	if (!_lexer.peek_next().expect(Token::type_word))
		throw ParsingError("types: missing extension");
	while (_lexer.next().expect(Token::type_word))
		_config.types[_current_token().get_value()] = type;
	_eat(Token::type_special_char, ";");
}
void	Parser::_set_default_mime_types() {
	_config.types["html"] = "text/html";
	_config.types["gif"] = "image/gif";
	_config.types["jpg"] = "image/jpeg";
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

int	Parser::_stoi(std::string const &s, int min, int max) {
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

bool Parser::_is_a_number(const char *s) {
	size_t	i = 0;
	while (s[i]) {
		if (s[i] < '0' || s[i] > '9')
			return (false);
		++i;
	}
	return (true);
}

/*
** ============================== Parse types =============================== **
*/

// void	Parser::_parse_types(std::string filename) {
// }
