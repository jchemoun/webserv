/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/15 12:05:43 by user42            #+#    #+#             */
/*   Updated: 2022/05/17 13:15:59 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

#include <cstddef>
# include <map>
# include <string>
# include <string.h>

# include "Request.hpp"
# include "Config.hpp"

class Cgi
{
public:
	typedef std::map<std::string, std::string>  env_map;

private:
	static const	size_t	_buffer_size;
	env_map			_env;
	std::string		_body;

public:
	Cgi(Request const &req, Config::Server const &serv);
	~Cgi();

	int			run();
	std::string	parse_body();

private:
	Cgi();
	char	**map_to_tab(env_map const &env);
	void	delete_tab(char **tab);
};

#endif