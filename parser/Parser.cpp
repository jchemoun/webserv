/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 20:29:10 by mjacq             #+#    #+#             */
/*   Updated: 2022/04/24 18:02:57 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include <iostream>

Parser::Parser(std::string filename): _lexer(filename) {
	while (_lexer.peek().get_type() != Token::type_eof)
		_parse_server_block();
}

Parser::~Parser(void) {
}

/*
** ============================= Parsing Utils ============================== **
*/

Token const	&Parser::_current_token() const {
	return (_lexer.peek());
}

void	Parser::_eat(Token::token_type type) {
	// use expect
	if (!_current_token().expect(type))
		throw std::runtime_error("Parsing error");
	_lexer.next();
}

void	Parser::_eat(Token::token_type type, Token::token_value value) {
	if (!_current_token().expect(type, value))
		throw std::runtime_error("Parsing error");
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
			throw std::runtime_error("Parsing error: missing `}' in server block");
		if (_current_token().expect(Token::type_word, "server_name")) {
			while (_lexer.next().expect(Token::type_word))
				server.server_names.push_back(_current_token().get_value());
			_eat(Token::type_special_char, ";");
		}
		if (_current_token().expect(Token::type_word, "listen")) {
			if (!_lexer.next().expect(Token::type_word))
				throw std::runtime_error("Parsing error (listen)");
			server.listen = std::stoi(_current_token().get_value());
			_lexer.next();
			_eat(Token::type_special_char, ";");
		}
	}
	_config.servers.push_back(server);
	_eat(Token::type_special_char, "}");
}

Config const	&Parser::get_config() const {
	return (_config);
}
