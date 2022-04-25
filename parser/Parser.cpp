/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 20:29:10 by mjacq             #+#    #+#             */
/*   Updated: 2022/04/25 13:37:12 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include <iostream>
#include <map>
#include <cstdlib> // std::atoi

void	Parser::_init_parsers() {
	_server_parsers["listen"] = &Parser::_parse_listen;
	_server_parsers["server_name"] = &Parser::_parse_server_name;
}

Parser::Parser(std::string filename): _lexer(filename) {
	_init_parsers();

	while (_lexer.peek().get_type() != Token::type_eof)
		_parse_server_block();
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

void	Parser::_parse_server_block() {
	_eat(Token::type_word, "server");
	_eat(Token::type_special_char, "{");
	Config::Server	server;

	while (!_current_token().expect(Token::type_special_char, "}")) {
		if (_current_token().expect(Token::type_eof))
			throw ParsingError("server: missing `}'");
		(this->*_get_server_parser())(server);
	}
	_config.servers.push_back(server);
	_eat(Token::type_special_char, "}");
}

void	Parser::_parse_server_name(Config::Server &server) {
	while (_lexer.next().expect(Token::type_word))
		server.server_names.push_back(_current_token().get_value());
	_eat(Token::type_special_char, ";");
}

void	Parser::_parse_listen(Config::Server &server) {
	if (!_lexer.next().expect(Token::type_word))
		throw ParsingError("listen: missing argument");
	server.listen = std::atoi(_current_token().get_value().c_str());
	_lexer.next();
	_eat(Token::type_special_char, ";");
}

/*
** =============================== Get parser =============================== **
*/

Parser::server_parser	Parser::_get_server_parser() const {
	if (!_current_token().expect(Token::type_word))
		throw std::runtime_error("Parsing error in server block");

	std::string const &name(_current_token().get_value());

	try {
		return (_server_parsers.at(name));
	}
	catch (std::out_of_range const &err) {
		throw ParsingError(std::string("Unknown server instruction: `") + name + "'");
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
