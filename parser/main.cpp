/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 16:05:35 by mjacq             #+#    #+#             */
/*   Updated: 2022/04/24 17:52:28 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// PARSING TEST FILE

#include "Token.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include <iostream>

int	main() {

	std::cout << "===== LEXER =====" << std::endl;
	Lexer	lexer("basic_server.conf");
	lexer.print();

	std::cout << "===== PARSER =====" << std::endl;
	try {
		Parser	parser("basic_server.conf");
		parser.get_config().print();
	}
	catch (std::runtime_error const &err) {
		std::cerr << err.what() << std::endl;
	}

	return (0);
}
