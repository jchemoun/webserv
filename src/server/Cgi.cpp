/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/15 12:06:23 by user42            #+#    #+#             */
/*   Updated: 2022/05/17 13:25:48 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"
#include <cstdio>
#include <cstring>
#include <vector>

Cgi::Cgi(/* args */): env()
{
	env["CONTENT_LENGTH"] = ""; //			body_size
	env["CONTENT_TYPE"] = ""; //			mime type of body
	env["GATEWAY_INTERFACE"] = "CGI/1.1"; //always this
	env["PATH_INFO"] = ""; //				request path
	env["QUERY_STRING"] = ""; //			things after '?' in url
	env["REMOTE_ADDR"] = ""; //				ip of the client
	env["REQUEST_METHODE"] = "";//			get or post
	env["SCRIPT_NAME"] = "";//				don't know for sure, probably just the path to cgi
	env["SERVER_NAME"] = "";//				name of the server receiving the request
	env["SERVER_PORT"] = "";//				request's port number
	env["SERVER_PROTOCOL"] = "HTTP/1.1";//	always this
	env["SERVER_SOFTWARE"] = "Webserv";//	always this
	env["REQUEST_URI"] = "";//				full request
	env["REDIRECT_STATUS"] = "200";//		always this
	env["SCRIPT_FILENAME"] = "";//			full path to cgi
}

Cgi::Cgi(Request const &req, Config::Server const &serv): env()
{
	env["CONTENT_LENGTH"] = req.get_body().size();//				body_size
	env["CONTENT_TYPE"] = file::get_mime(req.get_request_uri(), *serv.mime_types, serv.default_type);//	mime type of body
	env["GATEWAY_INTERFACE"] = "CGI/1.1";//							always this
	env["PATH_INFO"] = file::join(serv.root, req.get_uri());//								request path
	env["QUERY_STRING"] = req.get_query_string();//					things after '?' in url
	env["REMOTE_ADDR"] = ""/*TODO*/; //								ip of the client or the server idk
	env["REQUEST_METHODE"] = req.get_method();//					get or post
	env["SCRIPT_NAME"] = ""/*TODO*/;//								don't know for sure, probably just the path to cgi
	env["SERVER_NAME"] = ""/*TODO*/;//								name of the server receiving the request
	env["SERVER_PORT"] = ""/*TODO*/;//								request's port number
	env["SERVER_PROTOCOL"] = "HTTP/1.1";//							always this
	env["SERVER_SOFTWARE"] = "Webserv";//							always this
	env["REQUEST_URI"] = req.get_request_uri();//						full request
	env["REDIRECT_STATUS"] = "200";//								always this
	env["SCRIPT_FILENAME"] = ""/*TODO*/;//							full path to cgi
	(void)serv;
}

/*
TODO:	func map to char** for execve //TOTEST
		func for prep_pipe -> see minishell
		func exec cgi -> see minishell
		look what the cgi is returning
		func parse return cgi if needed
*/

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
	int			inout[2];
	char		**tab;
	char		buf[BUFFER_SIZE + 1];
	ssize_t		len;
	pid_t		cpid;

	inout[0] = dup(0);
	inout[1] = dup(1);
	if (pipe(pipefd) == -1)
		return (500);
	tab = map_to_tab(env);
	cpid = fork();
	if (cpid == -1)
	{
		delete_tab(tab);
		close(pipefd[0]);
		close(pipefd[1]);
		return (500);
	}
	else if (cpid == 0)
	{
		close(pipefd[0]);
		dup2(pipefd[1], 1);
		close(pipefd[1]);
		const char *const av[] = {env["PATH_INFO"].c_str(), NULL};
		execve(av[0], const_cast<char * const*>(av), tab);
		std::cout << "execve FAIL:" << std::strerror(errno) << std::endl;
		exit(1); // exit codes should be <= 255
	}
	else
	{
		close(pipefd[1]);
		dup2(pipefd[0], 0);
		close(pipefd[0]);
		wait(&cpid);
		while ((len = read(0, &buf, BUFFER_SIZE)) > 0)
		{
			buf[BUFFER_SIZE] = '\0';
			_body += buf;
		}
		if (len == -1)
			return (500);
	}
	
	dup2(inout[0], 0);
	dup2(inout[1], 1);
	// maybe close inout
	delete_tab(tab);
	return (200);
}

std::string	Cgi::parse_body()
{
	std::cout << "BODY CGI " << _body << '\n';
	return (_body);
}

Cgi::~Cgi()
{
}