/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_cgi.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/18 08:33:14 by mjacq             #+#    #+#             */
/*   Updated: 2022/05/20 11:54:18 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <stdlib.h>

const std::string ENV[] = {
	"GATEWAY_INTERFACE",
	"HTTP_ACCEPT",
	"HTTP_ACCEPT_ENCODING",
	"HTTP_ACCEPT_LANGUAGE",
	"HTTP_CONNECTION",
	"HTTP_HOST",
	"HTTP_USER_AGENT",
	"PATH",
	"QUERY_STRING",
	"PATH_INFO",
	"REMOTE_ADDR",
	"REMOTE_PORT",
	"REQUEST_METHOD",
	"REQUEST_URI",
	"SCRIPT_FILENAME",
	"SCRIPT_NAME",
	"SERVER_ADDR",
	"SERVER_NAME",
	"SERVER_PORT",
	"SERVER_PROTOCOL",
	"SERVER_SOFTWARE"
};

int main () {
	std::cout
		<< "Content-Type:text/html\r\n\r\n"
		<< "<html>\n"
		<< "<head>\n"
		<< "<title>CGI Environment Variables</title>\n"
		<< "</head>\n"
		<< "<body>\n"
		<< "<table border=\"1px solid black\">";

	for (size_t i = 0; i < sizeof ENV / sizeof ENV[0]; ++i) {
		
		const char *value = getenv(ENV[i].c_str()); // attempt to retrieve value of environment variable

		std::cout
			<< "<tr><td>" << ENV[i] << "</td><td>"
			<< (value ? value : "-")
			<< "</td></tr>\n";
	}

	std::cout << "</table>\n";
	std::cout << "</body>\n";
	std::cout << "</html>\n";

	return (0);
}
