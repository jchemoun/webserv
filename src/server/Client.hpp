/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/26 12:18:20 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/04 13:25:33 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Request.hpp"
# include "Config.hpp"

/*
get time for session if bonus
data in client ?
class is useless for now
*/

class Client
{
private:
	int	serv_id;
public:
	Request	request;
	Client(/* args */);

	void	set_serv_id(int serv_id);

	int		get_serv_id();

	~Client();
};

#endif