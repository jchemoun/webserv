/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 16:05:35 by mjacq             #+#    #+#             */
/*   Updated: 2022/04/22 19:19:16 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// PARSING TEST FILE

#include "Token.hpp"
#include "Lexer.hpp"
#include <iostream>

int	main() {
	Lexer	lexer("basic_server.conf");
	lexer.print();
	return (0);
}
