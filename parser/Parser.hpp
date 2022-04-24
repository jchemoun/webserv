/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 20:15:39 by mjacq             #+#    #+#             */
/*   Updated: 2022/04/24 16:06:20 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include "Lexer.hpp"
# include <string>

// # include "Server.hpp"
//
// syntax:
// config = block*
// block = word { (instruction | block)* }
// instruction = word*;

class Parser {
public:
	// CANONICAL FORM
	Parser(std::string filename);
	virtual ~Parser(void);

private:
	// CANONICAL FORM
	Parser(void);
	Parser(Parser const &copy);
	Parser	&operator=(Parser const &copy);

	Lexer	_lexer;
};

#endif
