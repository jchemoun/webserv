/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 13:17:02 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/03 07:30:59 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include <stdexcept>

Webserv::Webserv(): epfd(-1), conf(), clients()
{
	
}

void	Webserv::run()
{
	int	nfds;
	bool color = false;

	serv_init();
	if (!epoll_init())
		return ;
	while (1) //change to var to stop ?
	{
		errno = 0;
		nfds = epoll_wait(epfd, events, MAX_EVENTS, TIMEOUT);
		//std::cout << nfds << '\n';
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
			//std::cout << "event = " << events[i].events << "fd = " << events[i].data.fd << '\n';
			//for (client_map::iterator it = clients.begin(); it != clients.end(); it++)
			//{
			//	std::cout << "CLIENT FD" << (*it).first << '\n';
			//}
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
			std::cout << (color ? "\e[34m": "\e[35m") <<  "waiting\n" << "\e[0m";
			color = !color;
			// keep waiting ? time out client ?
		}
	}
	
}

void	Webserv::get_config(int ac, const char **av) {
	if (ac == 1) {
		throw std::runtime_error("Missing config file");
	} else if (ac > 2) {
		throw (std::runtime_error("Too many arguments"));
	} else {
		Parser	parser(av[1]);
		conf = parser.get_config();
	}
}

bool	Webserv::handle_error()
{
	std::cout << "error\n";
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
		delete_client(client_fd);
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
	Response	response(clients[client_fd].request.get_location());
	//std::cout << "insend\n";
	// need to get right server to response, todo after merge of 2 class config
	
	// need to create header, todo after looking at nginx response header && merge of class config

	send(client_fd, response.c_str(), response.size(), 0);
	//std::cout << "sent\n";
	event.data.fd = client_fd;
	event.events = EPOLLIN;
	epoll_ctl(epfd, EPOLL_CTL_MOD, client_fd, &event);

	return (true);
}

bool	Webserv::epoll_init()
{
	if ((epfd = epoll_create(conf.servers.size() + 1)) < 0) // to fix max fd can be more probably not
	{
		std::cerr << "epoll create error\n";
		return (false);
	}
	std::memset((struct epoll_event *) &event, 0, sizeof(event));
	for (serv_vector::const_iterator cit = conf.servers.begin(); cit != conf.servers.end(); cit++)
	{
		Config::Server const &serv = *cit;
		event.data.fd = serv.listen_fd;
		event.events = EPOLLIN;
		if (epoll_ctl(epfd, EPOLL_CTL_ADD, serv.listen_fd, &event) < 0)
		{
			std::cerr << "epoll ctl error\n";
			return (false);
		}
	}
	return (true);
}

void	Webserv::serv_init()
{
	for (serv_vector::iterator it = conf.servers.begin(); it != conf.servers.end(); it++) {
		Config::Server	&serv = *it;
		socket_init(serv);
	}
}

int		Webserv::socket_init(Config::Server &server)
{
	int					listen_fd;
	int					on = 1;
	struct sockaddr_in	address;

	if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw std::runtime_error("failed socket creation");
	server.listen_fd = listen_fd;
	if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &on, sizeof(int)) < 0)
		throw std::runtime_error("failed socket options");
	std::memset((char *)&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(server.listen_address);
	address.sin_port = htons(server.listen_port);
	if (bind(listen_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
		throw std::runtime_error("failed socket bind");
	if (listen(listen_fd, MAX_CLIENTS) < 0)
		throw std::runtime_error("failed socket listen");
	return (listen_fd); // unused
}

// void	Webserv::conf_init()
// {
	// conf.push_back(Config(INADDR_ANY, 8080));
// }

void	Webserv::delete_client(int client_fd)
{
	clients.erase(client_fd);
	epoll_ctl(epfd, EPOLL_CTL_DEL, client_fd, NULL);
	close(client_fd);
}

bool	Webserv::is_serv(int fd)
{
	for (serv_vector::const_iterator cit = conf.servers.begin(); cit != conf.servers.end(); cit++)
	{
		Config::Server const &serv = *cit;
		if (serv.listen_fd == fd)
			return (true);
	}
	return (false);
}

// void	Webserv::close_serv()
// {
// }

Webserv::~Webserv()
{
	for (serv_vector::const_iterator cit = conf.servers.begin(); cit != conf.servers.end(); cit++)
	{
		Config::Server const &serv = *cit;
		if (serv.listen_fd > 0)
			close(serv.listen_fd);
	}
	if (epfd != -1)
		close(epfd);
}