/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchemoun <jchemoun@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/04 11:30:46 by jchemoun          #+#    #+#             */
/*   Updated: 2022/04/22 14:24:04 by jchemoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <sys/epoll.h>

# include <unistd.h>
# include <cstdlib>
# include <cstring>
# include <iostream>
# include <string>
# include <vector>

// define temporaire ? valeur un peu random
# define MAX_CLIENTS	64
# define TIMEOUT		256
# define MAX_EVENTS		1024

class	Config {};

class Webserv
{
public:
	typedef	std::vector<int>	serv_vector;
	typedef std::vector<Config>	conf_vector;
	Webserv();
	~Webserv();
	void	run();
private:
	//epoll
	int					epfd;
	struct epoll_event	event;
	struct epoll_event	events[MAX_EVENTS];
	//serv
	serv_vector			serv;
	conf_vector			conf;

	//init
	bool	epoll_init();
	bool	serv_init();
	int		socket_init(Config conf);

	//close/error
	void	close_serv();
};

#endif
