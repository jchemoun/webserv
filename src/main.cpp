/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/04 11:30:24 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/14 12:34:09 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include <iostream>
#include <exception>
#include "color.hpp"

int main(int ac, const char **av)
{
	try {
          Webserv wb;
		  wb.get_config(ac, av);
          wb.run();
		  std::cout << std::endl << color::green << "Stopping gracefully. Bye 👋" << color::reset << std::endl;
        }
	catch (std::exception const &except) {
		std::cerr << except.what() << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
