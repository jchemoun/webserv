/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/24 18:01:33 by mjacq             #+#    #+#             */
/*   Updated: 2022/04/25 18:37:23 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <iostream>

static void print_vector(const std::vector<std::string> &v) {
	for (size_t i = 0; i < v.size(); i++) std::cout << v[i] << " ";
	std::cout << std::endl;
}

static void print_map(const std::map<int, std::string> &m, std::string indent = "") {
	std::map<int, std::string>::const_iterator	it = m.begin();
	while (it != m.end()) {
		std::cout << indent << "Error page " << (*it).first << ": " << (*it).second << std::endl;
		++it;
	}
}

void	Config::Location::print() const {
	std::cout << "\e[34mLocation: " << location_path << std::endl;
	std::cout << "    Root: " << root << std::endl;
	print_map(error_pages, "    ");
	std::cout << "\e[0m";
}

void	Config::Server::print() const {
	std::cout << "Server names: "; print_vector(server_names);
	std::cout << "Listening port: " << listen << std::endl;
	for (size_t i = 0; i < locations.size(); i++)
		locations[i].print();
	std::cout << "Indexes: "; print_vector(index);
	std::cout << "Root: " << root << std::endl;
	print_map(error_pages);
}

void	Config::print() const {
	std::cout << "> CONFIG:" << std::endl << std::endl;
		std::cout << "-------" << std::endl;
	for (size_t i = 0; i < servers.size(); ++i) {
		servers[i].print();
		std::cout << "-------" << std::endl;
	}
}
