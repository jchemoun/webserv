/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http_response_codes.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/16 14:45:44 by mjacq             #+#    #+#             */
/*   Updated: 2022/05/16 15:19:10 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_RESPONSE_CODES_HPP
# define HTTP_RESPONSE_CODES_HPP

# include <string>
# include <map>

namespace http {

	typedef std::map<int, std::string>	StatusMap;

	enum {
		// Info
		Continue                = 100,
		// Success
		Ok                      = 200,
		Created                 = 201,
		NoContent               = 204,
		// Redirections
		MovedPermanently        = 301,
		PermanentRedirect       = 308,
		// Client error
		BadRequest              = 400,
		Forbidden               = 403,
		NotFound                = 404,
		MethodNotAllowed        = 405,
		PayloadTooLarge         = 413,
		// Server error
		InternalServerError     = 500,
		HTTPVersionNotSupported = 505
	};

	extern const StatusMap	_status_header;
}

#endif