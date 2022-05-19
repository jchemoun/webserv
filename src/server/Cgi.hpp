/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/15 12:05:43 by user42            #+#    #+#             */
/*   Updated: 2022/05/19 09:38:56 by mjacq            ###   ########.fr       */
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
	typedef std::map<std::string, std::string>	Header;

private:
	static const size_t	_buffer_size;
	env_map				_env;
	char				**_env_tab;
	int					_pipefd[2];
	// int					&_pipe_in;
	// int					&_pipe_out;

public:
	std::string		_body;
	Header			_header;

	Cgi(Request const &req, Config::Server const &serv);
	~Cgi();

	void		run();

private:
	Cgi();
	void		_execute();
	void		_parse_body();

	// generic utils
	static char	**_map_to_tab(env_map const &env);
	static void	_delete_tab(char **tab);
};

#endif
