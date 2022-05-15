/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/15 12:05:43 by user42            #+#    #+#             */
/*   Updated: 2022/05/15 13:01:10 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include <map>
# include <string>
# include "Request.hpp"
# include "Config.hpp"

class Cgi
{
public:
	typedef std::map<std::string, std::string>  env_map;
	Cgi(/* args */);
	Cgi(Request const &req, Config::Server const &serv);
	~Cgi();
private:
	env_map env;
};

#endif