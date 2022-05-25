/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/15 12:05:43 by user42            #+#    #+#             */
/*   Updated: 2022/05/24 18:07:50 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include <cstddef>
# include <map>
# include <string>
# include <string.h>

# include "Request.hpp"
# include "Config.hpp"
# include "Uri.hpp"

class Cgi
{
public:
	typedef std::map<std::string, std::string>  env_map;
	typedef std::map<std::string, std::string>	Header;

private:
	static const size_t	_buffer_size;
	env_map				_env;
	char				**_env_tab;
	int					_pipe_from_cgi[2];
	int					_pipe_to_cgi[2];
	std::string const	&_req_body;
	std::string			_output;

public:
	Header			header;
	std::string		body;

	Cgi(Request const &req, Uri const &uri, Config::Connection const &client_info);
	~Cgi();

	void		run();

private:
	Cgi();
	void		_set_env(Request const &req, Uri const &uri, Config::Connection const &client_info);
	void		_execute();
	void		_child_execute();
	void		_parent_wait_and_read_pipe(int child_pid);
	void		_parse_output();

	// static utils
	static char			**_map_to_tab(env_map const &env);
	static void			_delete_tab(char **tab);
	static void			_close_pipe(int &fd);
	static std::string	_header_key_to_cgi_format(std::string const &key);
};

#endif
