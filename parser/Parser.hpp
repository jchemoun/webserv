/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 20:15:39 by mjacq             #+#    #+#             */
/*   Updated: 2022/04/24 17:38:17 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include "Lexer.hpp"
# include <string>

// # include "Server.hpp"

class Parser {
public:
	// CANONICAL FORM
	Parser(std::string filename);
	virtual ~Parser(void);

	struct Location {
		void print() const;
		std::string	location_path;
		std::string	root;
	};
	struct	Server {
		void print() const;
		std::vector<std::string>	server_names;
		int	listen;
		std::vector<Location>	locations;
	};
	struct Config {
		void print() const;
		std::vector<Server>	servers;
	};

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
