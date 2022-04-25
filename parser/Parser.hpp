/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 20:15:39 by mjacq             #+#    #+#             */
/*   Updated: 2022/04/25 13:37:28 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include "Lexer.hpp"
# include "Config.hpp"
# include <string>
# include <map>


class Parser {
public:
	// CANONICAL FORM
	Parser(std::string filename);
	virtual ~Parser(void);

	Config const &get_config() const;

	class	ParsingError: public std::runtime_error {
	public:
		ParsingError(std::string reason = "");
		ParsingError(const ParsingError &);
		virtual ~ParsingError() throw();
	private:
		ParsingError	&operator=(ParsingError const &copy);
	};

private:
	// CANONICAL FORM
	Parser(void);
	Parser(Parser const &copy);
	Parser	&operator=(Parser const &copy);
	
	// Parsing utils
	Token const &_current_token() const;
	void	_eat(Token::token_type type);
	void	_eat(Token::token_type type, Token::token_value value);
	void	_init_parsers();

	// Parsing functions
	void	_parse_server_block();
	void	_parse_server_name(Config::Server &server);
	void	_parse_listen(Config::Server &server);

	// Get parsing function
	typedef void (Parser::*server_parser)(Config::Server &);
	server_parser	_get_server_parser() const;

	Lexer	_lexer;
	Config	_config;
	std::map<std::string, server_parser> _server_parsers;
};

#endif
