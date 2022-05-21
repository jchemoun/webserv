/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/26 12:19:18 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/21 12:58:29 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <unistd.h>
#include <cstring> // bzero
#include "color.hpp"

Client::Client(Config::Connection const *listen_info):
	listen_info(listen_info), response(NULL)
{
	request.listen_info = listen_info;
}

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
