/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/24 17:59:25 by mjacq             #+#    #+#             */
/*   Updated: 2022/04/28 15:26:09 by mjacq            ###   ########.fr       */
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
		std::vector<std::string>	server_names;
		in_addr_t					listen;
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
