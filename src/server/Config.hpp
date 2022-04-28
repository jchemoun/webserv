/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchemoun <jchemoun@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/25 13:31:45 by jchemoun          #+#    #+#             */
/*   Updated: 2022/04/26 12:20:36 by jchemoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <stdint.h>

class	Config
{
	public:
	Config();
	Config(uint16_t addr, uint16_t port);
	uint16_t	addr;
	uint16_t	port;
};

#endif