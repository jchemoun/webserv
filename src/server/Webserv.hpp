/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/04 11:30:46 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/03 06:55:09 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <sys/epoll.h>
# include <errno.h>

# include <unistd.h>
# include <cstdlib>
# include <cstring>
# include <iostream>
# include <string>
# include <vector>
# include <map>

// # include "Config.hpp"
# include "Parser.hpp"
# include "Client.hpp"
# include "Response.hpp"

// define temporaire ? valeur un peu random
# define MAX_CLIENTS	64
# define TIMEOUT		256
# define MAX_EVENTS		1024
# define BUFFER_SIZE	4096

class Webserv
{
public:
	typedef std::vector<Config::Server>	serv_vector;
	typedef std::map<int, Client>	client_map; // maybe vector ? for now fd/client pair
	Webserv();
	~Webserv();
	void	run();
	void	get_config(int ac, const char **av);
private:
	//epoll
	int					epfd;
	struct epoll_event	event;
	struct epoll_event	events[MAX_EVENTS];
	//serv
	Config				conf;
	client_map			clients;

	//init
	bool	epoll_init();
	void	serv_init();
	int		socket_init(Config::Server &conf);

	//handle
	bool	handle_error();
	bool	handle_new_client(int serv_fd);
	bool	handle_recv(int client_fd);
	bool	handle_send(int client_fd);

	//utils
	void	delete_client(int client_fd);
	bool	is_serv(int fd);

	//close/error
	// void	close_serv(); // use destructor instead
};

#endif
