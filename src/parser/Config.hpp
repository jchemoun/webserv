/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/24 17:59:25 by mjacq             #+#    #+#             */
/*   Updated: 2022/05/24 21:27:16 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP
# include <vector>
# include <string>
# include <map>
# include <arpa/inet.h>  // in_addr_t
# include "../utils/http_response_codes.hpp"

struct Config {

	typedef std::map<std::string, std::string>	MimeMap;
	typedef std::map<http::code, std::string>	ErrPageMap;

	struct 			Connection {
		in_port_t	port;
		in_addr_t	addr;
		std::string	str_addr;
		int			fd;
		Connection();
		Connection	&operator=(const sockaddr_in &sockaddr);
	};

	typedef	std::vector<Connection>	ListenVect;

	struct	Location {
		enum path_type { type_prefix, type_match };

		std::string					location_path;
		path_type					type;
		std::string					root;
		std::vector<std::string>	index;
		ErrPageMap					error_pages;
		bool						autoindex; // make a pair, and init in Location()
		std::vector<std::string>	allow_methods;
		std::string					cgi;
		http::code					return_code;
		std::string					return_url;
		Location();
		void	print() const;
		bool	match(std::string const &path) const;
	};

	struct	Server {
		typedef size_t				body_size;

		Server();

		std::vector<std::string>	server_names;
		ListenVect					listen_vect;
		std::vector<Location>		locations;
		std::vector<std::string>	index;
		std::string					root;
		ErrPageMap					error_pages;
		bool						autoindex;
		std::vector<std::string>	allow_methods;
		std::string					default_mime;
		body_size					client_max_body_size;
		static body_size const		_overflow_body_size;
		const MimeMap				*mime_map;

		void		print() const;
		void		set_defaults();
		std::string get_mime(std::string const &filename) const;
	};

	std::vector<Server>	servers;
	MimeMap			types;

	void	print() const;
	void	set_defaults();
};

#endif
