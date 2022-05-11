/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/24 18:01:33 by mjacq             #+#    #+#             */
/*   Updated: 2022/05/11 09:44:22 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <iostream>
#include <arpa/inet.h> // INADDR_ANY

/*
** ================================= Utils ================================== **
*/

static void print_vector(const std::vector<std::string> &v) {
	for (size_t i = 0; i < v.size(); i++) std::cout << v[i] << " ";
	std::cout << std::endl;
}

template <class Key, class Value>
static void print_map(const std::map<Key, Value> &m, std::string indent = "") {
	typename std::map<Key, Value>::const_iterator	it = m.begin();
	while (it != m.end()) {
		std::cout << indent << (*it).first << ": " << (*it).second << std::endl;
		++it;
	}
}

/*
** ================================ Location ================================ **
*/

Config::Location::Location(): autoindex(false) { // autoindex should be unset
}

void	Config::Location::print() const {
	std::cout << "\e[34mLocation: " << location_path << std::endl;
	std::cout << "    Indexes: "; print_vector(index);
	std::cout << "    Root: " << root << std::endl;
	std::cout << "    Autoindex: " << std::boolalpha << autoindex << std::endl;
	print_map(error_pages, "    Error page ");
	std::cout << "\e[0m";
}

/*
** ================================= Server ================================= **
*/

Config::Server::Server():
	listen_port(0),
	listen_address(htonl(INADDR_ANY)),
	listen_string_address("*"),
	listen_fd(0),
	root("html"),
	autoindex(false),
	default_type("text/plain")
{
}

void	Config::Server::set_defaults() {
	if (!listen_port)
		listen_port = 80;
	if (index.empty())
		index.push_back("index.html");
}

void	Config::Server::print() const {
	std::cout << "Server names: "; print_vector(server_names);
	std::cout << "Listening port: " << listen_port << std::endl;
	std::cout << "Listening address: " << listen_string_address << std::endl;
	for (size_t i = 0; i < locations.size(); i++)
		locations[i].print();
	std::cout << "Indexes: "; print_vector(index);
	std::cout << "Root: " << root << std::endl;
	std::cout << "Autoindex: " << std::boolalpha << autoindex << std::endl;
	std::cout << "Default type: " << default_type << std::endl;
	print_map(error_pages, "Error page ");
}

/*
** ================================= Config ================================= **
*/

void	Config::set_defaults() {
	for (size_t	i = 0; i < servers.size(); ++i)
		servers[i].set_defaults();
}

void	Config::print() const {
	std::cout << "> CONFIG:" << std::endl << std::endl;
		std::cout << "-------" << std::endl;
	std::cout << "Mime-Types:\n\e[36m"; print_map(types); std::cout << "\e[0m\n";
	for (size_t i = 0; i < servers.size(); ++i) {
		servers[i].print();
		std::cout << "-------" << std::endl;
	}
}
