/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Uri.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 18:04:20 by mjacq             #+#    #+#             */
/*   Updated: 2022/05/24 18:05:28 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Uri.hpp"
#include "file.hpp"

Uri::Uri(const std::string &path, Config::Server const &serv):
	path          (path),
	root          (&serv.root),
	indexes       (&serv.index),
	error_pages   (&serv.error_pages),
	allow_methods (&serv.allow_methods),
	autoindex     (serv.autoindex),
	cgi           (NULL)
{ }

void Uri::resolve(Config::Server const &serv) {
	root          = &serv.root;
	indexes       = &serv.index;
	error_pages   = &serv.error_pages;
	allow_methods = &serv.allow_methods;
	autoindex     = serv.autoindex;
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
			autoindex = location.autoindex;
			break;
		}
	}
	full_path = file::join(*root, path);
}
