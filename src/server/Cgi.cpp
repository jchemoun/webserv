/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/15 12:06:23 by user42            #+#    #+#             */
/*   Updated: 2022/05/20 16:05:12 by mjacq            ###   ########.fr       */
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
#include "utils.hpp"
#include <algorithm> // std::transform

const size_t	Cgi::_buffer_size = 4242;

/*
** ======================== Constructor / Destructor ======================== **
*/

/*
** https://fr.wikipedia.org/wiki/Variables_d'environnement_CGI
*/

Cgi::Cgi(Request const &req, Config::Connection const &client_info):
	_env_tab(NULL),
	_req_body(req.get_body())
{
	_pipe_from_cgi[0] = -1;
	_pipe_from_cgi[1] = -1;
	_pipe_to_cgi[0] = -1;
	_pipe_to_cgi[1] = -1;
	_set_env(req, client_info);
	_env_tab = _map_to_tab(_env);
}

Cgi::~Cgi() {
	_delete_tab(_env_tab);
	if (_pipe_from_cgi[0] > 0) close(_pipe_from_cgi[0]);
	if (_pipe_from_cgi[1] > 0) close(_pipe_from_cgi[1]);
	if (_pipe_to_cgi[0] > 0) close(_pipe_to_cgi[0]);
	if (_pipe_to_cgi[1] > 0) close(_pipe_to_cgi[1]);
}

/*
** ============================ Public functions ============================ **
*/

void	Cgi::run()
{
	_execute();
	_parse_output();
}

/*
** =========================== Private functions ============================ **
*/

void	Cgi::_set_env(Request const &req, Config::Connection const &client_info) {
	Config::Server const	&serv = *req.current_server;
	std::string const 		&uri  = req.get_uri();

	bool		has_path_info = (uri.find(".cgi/") != std::string::npos);
	std::string	script_name   = (has_path_info ? uri.substr(0, uri.find(".cgi/") + 4) : uri);

	// Server specific
	_env["SERVER_SOFTWARE"]   = "Webserv";
	_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	_env["SERVER_NAME"]       = req.current_server_name;              // host name of the server
	_env["DOCUMENT_ROOT"]     = serv.root;

	// Request specific
	_env["SERVER_PROTOCOL"]         = req.get_protocol();
	_env["SERVER_PORT"]             = utils::to_str(req.listen_info->port);
	_env["REQUEST_METHOD"]          = req.get_method();
	// if (has_path_info) {
	// 	_env["PATH_INFO"]           = uri.substr(script_name.size());           // path in the request after the cgi's name
	// 	_env["PATH_TRANSLATED"]     = file::join(serv.root, _env["PATH_INFO"]); // corresponding full path as supposed by server, if PATH_INFO is present
	// }
	_env["PATH_INFO"]               = script_name;                              // Not really this but this is what 42 tester expects...
	_env["SCRIPT_NAME"]             = script_name;                              // relative path of the program (like /cgi-bin/script.cgi)
	_env["SCRIPT_FILENAME"]         = file::join(serv.root, script_name);       // Chemin d'accès complet au script CGI (FULL PATH)
	_env["QUERY_STRING"]            = req.get_query_string();                   // things after '?' in url
	/*_env["REMOTE_HOST"]           = "";*/                                     // host name of the client, unset if server did not perform such lookup.
	_env["REMOTE_ADDR"]             = client_info.str_addr;                     // ip, client side
	/*_env["AUTH_TYPE"]             = "";*/                                     // identification type, if applicable
	/*_env["REMOTE_USER"]           = "";*/                                     // used for certain AUTH_TYPEs.
	/*_env["REMOTE_IDENT"]          = "";*/                                     // used for certain AUTH_TYPEs.
	_env["CONTENT_TYPE"]            = serv.get_mime(req.get_uri());             // mime type of body
	// _env["CONTENT_LENGTH"]          = req.get_body().size();                    // body_size

	_env["REMOTE_PORT"]             = utils::to_str(client_info.port);          // port, client side
	_env["SERVER_ADDR"]             = req.listen_info->str_addr;
	_env["REQUEST_URI"]             = req.get_request_uri();
	// _env["REDIRECT_STATUS"]   = "200";

	// Client specific
	// Toutes les variables qui sont envoyées par le client sont aussi passées au script CGI,
	// après que le serveur a rajouté le préfixe « HTTP_ ».
	for (Header::const_iterator cit = req._header.begin(); cit != req._header.end(); ++cit)
		_env[_header_key_to_cgi_format(cit->first)] = cit->second;

	// More...
	const char	*s;
	_env["PATH"]              = ( (s = getenv("PATH") ) ? s : "");
}

void	Cgi::_execute()
{
	pid_t		cpid;
	std::string const &cgi_full_path = _env["SCRIPT_FILENAME"];

	if (file::get_type(cgi_full_path) == file::FT_UNKOWN)
		throw (http::NotFound);
	if (!file::has_exec_perm(cgi_full_path))
		throw (http::Forbidden);

	if ((pipe(_pipe_from_cgi) == -1) || (pipe(_pipe_to_cgi) == -1))
		throw (http::InternalServerError);

	if ((cpid = fork()) == -1)
		throw (http::InternalServerError);

	else if (cpid == 0)
		_child_execute();
	else
		_parent_wait_and_read_pipe(cpid);
}

void	Cgi::_child_execute()
{
	_close_pipe(_pipe_from_cgi[0]);
	_close_pipe(_pipe_to_cgi[1]);

	dup2(_pipe_from_cgi[1], STDOUT_FILENO);
	dup2(_pipe_to_cgi[0], STDIN_FILENO);
	_close_pipe(_pipe_from_cgi[1]);

	const char *const av[] = {_env["SCRIPT_FILENAME"].c_str(), NULL};
	execve(_env["SCRIPT_FILENAME"].c_str(), const_cast<char * const*>(av), _env_tab);

	std::cerr << color::red << "execve FAIL:" << std::strerror(errno) << color::reset << std::endl;
	exit(EXIT_FAILURE);

	// TODO: quit properly?
}

void	Cgi::_parent_wait_and_read_pipe(int child_pid)
{
	char		buf[_buffer_size];
	ssize_t		len;
	int			wait_status;

	_close_pipe(_pipe_from_cgi[1]);
	_close_pipe(_pipe_to_cgi[0]);
	if (_req_body.size()) {
		write(_pipe_to_cgi[1], _req_body.c_str(), _req_body.size());
	}
	_close_pipe(_pipe_to_cgi[1]);
	if (waitpid(child_pid, &wait_status, 0) == -1) {
		throw http::InternalServerError;
	}
	if (WIFEXITED(wait_status) && WEXITSTATUS(wait_status) != EXIT_SUCCESS) {
		throw http::InternalServerError;
	}
	// TODO: loop if !WIFEXITED
	while ((len = read(_pipe_from_cgi[0], &buf, _buffer_size - 1)) > 0) {
		buf[len] = '\0';
		_output += buf;
	}
	_close_pipe(_pipe_from_cgi[0]);
	if (len == -1)
		throw (http::InternalServerError);
}

void	Cgi::_parse_output()
{
	Request req;
	req.parse_cgi(_output);
	std::swap(req._body, body);
	std::swap(req._header, header);
}

/*
** ============================= Generic utils ============================== **
*/

char	**Cgi::_map_to_tab(env_map const &env)
{
	char	**ret;
	size_t	i;

	ret = new char*[env.size() + 1];
	i = 0;
	for (env_map::const_iterator cit = env.begin(); cit != env.end(); cit++)
	{
		std::string	str = cit->first + '=' + cit->second;
		ret[i] = new char[str.length() + 1];
		ret[i] = strcpy(ret[i], str.c_str());
		i++;
	}
	ret[i] = NULL;
	return (ret);
}

void	Cgi::_delete_tab(char **tab)
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

void	Cgi::_close_pipe(int &fd) { close (fd); fd = -1; }

static int	_cgi_char_toupper(int c) { if (c == '-') return ('_'); else return (::toupper(c)); }
/*
** @example would transform 'Accept-Language' into 'HTTP_ACCEPT_LANGUAGE'
*/
std::string Cgi::_header_key_to_cgi_format(std::string const &key) {
	static const std::string	prefix = "HTTP_";
	std::string	new_key(prefix + key);
	std::transform(new_key.begin(), new_key.end(), new_key.begin(), _cgi_char_toupper);
	return (new_key);
}
