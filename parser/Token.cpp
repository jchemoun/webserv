/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Token.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 16:11:46 by mjacq             #+#    #+#             */
/*   Updated: 2022/04/24 17:16:15 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Token.hpp"

Token::Token(void): _type(0), _value("") {
}

Token::Token(Token const &copy): _type(copy._type), _value(copy._value) {
}

Token::Token(token_type type, token_value value): _type(type), _value(value) {
}

Token	&Token::operator=(Token const &copy) {
	if (&copy == this)
		return (*this);
	_type = copy._type;
	_value = copy._value;
	return (*this);
}

Token::~Token(void) {
}

Token::token_type const	&Token::get_type() const {
	return (_type);
}

Token::token_value const &Token::get_value() const {
	return (_value);
}

bool	Token::expect(token_type type) const {
	return (type == _type);
}

bool	Token::expect(token_type type, token_value value) const {
	return (type == _type && value == _value);
}
