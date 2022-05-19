/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_cgi.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/18 08:33:14 by mjacq             #+#    #+#             */
/*   Updated: 2022/05/18 19:26:02 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <stdlib.h>

const std::string ENV[] = {
	"COMSPEC",
	"DOCUMENT_ROOT",
	"GATEWAY_INTERFACE",
	"HTTP_ACCEPT",
	"HTTP_ACCEPT_ENCODING",
	"HTTP_ACCEPT_LANGUAGE",
	"HTTP_CONNECTION",
	"HTTP_HOST",
	"HTTP_USER_AGENT",
	"PATH",
	"QUERY_STRING",
	"REMOTE_ADDR",
	"REMOTE_PORT",
	"REQUEST_METHOD",
	"REQUEST_URI",
	"SCRIPT_FILENAME",
	"SCRIPT_NAME",
	"SERVER_ADDR",
	"SERVER_ADMIN",
	"SERVER_NAME",
	"SERVER_PORT",
	"SERVER_PROTOCOL",
	"SERVER_SIGNATURE",
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
		<< "<table border = \"0\" cellspacing = \"2\">";

	for (size_t i = 0; i < sizeof ENV / sizeof ENV[0]; ++i) {
		std::cout << "<tr><td>" << ENV[i] << "</td><td>";

		// attempt to retrieve value of environment variable
		const char *value = getenv(ENV[i].c_str());
		if (value) {
			std::cout << value;
		} else {
			std::cout << "-";
		}
		std::cout << "</td></tr>\n";
	}

	std::cout << "</table><\n";
	std::cout << "</body>\n";
	std::cout << "</html>\n";

	return (0);
}
