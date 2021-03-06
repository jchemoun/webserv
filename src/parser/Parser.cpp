/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 20:29:10 by mjacq             #+#    #+#             */
/*   Updated: 2022/05/25 09:35:02 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include <iostream>
#include <map>
#include <cstring>
#include <algorithm>
#include "../utils/utils.hpp"

void	Parser::_init_parsers() {
	_server_parsers["listen"]               = &Parser::_parse_listen;
	_server_parsers["server_name"]          = &Parser::_parse_server_name;
	_server_parsers["location"]             = &Parser::_parse_location;
	_server_parsers["root"]                 = &Parser::_parse_root;
	_server_parsers["index"]                = &Parser::_parse_index;
	_server_parsers["error_page"]           = &Parser::_parse_error_page;
	_server_parsers["autoindex"]            = &Parser::_parse_autoindex;
	_server_parsers["default_type"]         = &Parser::_parse_default_mime;
	_server_parsers["client_max_body_size"] = &Parser::_parse_client_max_body_size;
	_server_parsers["allow_methods"]        = &Parser::_parse_allow_methods;

	_location_parsers["root"]                 = &Parser::_parse_root;
	_location_parsers["index"]                = &Parser::_parse_index;
	_location_parsers["error_page"]           = &Parser::_parse_error_page;
	_location_parsers["autoindex"]            = &Parser::_parse_autoindex;
	_location_parsers["allow_methods"]        = &Parser::_parse_allow_methods;
	_location_parsers["cgi"]                  = &Parser::_parse_cgi;
	_location_parsers["return"]               = &Parser::_parse_return;
	_location_parsers["client_max_body_size"] = &Parser::_parse_client_max_body_size;
	_location_parsers["rewrite_prefix"]       = &Parser::_parse_rewrite_prefix;
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
** @brief sort locations from smallest to biggest
*/
static bool	compare_locations(Config::Location const &loc1, Config::Location const &loc2) {
	if (loc1.type == Config::Location::type_match)
		return (true);
	else if (loc2.type == Config::Location::type_match)
			return (false);
	else
		return (loc1.location_path.size() > loc2.location_path.size());
}
/*
** ??? Syntax:	server { ... }
** ??? Default:	???
** ??? Context:	http
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
	server.mime_map = &_config.types;
	std::sort(server.locations.begin(), server.locations.end(), compare_locations);
	_config.servers.push_back(server);
	_eat(Token::type_special_char, "}");
}

/*
** ??? Syntax:	server_name name ...;
** ??? Default:	server_name "";
** ??? Context:	server
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
** ??? listen address[:port] ??? [default_server] [ssl] [http2 | spdy] [proxy_protocol] [setfib=number] [fastopen=number] [backlog=number] [rcvbuf=size] [sndbuf=size] [accept_filter=filter] [deferred] [bind] [ipv6only=on|off] [reuseport] [so_keepalive=on|off|[keepidle]:[keepintvl]:[keepcnt]];
** ??? listen port ??? [default_server] [ssl] [http2 | spdy] [proxy_protocol] [setfib=number] [fastopen=number] [backlog=number] [rcvbuf=size] [sndbuf=size] [accept_filter=filter] [deferred] [bind] [ipv6only=on|off] [reuseport] [so_keepalive=on|off|[keepidle]:[keepintvl]:[keepcnt]];
** ??? listen unix:path [default_server] [ssl] [http2 | spdy] [proxy_protocol] [backlog=number] [rcvbuf=size] [sndbuf=size] [accept_filter=filter] [deferred] [bind] [so_keepalive=on|off|[keepidle]:[keepintvl]:[keepcnt]];
** ??? Default:	listen *:80 | *:8000;
** ??? Context:	server
*/
void	Parser::_parse_listen(Config::Server &server) {
	if (!_lexer.next().expect(Token::type_word))
		throw ParsingError("listen: missing argument");

	const char *		arg = _current_token().get_value().c_str();
	Config::Connection	listen;

	if (!_is_a_number(arg))
		arg = _parse_address(listen, arg);
	try {
		if (*arg)
			listen.port = _stoi(arg, 0, std::numeric_limits<in_port_t>::max());
	}
	catch (const std::exception &err) { throw ParsingError(std::string("listen: port: ") + err.what()); }
	_lexer.next();
	_eat(Token::type_special_char, ";");
	server.listen_vect.push_back(listen);
}

const char	*Parser::_parse_address(Config::Connection &listen, const char *s) {
	listen.str_addr = s;
	listen.str_addr = listen.str_addr.substr(0, listen.str_addr.find(':'));
	if (s[0] == '*') {
		listen.addr = htonl(INADDR_ANY);
		listen.str_addr = "0.0.0.0";
	}
	else if (!std::strchr(s, '.'))
		throw ParsingError("listen: address hostname not handled: ipv4 only");
	else {
		if ( (listen.addr = inet_addr(listen.str_addr.c_str())) == std::numeric_limits<in_addr_t>::max())
			throw ParsingError("listen: address: bad format");
		listen.addr = htonl(listen.addr);
	}
	while (*s && *s != ':')
		++s;
	if (*s == ':')
		++s;
	return (s);
}

/*
** ??? Syntax:	index file ...;
** ??? Default:	index index.html;
** ??? Context:	(http, -> no need) server, location
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
** ??? Syntax:	default_type mime-type;
** ??? Default:	default_type text/plain;
** ??? Context:	[http,] server, [location]
**
** Defines the default MIME type of a response.
** Mapping of file name extensions to MIME types can be set with the types directive.
*/
void	Parser::_parse_default_mime(Config::Server &server) {
	if (!_lexer.peek_next().expect(Token::type_word))
		throw ParsingError("default_type: missing value");
	while (_lexer.next().expect(Token::type_word))
		server.default_mime = _current_token().get_value();
	_eat(Token::type_special_char, ";");
}

/*
** Syntax:
** ??? location [ = | ~ | ~* | ^~ ] uri { ... }  -> we will do only standard prefix location
** ??? location @name { ... }
** ??? Default:	???
** ??? Context:	server(, location)
*/
void	Parser::_parse_location(Config::Server &server) {
	_lexer.next();
	if (_current_token().get_type() != Token::type_word)
		throw ParsingError("location: missing path");
	Config::Location	location;
	location.location_path = _current_token().get_value();
	if (strchr(location.location_path.c_str(), '*'))
		location.type = Config::Location::type_match;
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
** ??? Syntax:	root path;
** ??? Default:	root html;
** ??? Context:	(http,) server, location, if in location
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
** ??? Syntax:	error_page code ... (??? [=[response]]) uri;
** ??? Default:	???
** ??? Context:	(http,) server, location, if in location
*/
template <class Context>
void	Parser::_parse_error_page(Context &config) {
	_lexer.next();
	if (_current_token().get_type() != Token::type_word)
		throw ParsingError("error_page: missing code");
	if (_lexer.peek_next().get_type() != Token::type_word)
		throw ParsingError("error_page: missing uri");
	std::vector<http::code>	codes;
	while (_lexer.peek_next().get_type() == Token::type_word) {
		try {
			int	code = _stoi(_current_token().get_value().c_str(), 0, 527);
			codes.push_back(static_cast<http::code>(code));
		}
		catch (std::exception &e) { throw ParsingError(std::string("error_page: code: ") + e.what());}
		_lexer.next();
	}
	for (size_t i = 0; i < codes.size(); i++)
		config.error_pages[codes.at(i)] = _current_token().get_value();
	_lexer.next();
	_eat(Token::type_special_char, ";");
}

/*
** ??? Syntax:	autoindex on | off;
** ??? Default:	autoindex off;
** ??? Context:	http, server, location
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

template <class Context>
void	Parser::_parse_client_max_body_size(Context	&context) {
	if (!_lexer.next().expect(Token::type_word))
		throw ParsingError("client_max_body_size: missing argument");
	const char *arg = _current_token().get_value().c_str();
	try { context.client_max_body_size = _stoi(arg, 0, Config::_overflow_body_size - 1); }
	catch (const std::exception &err) { throw ParsingError(std::string("listen: port: ") + err.what()); }
	_lexer.next();
	_eat(Token::type_special_char, ";");
}

/*
** Syntax:
** ??? return code [text];
** ??? return code URL;
** ??? return URL;
** ??? Default:	???
** Context:	[server,] location, [if]
*/
void	Parser::_parse_return(Config::Location &location) {
	_lexer.next();
	if (_current_token().get_type() != Token::type_word)
		throw ParsingError("return: missing code");
	if (_lexer.peek_next().get_type() != Token::type_word)
		throw ParsingError("return: missing url");
	try {
		location.return_code = static_cast<http::code>(
				_stoi(_current_token().get_value().c_str(), 0, 527));
	}
	catch (std::exception &e) { throw ParsingError(std::string("return: code: ") + e.what());}
	if (!utils::get(http::status, location.return_code))
		throw ParsingError(std::string("return: unsupported code: ") + utils::to_str(location.return_code));
	_lexer.next();
	location.return_url = _current_token().get_value();
	_lexer.next();
	_eat(Token::type_special_char, ";");

}

/*
** =========================== Nginx incompatible =========================== **
*/

static bool	is_valid_method(std::string const &method) {
	static char const *allow_methods[] = { "GET", "POST", "DELETE", "PUT" };
	for (size_t i = 0; i < sizeof(allow_methods) / sizeof(allow_methods[0]); ++i)
		if (method == allow_methods[i])
			return (true);
	return (false);
}
template <class Context>
void	Parser::_parse_allow_methods(Context &context) {
	if (!_lexer.peek_next().expect(Token::type_word))
		throw ParsingError("allow_methods: missing value");
	while (_lexer.next().expect(Token::type_word)) {
		if (is_valid_method(_current_token().get_value()))
			context.allow_methods.push_back(_current_token().get_value());
		else
			throw ParsingError("allow methods: unsupported method");
	}
	_eat(Token::type_special_char, ";");
}

/*
** @brief cgi path_to_cgi;
** Location blocks only
*/
void	Parser::_parse_cgi(Config::Location &location) {
	_lexer.next();
	if (_current_token().get_type() != Token::type_word)
		throw ParsingError("root: missing path");
	location.cgi = _current_token().get_value();
	_lexer.next();
	_eat(Token::type_special_char, ";");
}

void	Parser::_parse_rewrite_prefix(Config::Location &location) {
	_lexer.next();
	if (_current_token().get_type() != Token::type_word)
		throw ParsingError("rewrite_prefix: missing prefix");
	if (_lexer.peek_next().get_type() != Token::type_word)
		throw ParsingError("rewrite_prefix: missing substitute");
	location.rewrite_prefix.first = _current_token().get_value();
	_lexer.next();
	location.rewrite_prefix.second = _current_token().get_value();
	_lexer.next();
	_eat(Token::type_special_char, ";");
}

/*
** ============================== Parse types =============================== **
*/

/*
** ??? Syntax:	types { ... }
** ??? Default:
**     text/html  html;
**     image/gif  gif;
**     image/jpeg jpg;
** ??? Context:	http[, server, location]
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
	: std::runtime_error(std::string("\e[31;1m???? Parsing error: \e[22m") + reason + "\e[0m") {
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
