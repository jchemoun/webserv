/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchemoun <jchemoun@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/04 11:30:24 by jchemoun          #+#    #+#             */
/*   Updated: 2022/04/25 13:05:32 by jchemoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

int main(void)
{
	int					server_fd;
	int					new_socket;
	struct sockaddr_in	address;
	struct sockaddr_in	address_client;
	socklen_t			remote_sinlen = sizeof(address);
	const int			port = 8080;
	int					on;
	char				buffer[3000] = {0};
	std::string			output = "Hello world!";
	long				read_len;

	on = 1;
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cerr << "failed socket creation" << std::endl;
		return (1);
	}
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &on, sizeof(int)) < 0)
	{
		close(server_fd);
		std::cerr << "failed socket options" << std::endl;
		return (1);
	}
	std::memset((char *)&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htons(INADDR_ANY);
	address.sin_port = htons(port);
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		close (server_fd);
		std::cerr << "failed socket bind" << std::endl;
		return (1);
	}
	if (listen(server_fd, 3) < 0)
	{
		close (server_fd);
		std::cerr << "failed socket listen" << std::endl;
		return (1);
	}
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address_client, &remote_sinlen)) < 0)
	{
		close (server_fd);
		std::cerr << "failed socket accept" << std::endl;
		return (1);
	}
	read_len = read(new_socket, buffer, 3000);
	std::cout << read_len << '\n' << buffer << '\n';
	write(new_socket, output.c_str(), output.length());
	std::cout << ntohs(address.sin_addr.s_addr) << std::endl;
	//struct in_addr host;
	//(void)host;
	close(server_fd);
	Webserv wb;
	wb.run();
	return (0);
}
