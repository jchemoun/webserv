/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchemoun <jchemoun@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 13:17:02 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/01 13:04:57 by jchemoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

Webserv::Webserv(): epfd(-1), serv(), conf(), clients()
{
	
}

void	Webserv::run()
{
	int	nfds;

	conf_init();
	if (!serv_init())
		return ;
	if (!epoll_init())
		return ;
	while (1) //change to var to stop ?
	{
		errno = 0;
		nfds = epoll_wait(epfd, events, MAX_EVENTS, TIMEOUT);
		if (errno == EINVAL || errno == EFAULT || errno == EBADFD)
			std::cerr << "epoll error " << strerror(errno) << '\n'; //use errno ?
		for (int i = 0; i < nfds; i++)
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
			if (events[i].events == EPOLLERR || events[i].events == EPOLLHUP)
				handle_error();
			else if (events[i].events == EPOLLIN && is_serv(events[i].data.fd))
				handle_new_client(events[i].data.fd);
			else if (events[i].events == EPOLLIN)
				handle_recv(events[i].data.fd);
			else if (events[i].events == EPOLLOUT)
				handle_send(events[i].data.fd);
			//std::cout << "fgh" << (events[i].events) << EPOLLOUT << '\n';
		}
		if (nfds == 0)
		{
			std::cout << "waiting\n";
			// keep waiting ? time out client ?
		}
	}
	
}

bool	Webserv::handle_error()
{
	return (true);
}

bool	Webserv::handle_new_client(int serv_fd)
{
	Client	client;
	int		client_fd;
	
	client_fd = -1;
	if ((client_fd = accept(serv_fd, NULL, NULL)) < 0)
	{
		std::cerr << "error accept\n";
		return (false);
	}
	event.data.fd = client_fd;
	event.events = EPOLLIN;
	epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &event);
	clients[client_fd] = client;
	std::cout << "connection worked\n";
	return (true);
}

bool	Webserv::handle_recv(int client_fd)
{
	char	buffer[BUFFER_SIZE] = {0};
	ssize_t	len;

	len = recv(client_fd, buffer, BUFFER_SIZE, 0); // maybe while read || up buffer_size
	if (len == -1)
	{
		std::cerr << "error recv\n";
		return (false);
	}
	else if (len == 0)
	{
		//connection close don't know what to do exactly;
		return (true);
	}
	else
	{
		//parse request && identify what client wants
		//std::cout << len << '\n' << buffer << '\n';
		clients[client_fd].request.append_unparsed_request(buffer, len);
		clients[client_fd].request.parse_request();
		//if response needed set client to epollout
		event.data.fd = client_fd;
		event.events = EPOLLOUT;
		epoll_ctl(epfd, EPOLL_CTL_MOD, client_fd, &event);
	}
	std::cout << "inrecv\n";
	
	return (true);
}

bool	Webserv::handle_send(int client_fd)
{
	// for now response is here, could be in client
	Response	response;
	//std::cout << "insend\n";
	// need to get right server to response, todo after merge of 2 class config
	
	// need to create header, todo after looking at nginx response header && merge of class config

	response.read_file(clients[client_fd].request.get_location());
	response.set_full_response();
	send(client_fd, response.get_full_response().c_str(), response.get_len(), 0);
	
	event.data.fd = client_fd;
	event.events = EPOLLIN;
	epoll_ctl(epfd, EPOLL_CTL_MOD, client_fd, &event);

	return (true);
}

bool	Webserv::epoll_init()
{
	if ((epfd = epoll_create(serv.size() + 1)) < 0) // to fix max fd can be more probably not
	{
		std::cerr << "epoll create error\n";
		return (false);
	}
	std::memset((struct epoll_event *) &event, 0, sizeof(event));
	for (serv_vector::const_iterator cit = serv.begin(); cit != serv.end(); cit++)
	{
		event.data.fd = *cit;
		event.events = EPOLLIN;
		if (epoll_ctl(epfd, EPOLL_CTL_ADD, *cit, &event) < 0)
		{
			std::cerr << "epoll ctl error\n";
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
	return (true);
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

void	Webserv::conf_init()
{
	conf.push_back(Config(INADDR_ANY, 8080));
}

bool	Webserv::is_serv(int fd)
{
	for (serv_vector::const_iterator cit = serv.begin(); cit != serv.end(); cit++)
	{
		if (*cit == fd)
			return (true);
	}
	return (false);
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