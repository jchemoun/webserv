/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 13:17:02 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/17 11:35:09 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include "Request.hpp"
#include <signal.h> // handle <c-c>
#include <stdexcept>
#include <sys/epoll.h>
#include "color.hpp"

// Quit gracefully with <c-c>
volatile sig_atomic_t should_run = true;
static void	sigint_handler(int sig) { if (sig == SIGINT) should_run = false; }

Webserv::Webserv(): epfd(-1), conf(), clients()
{
	signal(SIGINT, sigint_handler);
}

void	Webserv::run()
{
	int	nfds;

	serv_init();
	epoll_init();
	while (should_run)
	{
		nfds = epoll_wait();
		for (int i = 0; i < nfds; i++)
		{
			const int event = events[i].events;
			const int event_fd =events[i].data.fd;
			if (event & EPOLLIN) {
				if (is_serv(event_fd))
					handle_new_client(event_fd);
				else
					handle_recv(event_fd);
			}
			else if (event & EPOLLOUT)
				handle_send(event_fd);
			else if (event & EPOLLERR || event & EPOLLHUP)
				handle_event_error();
			else
				std::cerr << "Unknown epoll event: " << event << std::endl;
		}
		if (nfds == 0) { // keep waiting ? time out client ?
		}
	}
}

/*
** @brief Parse the config file (cli argument) and set the conf attribute accordingly
*/
void	Webserv::get_config(int ac, const char **av) {
	if (ac == 1) {
		throw std::runtime_error("Missing config file");
	} else if (ac > 2) {
		throw (std::runtime_error("Too many arguments"));
	} else {
		Parser	parser(av[1]);
		conf = parser.get_config();
		conf.print();
	}
}

bool	Webserv::handle_event_error()
{
	std::cout << "epoll event error (EPOLLERR or EPOLLHUP)\n"; // TODO: better error handling
	return (true);
}

/*
** @brief accept incoming connection,
** initialize the client with that connection socket fd
** and track it in epoll (EPOLLIN mode)
*/
bool	Webserv::handle_new_client(int listen_fd)
{
	Client	client(&connections.at(listen_fd));
	if (!client.accept_connection())
		return (false);
	epoll_add(client.accept_info.fd, EPOLLIN);
	clients[client.accept_info.fd] = client;
	return (true);
}

/*
** @brief recieves BUFFER_SIZE char,
** - if not empty record and parses the request,
**   then set epoll client's socket connection fd to EPOLLOUT
** - otherwise if empty closes connection (delete client)
*/
bool	Webserv::handle_recv(int client_fd)
{
	char	buffer[BUFFER_SIZE] = {0};
	ssize_t	len;

	len = recv(client_fd, buffer, BUFFER_SIZE, 0);            // TODO: check that it is working fine with small BUFFER_SIZE values
	std::cout << color::bold << "> Incoming reception of len " << len << color::reset << " on client fd: " << client_fd << "\n";
	if (len == -1)
	{
		std::cerr << color::red << "error recv\n" << color::reset;               // TODO: better recv error handling
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
		Request	&request = clients[client_fd].request;
		request.append_unparsed_request(buffer, len);
		request.parse_request(server_map, default_server_map);
		if (request.is_complete()) {
			//if response needed set client to epollout
			epoll_mod(client_fd, EPOLLOUT);
		}
	}

	return (true);
}

/*
** @brief set and send the response to the client
** Finally reset the client_fd epoll mode to EPOLLIN
*/
bool	Webserv::handle_send(int client_fd)
{
	Client			&client = clients[client_fd];
	Response		response(client.request);
	// need to get right server to response, todo after merge of 2 class config
	// need to create header, todo after looking at nginx response header && merge of class config

	clients[client_fd].request.reset();
	if (response.is_large_file)
	{
		// todo boucle while read/send
		std::cout << response.size_file / BUFFER_SIZE << "\n";
		send(client_fd, response.c_str(), response.size(), 0);
		char	buf[BUFFER_SIZE] = {0};
		for (long i = response.size_file / BUFFER_SIZE; i > 0; i--)
		{
			response.file.read((char*)&buf, BUFFER_SIZE);
			send(client_fd, buf, BUFFER_SIZE, 0);
		}
		response.file.read((char*)&buf, response.size_file % BUFFER_SIZE);
		send(client_fd, buf, response.size_file % BUFFER_SIZE, 0);
	}
	else // TODO: what to do in case of send error
	{
		if (send(client_fd, response.c_str(), response.size(), 0) < 0)
		{
			std::cerr << "error send\n";
			return (false);
		}
	}
	epoll_mod(client_fd, EPOLLIN);
	return (true);
}

/*
** @brief init epoll with epoll_create and add every server listen socket
** to the epoll tracked fds
*/
bool	Webserv::epoll_init()
{
	if ((epfd = epoll_create(conf.servers.size() + 1)) < 0) // to fix max fd can be more probably not
		throw std::runtime_error("epoll create error");
	for (Connections::const_iterator cit = connections.begin(); cit != connections.end(); ++cit)
		epoll_add(cit->second.fd, EPOLLIN);
	return (true); // unused
}

Config::Connection	*Webserv::find_connection(Config::Connection &conn) {
	for (Connections::iterator it = connections.begin(); it != connections.end(); ++it) {
		if (it->second.port == conn.port && it->second.addr == conn.addr)
			return (&it->second);
	}
	return (NULL);
}

/*
** Setup the listening socket for every server config parsed
*/
void	Webserv::serv_init()
{
	for (serv_vector::iterator it = conf.servers.begin(); it != conf.servers.end(); it++) {
		Config::Server	&serv = *it;
		for (size_t	i = 0; i < serv.listen_vect.size(); ++i) {
			Config::Connection	&new_conn = serv.listen_vect[i];
			Config::Connection	*existing_conn = find_connection(new_conn);
			if (!existing_conn) {
				socket_init(new_conn);
				default_server_map[new_conn.fd] = &serv;
			}
			else
				new_conn.fd = existing_conn->fd;
			NameToServMap	&name_to_serv_map = server_map[new_conn.fd];
			for (size_t	i = 0; i < serv.server_names.size(); ++i)
				name_to_serv_map[serv.server_names.at(i)] = &serv;
		}
		serv.mime_types = &conf.types;
	}
}

/*
** @brief create server.listen_fd socket, set socket options, bind and listen
*/
int		Webserv::socket_init(Config::Connection &sock)
{
	int					listen_fd;
	int					optval = 1;

	if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw std::runtime_error("failed socket creation");
	sock.fd = listen_fd;
	connections[listen_fd] = sock;
	if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, sizeof(optval)) < 0)
		throw std::runtime_error("failed socket options");
	sockaddr_in	address = {
		.sin_family = AF_INET,
		.sin_port = htons(sock.port),
		.sin_addr = { .s_addr = htonl(sock.addr) },
		.sin_zero = { }
	};
	if (bind(listen_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
		throw std::runtime_error("failed socket bind");
	if (listen(listen_fd, MAX_CLIENTS) < 0)
		throw std::runtime_error("failed socket listen");
	return (listen_fd); // unused
}

/*
** @brief remove from the map and epoll the client whose connection socket is client_fd,
** and close the given client_fd
*/
void	Webserv::delete_client(int client_fd)
{
	clients.erase(client_fd);
	epoll_del(client_fd);
	close(client_fd);
}

/*
** @brief check if the given fd matches any of the servers' listening socket fd
*/
bool	Webserv::is_serv(int fd)
{
	for (serv_vector::const_iterator cit = conf.servers.begin(); cit != conf.servers.end(); cit++)
	{
		Config::Server const &serv = *cit;
		for (size_t	i = 0; i < serv.listen_vect.size(); ++i)
			if (serv.listen_vect[i].fd == fd)
				return (true);
	}
	return (false);
}

void	Webserv::epoll_add(int fd, int events) { utils::epoll_ctl(epfd, EPOLL_CTL_ADD, fd, events); }
void	Webserv::epoll_mod(int fd, int events) { utils::epoll_ctl(epfd, EPOLL_CTL_MOD, fd, events); }
void	Webserv::epoll_del(int fd)             { epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL); }

int		Webserv::epoll_wait() {
	int	nfds;
	if ((nfds = ::epoll_wait(epfd, events, MAX_EVENTS, TIMEOUT)) < 0 && errno != EINTR) // only error tolerated is if signal is caught
		throw std::runtime_error(std::string("\nepoll error: ") + strerror(errno)); // EINVAL, EFAULT, EBADFD
	else {
		std::cout << (nfds == 0 ? "." : "\n") << std::flush;
		return (nfds);
	}

}

Webserv::~Webserv()
{
	for (Connections::iterator it = connections.begin(); it != connections.end(); ++it)
		if (it->second.fd > 0)
			close(it->second.fd);
	if (epfd != -1)
		close(epfd);
}
