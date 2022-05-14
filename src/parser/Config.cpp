/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/24 18:01:33 by mjacq             #+#    #+#             */
/*   Updated: 2022/05/14 12:35:32 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <cstddef>
#include <iostream>
#include <arpa/inet.h> // INADDR_ANY
#include <limits>

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

Config::Connection::Connection():
	port(80),
	addr(htonl(INADDR_ANY)),
	str_addr("*"),
	fd(0)
{ }

/*
** ================================= Server ================================= **
*/

const Config::Server::body_size Config::Server::_overflow_body_size = std::numeric_limits<int>::max();

Config::Server::Server():
	listen_vect(),
	root("html"),
	autoindex(false),
	default_type("text/plain"),
	client_max_body_size(1024 * 1024), // 1MB
	mime_types(NULL)
{ }

void	Config::Server::set_defaults() {
	if (listen_vect.empty())
		listen_vect.push_back(Connection());
	if (index.empty())
		index.push_back("index.html");
}

void	Config::Server::print() const {
	std::cout << "Server names: "; print_vector(server_names);
	for (size_t i = 0; i < listen_vect.size(); ++i) {
		std::cout << "Listening port: \e[35m" << listen_vect[i].port << "\e[0m" << std::endl;
		std::cout << "Listening address: " << listen_vect[i].str_addr << std::endl;
	}
	for (size_t i = 0; i < locations.size(); i++)
		locations[i].print();
	std::cout << "Indexes: "; print_vector(index);
	std::cout << "Root: " << root << std::endl;
	std::cout << "Autoindex: " << std::boolalpha << autoindex << std::endl;
	std::cout << "Default type: " << default_type << std::endl;
	std::cout << "Max client body size: " << client_max_body_size << std::endl;
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
	std::cout << "-------" << std::endl;
	for (size_t i = 0; i < servers.size(); ++i) {
		servers[i].print();
		std::cout << "-------" << std::endl;
	}
}
