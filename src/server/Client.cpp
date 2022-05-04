/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/26 12:19:18 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/04 13:26:00 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(/* args */): request()
{
}

void    Client::set_serv_id(int serv_id)
{
    serv_id = serv_id;
}

int     Client::get_serv_id()
{
    return (serv_id);
}

Client::~Client()
{
}
