/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 20:29:10 by mjacq             #+#    #+#             */
/*   Updated: 2022/04/24 17:50:01 by mjacq            ###   ########.fr       */
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
** =============================== Structures =============================== **
*/

void	Parser::Location::print() const {
	std::cout << "Location path: " << location_path << std::endl;
	std::cout << "root: " << root << std::endl;
}

void	Parser::Server::print() const {
	std::cout << "Server: ";
	for (size_t i = 0; i < server_names.size(); i++)
		std::cout << server_names[i] << " ";
	std::cout << std::endl;
	std::cout << "Listening port: " << listen << std::endl;
	for (size_t i = 0; i < locations.size(); i++)
		locations[i].print();
}

void	Parser::Config::print() const {
	std::cout << "> CONFIG:" << std::endl;
	for (size_t i = 0; i < servers.size(); ++i) {
		servers[i].print();
		std::cout << "-------" << std::endl;
	}
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
	Server	server;
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

Parser::Config const	&Parser::get_config() const {
	return (_config);
}
