/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/26 12:19:18 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/16 18:44:03 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <unistd.h>
#include <cstring> // bzero
#include "color.hpp"
#include "utils.hpp"

Client::Client(Config::Connection const *listen_info):
	listen_info(listen_info),
	current_server(NULL)
{ }

Client::~Client()
{ }

bool	Client::accept_connection() {
	sockaddr_in	sockaddress;
	socklen_t	socklen = sizeof sockaddress;
	if ((accept_info.fd = accept(listen_info->fd, (struct sockaddr *)&sockaddress, &socklen)) < 0) {
		std::cerr << "error accept\n";
		return (false);
	}
	accept_info = sockaddress;
	_print_connection_info();
	return (true);
}

void	Client::_print_connection_info() {
	std::cout
		<< color::bold << color::green << "New connection accepted" << color::reset
		<< " on " << listen_info->str_addr << ":" << color::bold << color::green << listen_info->port << color::reset
		<< " by client: @" << accept_info.str_addr << ":" << accept_info.port
		<< " (listen_fd: " << listen_info->fd << ", connection_fd: " << accept_info.fd << ")\n";
}

void	Client::close_connection() {
	if (accept_info.fd > 0) {
		close(accept_info.fd);
		accept_info.fd = -1;
	}
}

/*
** @brief set the current server and the current server name according to the header Host
**
** Fallback to the default server in either cases:
** - Host header not found
** - Host header not matching with any server listening on the current listen fd
*/
void	Client::resolve_server(ServerMap &serverMap, DefaultServerMap &default_server_map)
{
	std::string const	*host = utils::get(request.get_header(), std::string("Host"));

	if (host)
	{
		std::string		server_name       = host->substr(0, host->find(':'));
		NameToServMap	&name_to_serv_map = serverMap.at(listen_info->fd);
		Config::Server	**server          = utils::get(name_to_serv_map, server_name);

		if (server)
		{
			current_server      = *server;
			current_server_name = server_name;

			std::cout << "Found server name matching Host: "
				<< color::bold << color::green << server_name << color::reset << "\n";
			return ;
		}
	}
	current_server      = default_server_map.at(listen_info->fd);
	current_server_name = (current_server->server_names.empty() ? "" : current_server->server_names[0]);

	std::cout << "No server found where name is matching Host. Fallback to default server "
		<< color::bold << color::yellow << current_server_name << color::reset << "\n";
}
