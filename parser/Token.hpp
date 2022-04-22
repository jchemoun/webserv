/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Token.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 15:57:40 by mjacq             #+#    #+#             */
/*   Updated: 2022/04/22 19:23:03 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKEN_HPP
# define TOKEN_HPP

# include <string>

class Token {
public:
	typedef	int			token_type;
	typedef std::string	token_value;

	enum Types {
		type_word,
		type_special_char
	};

	// CANONICAL FORM
	Token(void);
	Token(Token const &copy);
	Token(token_type type, token_value value);
	Token	&operator=(Token const &copy);
	virtual ~Token(void);

	token_type const &get_type() const;
	token_value const &get_value() const;

private:
	token_type	_type;
	token_value	_value;
};

#endif
