/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/14 12:25:44 by mjacq             #+#    #+#             */
/*   Updated: 2022/05/14 12:56:55 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COLOR_HPP
# define COLOR_HPP

namespace color {
	static const char	*const reset   = "\e[0m";
	static const char	*const bold    = "\e[1m";
	static const char	*const italics = "\e[2m";

	static const char	*const red     = "\e[31m";
	static const char	*const green   = "\e[32m";
	static const char	*const yellow  = "\e[33m";
	static const char	*const blue    = "\e[34m";
	static const char	*const magenta = "\e[35m";
	static const char	*const cyan    = "\e[36m";
}

#endif
