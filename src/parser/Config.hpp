/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/24 17:59:25 by mjacq             #+#    #+#             */
/*   Updated: 2022/05/11 22:23:54 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP
# include <vector>
# include <string>
# include <map>
# include <arpa/inet.h>  // in_addr_t

struct Config {

	typedef std::map<std::string, std::string>	MimeTypes;

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

		Server();
		std::vector<std::string>	server_names;
		in_port_t					listen_port;
		in_addr_t					listen_address;
		std::string					listen_string_address; // for printing and debuging
		int							listen_fd;
		std::vector<Location>		locations;
		std::vector<std::string>	index;
		std::string					root;
		std::map<int, std::string>	error_pages;
		bool						autoindex;
		std::string					default_type;
		body_size					client_max_body_size;
		static body_size const		_overflow_body_size;
		const MimeTypes				*mime_types;
		void	print() const;
		void	set_defaults();
	};

	std::vector<Server>	servers;
	MimeTypes			types;

	void	print() const;
	void	set_defaults();
};

#endif
