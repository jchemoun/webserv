/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/12 11:28:42 by mjacq             #+#    #+#             */
/*   Updated: 2022/05/13 16:50:05 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"
#include <sys/epoll.h>

void	utils::epoll_ctl(int epfd, int operation, int fd, int events) {
	epoll_event event = { };
	event.events = events;
	event.data.fd = fd;
	if (epoll_ctl(epfd, operation, fd, &event) < 0)
		throw std::runtime_error("epoll_ctl error");
}
