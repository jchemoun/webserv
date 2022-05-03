/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/24 17:59:25 by mjacq             #+#    #+#             */
/*   Updated: 2022/04/28 17:36:48 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP
# include <vector>
# include <string>
# include <map>
# include <arpa/inet.h>  // in_addr_t

struct Config {

	struct	Location {
		std::string					location_path;
		std::string					root;
		std::vector<std::string>	index;
		std::map<int, std::string>	error_pages;
		void print() const;
	};

	struct	Server {
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
		void print() const;
	};

	std::vector<Server>	servers;
	void print() const;
};

#endif
