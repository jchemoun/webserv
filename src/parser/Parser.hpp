/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 20:15:39 by mjacq             #+#    #+#             */
/*   Updated: 2022/05/13 10:02:05 by mjacq            ###   ########.fr       */
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
	Parser(std::string filename, std::string mimefile = "conf/mime.types");
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
	const char	*_parse_address(Config::Listen &listen, const char *s);  // part of _parse_listen
	void	_parse_default_type(Config::Server &server);                 // http://nginx.org/en/docs/http/ngx_http_core_module.html#default_type
	template <class Context>
	void	_parse_index(Context &context);                              // http://nginx.org/en/docs/http/ngx_http_index_module.html#index
	void	_parse_location(Config::Server &server);                     // http://nginx.org/en/docs/http/ngx_http_core_module.html#location
	template <class Context>
	void	_parse_root(Context &context);                               // http://nginx.org/en/docs/http/ngx_http_core_module.html#root
	template <class Context>
	void	_parse_error_page(Context &context);                         // http://nginx.org/en/docs/http/ngx_http_core_module.html#error_page
	template <class Context>
	void	_parse_autoindex(Context &context);                          // http://nginx.org/en/docs/http/ngx_http_core_module.html#error_page
	void	_parse_client_max_body_size(Config::Server	&server);        // http://nginx.org/en/docs/http/ngx_http_core_module.html#client_max_body_size

	// Parse types
	void	_parse_types();
	void	_parse_type_line();
	void	_set_default_mime_types();

	// Get parsing function
	template <class parser_type>
	parser_type	_get_directive_parser(std::map<std::string, parser_type> &parsers) const;

	Lexer	_lexer;
	Config	_config;

	typedef void (Parser::*server_parser)(Config::Server &);
	std::map<std::string, server_parser>	_server_parsers;
	typedef void (Parser::*location_parser)(Config::Location &);
	std::map<std::string, location_parser>	_location_parsers;

public:
	// Static Utils
	static int	_stoi(std::string const &s,
			int min = std::numeric_limits<int>::min(), int max = std::numeric_limits<int>::max());
	static bool _is_a_number(const char *s);
};

#endif
