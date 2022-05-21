/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/04 11:30:46 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/21 13:41:10 by user42           ###   ########.fr       */
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
# include "Include.hpp"

// define temporaire ? valeur un peu random
 // was 4096

class Webserv
{
public:
	typedef std::vector<Config::Server>	serv_vector;
	typedef std::map<int, Client>		client_map;

	typedef std::map<int, Config::Connection>			Connections;      // key: listen_fd
	typedef std::map<std::string, Config::Server *>		NameToServMap;    // key: server_name
	typedef std::map<int, NameToServMap>				ServerMap;        // first key: listen_fd, second: server_name
	typedef std::map<int, Config::Server *>				DefaultServerMap; // key: listen_fd

	Webserv();
	~Webserv();
	void	run();
	void	get_config(int ac, const char **av);
private:
	//epoll
	int					epfd;
	struct epoll_event	events[MAX_EVENTS];
	//serv
	Config				conf;
	client_map			clients;
	Connections			connections;
	ServerMap			server_map;
	DefaultServerMap	default_server_map;

	//init
	bool	epoll_init();
	void	serv_init();
	int		socket_init(Config::Connection &sock);

	//handle
	bool	handle_event_error(int client_fd);
	bool	handle_new_client(int serv_fd);
	bool	handle_recv(int client_fd);
	bool	handle_send(int client_fd);
	bool	handle_big_send(int client_fd);

	//utils
	void	delete_client(int client_fd);
	bool	is_serv(int fd);
	int		find_serv_id(int serv_fd);

	//epoll utils
	void	epoll_add(int fd, int events);
	void	epoll_mod(int fd, int events);
	void	epoll_del(int fd);
	int		epoll_wait();

	Config::Connection	*find_connection(Config::Connection &conn);

};

#endif
