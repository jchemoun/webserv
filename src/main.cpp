/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchemoun <jchemoun@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/04 11:30:24 by jchemoun          #+#    #+#             */
/*   Updated: 2022/04/26 13:18:16 by jchemoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include <iostream>
#include <exception>

int main(int ac, const char **av)
{
	try {
          Webserv wb;
		  wb.get_config(ac, av);
          wb.run();
        }
	catch (std::exception const &except) {
		std::cerr << except.what() << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
