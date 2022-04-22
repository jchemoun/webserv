/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchemoun <jchemoun@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/04 11:30:46 by jchemoun          #+#    #+#             */
/*   Updated: 2022/04/22 13:21:32 by jchemoun         ###   ########.fr       */
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

# define MAX_CLIENTS	64
# define TIMEOUT		200

class Webserv
{
public:
	typedef	std::vector<int>	serv_vector;
	Webserv();
	~Webserv();
	void	run();
private:
	//epoll
	int					epfd;
	struct epoll_event	event;
	//serv
	serv_vector			serv;

	//init
	bool	epoll_init();
	bool	serv_init();
};

#endif
