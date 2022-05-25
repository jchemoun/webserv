/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Uri.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 18:02:49 by mjacq             #+#    #+#             */
/*   Updated: 2022/05/25 09:26:33 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef URI_HPP
# define URI_HPP

# include <string>
# include "Config.hpp"

struct Uri {
	std::string									path;
	std::string									full_path;
	std::string const							*root;
	std::vector<std::string> const				*indexes;
	Config::ErrPageMap const					*error_pages;
	std::vector<std::string> const				*allow_methods;
	bool										autoindex;
	std::string const							*cgi;
	http::code const							*return_code;
	std::string const							*return_url;
	Config::body_size const						*client_max_body_size;
	std::pair<std::string, std::string> const	*rewrite_prefix;

	Uri(const std::string &path, Config::Server const &serv);
	void resolve(Config::Server const &serv);

	private:
	Uri();
};

#endif
