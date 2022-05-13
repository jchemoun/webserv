/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/26 12:19:18 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/13 22:49:13 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <netinet/in.h> // inet_ntop
#include <unistd.h>
#include <cstring> // bzero

Client::Client(): serv_fd(-1)
{ }

Client::~Client()
{ }

bool	Client::accept_connection(int listen_fd) {
	serv_fd = listen_fd;
	sockaddr_in	sockaddress;
	socklen_t	socklen = sizeof sockaddress;
	if ((connection.fd = accept(listen_fd, (struct sockaddr *)&sockaddress, &socklen)) < 0) {
		std::cerr << "error accept\n";
		return (false);
	}
	connection.addr = ntohl(sockaddress.sin_addr.s_addr);
	connection.port = ntohs(sockaddress.sin_port); // this is not the server port, but the client port

	char	addr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(sockaddress.sin_addr), addr, INET_ADDRSTRLEN);
	connection.str_addr = addr;
	std::cout << "New connection by client: @" << connection.str_addr << ":" << connection.port << "\n";
	return (true);
}

void	Client::close_connection() {
	if (connection.fd > 0) {
		close(connection.fd);
		connection.fd = -1;
	}
}

Config::Server	&Client::resolve_server(ServerMap &serverMap, DefaultServerMap &default_server_map) const
{
	NameToServMap	&name_to_serv_map = serverMap.at(serv_fd);
	Request::Header::const_iterator	host_match = request.get_header().find("Host");
	if (host_match != request.get_header().end()) { // else missing Host with multiple servers on same fd
		std::string	server_name = host_match->second.substr(0, host_match->second.find(':'));
		NameToServMap::iterator server_match = name_to_serv_map.find(server_name);
		if (server_match != name_to_serv_map.end()) // else 'Host' not found
			return (*server_match->second);
	}
	return (*default_server_map.at(serv_fd));
}
