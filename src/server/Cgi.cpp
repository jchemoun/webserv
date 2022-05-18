/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/15 12:06:23 by user42            #+#    #+#             */
/*   Updated: 2022/05/18 15:09:48 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"
#include "http_response_codes.hpp"
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <cstdlib> // exit
#include <sys/wait.h>
#include "file.hpp"
#include "errno.h"

const size_t	Cgi::_buffer_size = 4242;

Cgi::Cgi(Request const &req, Config::Server const &serv)
{
	_env["CONTENT_LENGTH"]    = req.get_body().size();                                                      // body_size
	_env["CONTENT_TYPE"]      = file::get_mime(req.get_request_uri(), *serv.mime_types, serv.default_type); // mime type of body
	_env["GATEWAY_INTERFACE"] = "CGI/1.1";                                                                  // always this
	_env["PATH_INFO"]         = file::join(serv.root, req.get_uri());                                       // request path
	_env["QUERY_STRING"]      = req.get_query_string();                                                     // things after '?' in url
	_env["REMOTE_ADDR"]       = ""/*TODO*/;                                                                 // ip of the client or the server idk
	_env["REQUEST_METHOD"]    = req.get_method();                                                           // get or post
	_env["SCRIPT_NAME"]       = ""/*TODO*/;                                                                 // don't know for sure, probably just the path to cgi
	_env["SERVER_NAME"]       = ""/*TODO*/;                                                                 // name of the server receiving the request
	_env["SERVER_PORT"]       = ""/*TODO*/;                                                                 // request's port number
	_env["SERVER_PROTOCOL"]   = "HTTP/1.1";                                                                 // always this
	_env["SERVER_SOFTWARE"]   = "Webserv";                                                                  // always this
	_env["REQUEST_URI"]       = req.get_request_uri();                                                      // full request
	_env["REDIRECT_STATUS"]   = "200";                                                                      // always this
	_env["SCRIPT_FILENAME"]   = ""/*TODO*/;                                                                 // full path to cgi
	(void)serv;
}

char	**Cgi::map_to_tab(env_map const &env)
{
	char	**ret;
	size_t	i;

	ret = new char*[env.size() + 1];
	i = 0;
	for (env_map::const_iterator cit = env.begin(); cit != env.end(); cit++)
	{
		std::string	str = (*cit).first + '=' + (*cit).second;
		ret[i] = new char[str.length() + 1];
		ret[i] = strcpy(ret[i], str.c_str());
		i++;
	}
	ret[i] = NULL;
	return (ret);
}

void	Cgi::delete_tab(char **tab)
{
	size_t i;

	i = 0;
	while (tab[i])
	{
		delete[] tab[i];
		i++;
	}
	delete[] tab;
}

int		Cgi::run()
{
	int			pipefd[2];
	char		**tab;
	char		buf[_buffer_size];
	ssize_t		len;
	pid_t		cpid;

	if (pipe(pipefd) == -1)
		return (http::InternalServerError);
	tab = map_to_tab(_env);
	cpid = fork();
	if (cpid == -1)
	{
		delete_tab(tab);
		close(pipefd[0]);
		close(pipefd[1]);
		return (http::InternalServerError);
	}
	else if (cpid == 0)
	{
		close(pipefd[0]);
		dup2(pipefd[1], 1);
		close(pipefd[1]);
		const char *const av[] = {_env["PATH_INFO"].c_str(), NULL};
		execve(av[0], const_cast<char * const*>(av), tab);
		std::string	error = "Status: 500\r\n\r\n";     // std::cout << "execve FAIL:" << std::strerror(errno) << std::endl;
		write(pipefd[1], error.c_str(), error.size());
		exit(EXIT_FAILURE);                            // exit codes should be <= 255
	}
	else
	{
		close(pipefd[1]);
		wait(&cpid);
		while ((len = read(pipefd[0], &buf, _buffer_size - 1)) > 0)
		{
			buf[len] = '\0';
			_body += buf;
		}
	}
	close(pipefd[0]);
	delete_tab(tab);
	if (len == -1)
		return (http::InternalServerError);
	return (http::Ok);
}

std::string	Cgi::parse_body()
{
	std::cout << "BODY CGI " << _body << '\n';
	return (_body);
}

Cgi::~Cgi()
{
}
