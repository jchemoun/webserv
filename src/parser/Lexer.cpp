/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 18:01:10 by mjacq             #+#    #+#             */
/*   Updated: 2022/05/10 17:08:25 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Lexer.hpp"
#include <stdexcept>
#include <iostream>
#include <cstring>

/*
** ======================== Constructor / Destructor ======================== **
*/

Lexer::Lexer(std::string filename):
	_line(), _pos(0), _vect(), _istream(), _token_pos(0) {
	tokenize_file(filename);
}

Lexer::~Lexer(void) {
	_istream.close();
}

/*
** ============================= Public methods ============================= **
*/

void	Lexer::print() const{
	for (size_t i = 0; i < _vect.size(); i++)
		std::cout << _vect.at(i).get_value() << std::endl;
}

size_t	Lexer::size() const {
	return (_vect.size());
}

Token const	&Lexer::next() {
	return (_vect.at(++_token_pos));
}

Token const	&Lexer::peek() const {
	return (_vect.at(_token_pos));
}

Token const	&Lexer::peek_next() const {
	return (_vect.at(_token_pos + 1));
}

/*
** ============================ Private methods ============================= **
*/

void	Lexer::_clear() {
	_line.clear();
	_pos = 0;
	_vect.clear();
	_istream.close();
	_token_pos = 0;
}

void	Lexer::_open(std::string &filename) {
	_istream.open(filename.c_str()); // Calls setstate(failbit) on failure.
	if (!_istream)
		throw std::runtime_error("Fails to open file.");
}

void	Lexer::tokenize_file(std::string filename) {
	_clear();
	_open(filename);
	while (std::getline(_istream, _line)) {
		_tokenize_line();
	}
	_vect.push_back(Token(Token::type_eof, ""));
}

void	Lexer::_tokenize_line() {
	_pos = 0;
	char	c;
	while ((c = _line[_pos])) {
		if (_is_skipped_char(c))
			++_pos;
		else if (_is_comment_char(c))
			_skip_comment();
		else if (_is_special_char(c))
			_tokenize_special_char();
		else
			_tokenize_word();
	}
}

void	Lexer::_tokenize_special_char() {
	std::string	value;
	value =_line[_pos];
	_vect.push_back(Token(Token::type_special_char, value));
	++_pos;
}

void	Lexer::_tokenize_word() {
	size_t		start = _pos;
	char		c;
	std::string	strtoken;
	while ((c = _line[_pos])) {
		if (_is_skipped_char(c) || _is_special_char(c))
			break;
		if (c =='\\') {
			if (_line[_pos + 1] == ' ' || _line[_pos + 1] == '\\') {
				strtoken += _line.substr(start, _pos - start) + _line[_pos + 1];
				start = ++_pos + 1;
			}
		}
		++_pos;
	}
	strtoken += _line.substr(start, _pos - start);
	_vect.push_back(Token(Token::type_word, strtoken));
}

const char *Lexer::_special_chars = ";{}#";

bool	Lexer::_is_special_char(char c) {
	return (std::strchr(_special_chars, c));
}

bool	Lexer::_is_skipped_char(char c) {
	return (std::strchr("\t ", c));
}

bool	Lexer::_is_comment_char(char c) {
	return (c == '#');
}

void	Lexer::_skip_comment() {
	_pos = _line.size();
}
