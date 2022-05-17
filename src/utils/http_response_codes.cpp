/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http_response_codes.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/16 14:54:16 by mjacq             #+#    #+#             */
/*   Updated: 2022/05/17 11:49:21 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http_response_codes.hpp"

static http::StatusMap		_init_status_header()
{
	http::StatusMap	status;

	// Info
	status[http::Continue]                = "Continue";
	// Success
	status[http::Ok]                      = "OK";
	status[http::Created]                 = "Created";
	status[http::NoContent]               = "No Content";
	// Redirections
	status[http::MovedPermanently]        = "Moved Permanently";
	status[http::PermanentRedirect]       = "Permanent Redirect";
	// Client error
	status[http::BadRequest]              = "Bad Request";
	status[http::Forbidden]               = "Forbidden";
	status[http::NotFound]                = "Not Found";
	status[http::MethodNotAllowed]        = "Method Not Allowed";
	status[http::PayloadTooLarge]         = "Request Entity Too Large";
	// Server error
	status[http::InternalServerError]     = "Internal Server Error";
	status[http::HTTPVersionNotSupported] = "HTTP Version Not Supported";

	return (status);
}

http::StatusMap const	http::status = _init_status_header();
