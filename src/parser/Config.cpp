/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/24 18:01:33 by mjacq             #+#    #+#             */
/*   Updated: 2022/05/24 16:26:09 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <cstddef>
#include <iostream>
#include <arpa/inet.h> // INADDR_ANY
#include <netinet/in.h> // inet_ntop
#include <limits>
#include <cstring> // strncmp
#include "../utils/file.hpp"

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

Config::Location::Location():
	type(type_prefix),
	autoindex(false)
{ }

static bool    match(const char *s1, const char *s2) {
    // if (!s1 || !s2) // never true in our case
    //     return (false);
    if (!*s1 && !*s2)
        return (true);
    if (*s1 && *s2 != '*' && *s1 == *s2)
        return (match(s1 + 1, s2 + 1));
    if (*s1 == '*' && *s2 == '\\' && s2[1] == '*')
        return (match(s1 + 1, s2 + 2));
    if (*s1 && *s2 == '*')
        return (match(s1, s2 + 1) || match(s1 + 1, s2));
    if (!*s1 && *s2 == '*')
        return (match(s1, s2 + 1));
    return (false);
}
bool	Config::Location::match(std::string const &uri) const {
	if (type == type_match)
		return (::match(uri.c_str(), location_path.c_str()));
	else // prefix
		return (!strncmp(location_path.c_str(), uri.c_str(), location_path.size()));
}

void	Config::Location::print() const {
	std::cout << "\e[34mLocation: " << location_path << std::endl;
	std::cout << "    Indexes: "; print_vector(index);
	std::cout << "    Root: " << root << std::endl;
	std::cout << "    Autoindex: " << std::boolalpha << autoindex << std::endl;
	print_map(error_pages, "    Error page ");
	std::cout << "    Allow methods: "; print_vector(allow_methods);
	std::cout << "\e[0m";
}

/*
** =============================== Connection =============================== **
*/

Config::Connection::Connection():
	port(80),
	addr(htonl(INADDR_ANY)),
	str_addr("0.0.0.0"),
	fd(0)
{ }

static std::string	get_printable_address(sockaddr_in  const &sockaddress) {
	char	addr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(sockaddress.sin_addr), addr, INET_ADDRSTRLEN);
	return (addr);
}

Config::Connection	&Config::Connection::operator=(const sockaddr_in &sockaddress) {
	addr     = ntohl(sockaddress.sin_addr.s_addr);
	port     = ntohs(sockaddress.sin_port);
	str_addr = get_printable_address(sockaddress);
	return (*this);
}

/*
** ================================= Server ================================= **
*/

const Config::Server::body_size Config::Server::_overflow_body_size = std::numeric_limits<int>::max();

Config::Server::Server():
	listen_vect(),
	root("html"),
	autoindex(false),
	default_mime("text/plain"),
	client_max_body_size(1024 * 1024), // 1MB
	mime_map(NULL)
{ }

void	Config::Server::set_defaults() {
	if (listen_vect.empty())
		listen_vect.push_back(Connection());
	if (index.empty())
		index.push_back("index.html");
	if (allow_methods.empty()) {
		allow_methods.push_back("GET");
		allow_methods.push_back("POST");
	}
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
	std::cout << "Default mime-type: " << default_mime << std::endl;
	std::cout << "Max client body size: " << client_max_body_size << std::endl;
	print_map(error_pages, "Error page ");
	std::cout << "Allow methods: "; print_vector(allow_methods);
}

std::string	Config::Server::get_mime(std::string const &filename) const{
	std::string	extension = file::get_extension(filename);
	std::string mime_type = default_mime;
	if (!extension.empty()) {
		Config::MimeMap::const_iterator	it = mime_map->find(extension);
		if (it != mime_map->end())
			mime_type = it->second;
	}
	return (mime_type);
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
