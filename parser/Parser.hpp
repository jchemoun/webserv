/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 20:15:39 by mjacq             #+#    #+#             */
/*   Updated: 2022/04/24 18:01:00 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include "Lexer.hpp"
# include "Config.hpp"
# include <string>


class Parser {
public:
	// CANONICAL FORM
	Parser(std::string filename);
	virtual ~Parser(void);

	Config const &get_config() const;

private:
	// CANONICAL FORM
	Parser(void);
	Parser(Parser const &copy);
	Parser	&operator=(Parser const &copy);
	
	// Parsing utils
	Token const &_current_token() const;
	void	_eat(Token::token_type type);
	void	_eat(Token::token_type type, Token::token_value value);

	// Parsing functions
	void	_parse_server_block();

	Lexer	_lexer;
	Config	_config;
};

#endif
