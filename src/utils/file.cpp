/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/12 11:59:54 by mjacq             #+#    #+#             */
/*   Updated: 2022/05/12 15:40:29 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "file.hpp"
# include <sys/stat.h>

file::e_type	file::get_type(std::string const &path) {
	struct stat	s;

	if (stat(path.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFDIR)
			return (FT_DIR);
		else if (s.st_mode & S_IFREG)
			return (FT_FILE);
		else
			return (FT_UNKOWN);
	}
	return (FT_UNKOWN);
}

bool	file::has_read_perm(std::string const &path) {
	struct stat	s;

	if (stat(path.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IROTH)
			return (true);
	}
	return (false);
}

bool	file::has_write_perm(std::string const &path) {
	struct stat	s;

	if (stat(path.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IWOTH)
			return (true);
	}
	return (false);
}

std::string	file::time_last_change(std::string const &file) {
	struct stat	s;
	struct tm	*time;
	char		buf[200];

	stat(file.c_str(), &s);
	time = localtime(&(s.st_ctim.tv_sec));
	strftime(buf, sizeof(buf), "%d-%b-%Y %H:%M", time);
	return (buf);
}

long	file::size(std::string const &file) {
	struct stat	s;

	return (stat(file.c_str(), &s));
}

std::string file::get_extension(std::string const &path) {
	if (path.size() < 2)
		return ("");
	for (size_t i = path.size() - 2; i > 0; --i)
		if (path[i] == '.')
			return (path.substr(i + 1));
	return ("");
}

std::string	file::join(std::string const &head, std::string const &tail) {
	char	sep = '/';
	if (head.at(head.size() - 1) != sep && tail.at(0) != sep)
		return (head + sep + tail);
	else
		return (head + tail);
}

std::string	file::get_mime(std::string const &file, Config::MimeMap const &mime_map, std::string const &default_mime) {
	std::string	extension = file::get_extension(file);
	std::string mime_type = default_mime;
	if (!extension.empty()) {
		Config::MimeMap::const_iterator	it = mime_map.find(extension);
		if (it != mime_map.end())
			mime_type = it->second;
	}
	return (mime_type);
}
