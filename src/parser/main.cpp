/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 16:05:35 by mjacq             #+#    #+#             */
/*   Updated: 2022/05/10 18:02:49 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// PARSING TEST FILE

#include "Token.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include <iostream>

int	main() {

	std::cout << "===== LEXER =====" << std::endl << std::endl;
	Lexer	lexer("basic_server.conf");
	lexer.print();

	std::cout << "===== PARSER =====" << std::endl << std::endl;
	try {
		Parser	parser("basic_server.conf", "mime.types");
		parser.get_config().print();
	}
	catch (std::runtime_error const &err) {
		std::cerr << err.what() << std::endl;
	}
	// catch (Parser::ParsingError const &err) {
	// 	std::cerr << err.what() << std::endl;
	// }

	return (0);
}
