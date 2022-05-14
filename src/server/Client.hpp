/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/26 12:18:20 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/13 23:21:05 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Request.hpp"
# include "Config.hpp"

/*
get time for session if bonus
data in client ?
class is useless for now
*/

class Client
{
	int					_listen_fd;
public:
	Config::Connection	connection;
	Request				request;

	Client();
	~Client();

	bool	accept_connection(int listen_fd);
	void	_print_connection_info();
	void	close_connection();

	typedef std::map<std::string, Config::Server *>	NameToServMap;    // key: server_name
	typedef std::map<int, NameToServMap>			ServerMap;        // first key: listen_fd, second: server_name
	typedef std::map<int, Config::Server *>			DefaultServerMap; // key: listen_fd
	Config::Server	&resolve_server(ServerMap &serverMap, DefaultServerMap &def_server_map) const;
};

#endif
