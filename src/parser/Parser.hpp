/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 20:15:39 by mjacq             #+#    #+#             */
/*   Updated: 2022/04/28 17:50:40 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include "Lexer.hpp"
# include "Config.hpp"
# include <string>
# include <map>
# include <limits>


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
	void	_parse_server();                                             // http://nginx.org/en/docs/http/ngx_http_core_module.html#server
	void	_parse_server_name(Config::Server &server);                  // http://nginx.org/en/docs/http/ngx_http_core_module.html#server_name
	void	_parse_listen(Config::Server &server);                       // http://nginx.org/en/docs/http/ngx_http_core_module.html#listen
	const char	*_parse_address(Config::Server &server, const char *s);  // part of _parse_listen
	template <class Context>
	void	_parse_index(Context &context);                              // http://nginx.org/en/docs/http/ngx_http_index_module.html#index
	void	_parse_location(Config::Server &server);                     // http://nginx.org/en/docs/http/ngx_http_core_module.html#location
	template <class Context>
	void	_parse_root(Context &context);                               // http://nginx.org/en/docs/http/ngx_http_core_module.html#root
	template <class Context>
	void	_parse_error_page(Context &context);                         // http://nginx.org/en/docs/http/ngx_http_core_module.html#error_page

	// Get parsing function
	template <class parser_type>
	parser_type	_get_directive_parser(std::map<std::string, parser_type> &parsers) const;

	Lexer	_lexer;
	Config	_config;

	typedef void (Parser::*server_parser)(Config::Server &);
	std::map<std::string, server_parser>	_server_parsers;
	typedef void (Parser::*location_parser)(Config::Location &);
	std::map<std::string, location_parser>	_location_parsers;

	// Static Utils
	static int	stoi(std::string const &s,
			int min = std::numeric_limits<int>::min(), int max = std::numeric_limits<int>::max());
};

#endif
