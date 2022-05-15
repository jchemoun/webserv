/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/15 12:06:23 by user42            #+#    #+#             */
/*   Updated: 2022/05/15 13:12:11 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"

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
	env["REQUEST_URI"] = "";//				request path
	env["REDIRECT_STATUS"] = "200";//		always this
	env["SCRIPT_FILENAME"] = "";//			full path to cgi
}

Cgi::Cgi(Request const &req, Config::Server const &serv): env()
{
	env["CONTENT_LENGTH"] = req.get_body().size(); //				body_size
	env["CONTENT_TYPE"] = req.get_header().at("Content-Type"); //	mime type of body
	env["GATEWAY_INTERFACE"] = "CGI/1.1"; //						always this
	env["PATH_INFO"] = req.get_location(); //						request path
	env["QUERY_STRING"] = ""/*TODO*/; //							things after '?' in url
	env["REMOTE_ADDR"] = ""/*TODO*/; //								ip of the client or the server idk
	env["REQUEST_METHODE"] = req.get_method();//					get or post
	env["SCRIPT_NAME"] = ""/*TODO*/;//								don't know for sure, probably just the path to cgi
	env["SERVER_NAME"] = serv.server_names.front();//				name of the server receiving the request
	env["SERVER_PORT"] = ""/*TODO*/;//								request's port number
	env["SERVER_PROTOCOL"] = "HTTP/1.1";//							always this
	env["SERVER_SOFTWARE"] = "Webserv";//							always this
	env["REQUEST_URI"] = req.get_location();//						request path
	env["REDIRECT_STATUS"] = "200";//								always this
	env["SCRIPT_FILENAME"] = ""/*TODO*/;//							full path to cgi
}

/*
TODO:	func map to char** for execve
		func for prep_pipe -> see minishell
		func exec cgi -> see minishell
		look what the cgi is returning
		func parse return cgi if needed
*/

Cgi::~Cgi()
{
}