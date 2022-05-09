/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/26 12:18:20 by jchemoun          #+#    #+#             */
/*   Updated: 2022/05/07 17:26:57 by mjacq            ###   ########.fr       */
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
	int	_serv_id;
public:
	Request	request;
	Client(int serv_id = -1);
	~Client();

	int		get_serv_id();
};

#endif
