/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchemoun <jchemoun@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 13:17:02 by jchemoun          #+#    #+#             */
/*   Updated: 2022/04/22 14:35:47 by jchemoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

Webserv::Webserv(): epfd(-1), serv(), conf()
{
	
}

void	Webserv::run()
{
	int	nfds;

	if (!serv_init())
		return ;
	if (!epoll_init())
		return ;
	while (1) //change to var to stop ?
	{
		nfds = epoll_wait(epfd, events, MAX_EVENTS, TIMEOUT);
		//if (error on epoll) //use errno ?
		for (size_t i = 0; i < nfds; i++)
		{
			//if else if bloc
			/*	if (error) 
					handle error // quelles erreures ? comment gérer ?
				else if (nouvelle connection events[i])
					accept fonction (nouveau client) // db de clients ?
				else if (read events[i])
					revc fonction (request)
				else if (write events[i])
					send fonction (response to request)
			*/
		}
		if (nfds == 0)
		{
			// keep waiting ? time out client ?
		}
	}
	
}

bool	Webserv::epoll_init()
{
	if ((epfd = epoll_create(serv.size())) < 0)
	{
		return (false);
	}
	std::memset((struct epoll_event *) &event, 0, sizeof(event));
	for (serv_vector::const_iterator cit = serv.begin(); cit != serv.end(); cit++)
	{
		event.data.fd = *cit;
		event.events = EPOLLIN;
		if (epoll_ctl(epfd, EPOLL_CTL_ADD, *cit, &event) < 0)
		{
			return (false);
		}
	}
	return (true);
}

bool	Webserv::serv_init()
{
	int	tmp;
	for (conf_vector::const_iterator cit = conf.begin(); cit != conf.end(); cit++)
	{
		tmp = socket_init(*cit);
		if (tmp == -1)
			return (false);
		serv.push_back(tmp);
	}
	
}

int		Webserv::socket_init(Config conf)
{
	int					listen_fd;
	int					on;
	int					port;
	struct sockaddr_in	address;

	(void)conf;
	on = 1;
	port = 8080;
	if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cerr << "failed socket creation" << std::endl;
		close_serv();
		return (-1);
	}
	if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &on, sizeof(int)) < 0)
	{
		std::cerr << "failed socket options" << std::endl;
		close_serv();
		return (-1);
	}
	std::memset((char *)&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htons(INADDR_ANY);	//change to conf addr
	address.sin_port = htons(port);					//change to conf port;
	if (bind(listen_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		std::cerr << "failed socket bind" << std::endl;
		close_serv();
		return (-1);
	}
	if (listen(listen_fd, MAX_CLIENTS) < 0)
	{
		std::cerr << "failed socket listen" << std::endl;
		close_serv();
		return (-1);
	}
	return (listen_fd);
}

void	Webserv::close_serv()
{
	for (serv_vector::const_iterator cit = serv.begin(); cit != serv.end(); cit++)
	{
		close(*cit);
	}
	if (epfd != -1)
		close(epfd);
}

Webserv::~Webserv()
{
	
}