/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Token.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 16:11:46 by mjacq             #+#    #+#             */
/*   Updated: 2022/04/22 19:16:07 by mjacq            ###   ########.fr       */
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
