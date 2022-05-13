/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/24 17:59:25 by mjacq             #+#    #+#             */
/*   Updated: 2022/05/13 05:39:39 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP
# include <vector>
# include <string>
# include <map>
# include <arpa/inet.h>  // in_addr_t

struct Config {

	typedef std::map<std::string, std::string>	MimeMap;
	typedef std::map<int, std::string>			ErrPageMap;

	struct	Location {
		Location();
		std::string					location_path;
		std::string					root;
		std::vector<std::string>	index;
		std::map<int, std::string>	error_pages;
		bool						autoindex;
		void	print() const;
	};

	struct	Server {
		typedef size_t	body_size;

		struct 			Listen {
			in_port_t	port;
			in_addr_t	addr;
			std::string	str_addr;
			int			fd;
			Listen();
		};

		typedef	std::vector<Listen>	ListenVect;

		Server();
		std::vector<std::string>	server_names;
		// in_port_t					listen_port;
		// in_addr_t					listen_address;
		// std::string					listen_string_address; // for printing and debuging
		// int							listen_fd;
		ListenVect					listen;
		std::vector<Location>		locations;
		std::vector<std::string>	index;
		std::string					root;
		ErrPageMap					error_pages;
		bool						autoindex;
		std::string					default_type;
		body_size					client_max_body_size;
		static body_size const		_overflow_body_size;
		const MimeMap				*mime_types;
		void	print() const;
		void	set_defaults();
	};

	std::vector<Server>	servers;
	MimeMap			types;

	void	print() const;
	void	set_defaults();
};

#endif
