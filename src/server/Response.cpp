/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchemoun <jchemoun@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/30 14:02:37 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/01 13:03:30 by jchemoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(/* args */): header(), body(), full_response(), len(0)
{
}

int	Response::check_path(std::string path)
{
	struct stat	s;

	if (stat(path.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFDIR)
			return (1);
		else if (s.st_mode & S_IFREG)
			return (2);
		else
			return (0);
	}
	return (0);
}

bool	Response::check_read_perm(std::string path)
{
	struct stat	s;

	if (stat(path.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IROTH)
			return (true);
	}
	return (false);
}

std::string	Response::create_auto_index_page(std::string location)
{
	(void)location;
	return (0);
}

size_t	Response::read_file(std::string location)
{
	std::ofstream		file;
	std::stringstream	buf;

	//std::cout << "fgh";
	if (check_path(location) == 1 /* && auto_index_on */)
		body = create_auto_index_page(location);
	else if (check_path(location) == 2)
	{
		// read file
		//std::cout << "123\n";
		if (check_read_perm(location) == false)
		{
			//std::cout << "403\n";
			//403
			return (0);
		}
		file.open(location.c_str(), std::ifstream::in);
		if (file.is_open() == false)
		{
			// 404
			return (0);
		}
		buf << file.rdbuf();
		file.close();
		body = buf.str();
		len += body.length();
	}
	else
	{
		//std::cout << "404\n";
		// 404
		return (0);
	}
	std::cout << body;
	return (body.length());
}

// read file for error pages

void		Response::set_full_response()
{
	full_response.append(header);
	full_response.append(body);
}

std::string	Response::get_full_response() const
{
	return (full_response);
}

size_t		Response::get_len() const
{
	return (len);
}

Response::~Response()
{
}