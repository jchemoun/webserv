/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/24 18:01:33 by mjacq             #+#    #+#             */
/*   Updated: 2022/04/24 18:02:28 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <iostream>

void	Config::Location::print() const {
	std::cout << "Location path: " << location_path << std::endl;
	std::cout << "root: " << root << std::endl;
}

void	Config::Server::print() const {
	std::cout << "Server: ";
	for (size_t i = 0; i < server_names.size(); i++)
		std::cout << server_names[i] << " ";
	std::cout << std::endl;
	std::cout << "Listening port: " << listen << std::endl;
	for (size_t i = 0; i < locations.size(); i++)
		locations[i].print();
}

void	Config::print() const {
	std::cout << "> CONFIG:" << std::endl;
	for (size_t i = 0; i < servers.size(); ++i) {
		servers[i].print();
		std::cout << "-------" << std::endl;
	}
}

