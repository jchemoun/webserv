/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/26 12:19:18 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/13 23:41:28 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <netinet/in.h> // inet_ntop
#include <unistd.h>
#include <cstring> // bzero

Client::Client(): _listen_fd(-1)
{ }

Client::~Client()
{ }

static std::string	get_printable_address(sockaddr_in &sockaddress) {
	char	addr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(sockaddress.sin_addr), addr, INET_ADDRSTRLEN);
	return (addr);
}

bool	Client::accept_connection(int listen_fd) {
	_listen_fd = listen_fd;
	sockaddr_in	sockaddress;
	socklen_t	socklen = sizeof sockaddress;
	if ((connection.fd = accept(listen_fd, (struct sockaddr *)&sockaddress, &socklen)) < 0) {
		std::cerr << "error accept\n";
		return (false);
	}
	connection.addr     = ntohl(sockaddress.sin_addr.s_addr);
	connection.port     = ntohs(sockaddress.sin_port); // this is not the server port, but the client port
	connection.str_addr = get_printable_address(sockaddress);
	_print_connection_info();
	return (true);
}

void	Client::_print_connection_info() {
	struct sockaddr_in sin;
	socklen_t len = sizeof(sin);
	if (getsockname(_listen_fd, (struct sockaddr *)&sin, &len) == 0)
	std::cout << "\e[1;32mNew connection accepted\e[0m on " << get_printable_address(sin) << ":\e[1;32m" << ntohs(sin.sin_port)
		<< "\e[0m by client: @" << connection.str_addr << ":" << connection.port
		<< " (listen_fd: " << _listen_fd << ", connection_fd: " << connection.fd << ")\n";
}

void	Client::close_connection() {
	if (connection.fd > 0) {
		close(connection.fd);
		connection.fd = -1;
	}
}

Config::Server	&Client::resolve_server(ServerMap &serverMap, DefaultServerMap &default_server_map) const
{
	NameToServMap	&name_to_serv_map = serverMap.at(_listen_fd);
	Request::Header::const_iterator	host_match = request.get_header().find("Host");
	if (host_match != request.get_header().end()) { // else missing Host with multiple servers on same fd
		std::string	server_name = host_match->second.substr(0, host_match->second.find(':'));
		NameToServMap::iterator server_match = name_to_serv_map.find(server_name);
		if (server_match != name_to_serv_map.end()) // else 'Host' not found
			return (*server_match->second);
	}
	return (*default_server_map.at(_listen_fd));
}
