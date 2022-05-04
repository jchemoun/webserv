/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/04 11:30:24 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/04 09:42:32 by mjacq            ###   ########.fr       */
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
		  std::cout << std::endl << "\e[32mStopping gracefully. Bye 👋\e[0m" << std::endl;
        }
	catch (std::exception const &except) {
		std::cerr << except.what() << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
