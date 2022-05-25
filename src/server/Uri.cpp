/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Uri.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 18:04:20 by mjacq             #+#    #+#             */
/*   Updated: 2022/05/25 10:26:26 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Uri.hpp"
#include "file.hpp"
#include <cstring>

const size_t	Uri::max_rewrite_count = 42;

Uri::Uri(const std::string &path, Config::Server const &serv):
	path          (path),
	root          (&serv.root),
	indexes       (&serv.index),
	error_pages   (&serv.error_pages),
	allow_methods (&serv.allow_methods),
	autoindex     (serv.autoindex),
	cgi           (NULL),
	return_code   (NULL),
	client_max_body_size(NULL),
	rewrite_prefix(NULL),
	rewrite_count(0)
{ }

void Uri::resolve(Config::Server const &serv) {
	for (size_t i = 0; i < serv.locations.size(); ++i) {
		Config::Location const &location = serv.locations.at(i);
		if (location.match(path)) {
			if (!location.root.empty())
				root = &location.root;
			if (!location.index.empty())
				indexes = &location.index;
			if (!location.error_pages.empty())
				error_pages = &location.error_pages;
			if (!location.allow_methods.empty())
				allow_methods = &location.allow_methods;
			if (!location.cgi.empty())
				cgi = &location.cgi;
			if (!location.return_url.empty())
				return_url = &location.return_url;
			if (location.return_code)
				return_code = &location.return_code;
			if (location.client_max_body_size != Config::_overflow_body_size)
				client_max_body_size = &location.client_max_body_size;
			if (!location.rewrite_prefix.first.empty()) {
				rewrite_prefix = &location.rewrite_prefix; // dispensable
				if (!strncmp(path.c_str(), rewrite_prefix->first.c_str(), rewrite_prefix->first.size())) {
					path = rewrite_prefix->second + path.substr(rewrite_prefix->first.size());
					++rewrite_count;
				}
			}
			autoindex = location.autoindex;
			break;
		}
	}
	full_path = file::join(*root, path);
}
