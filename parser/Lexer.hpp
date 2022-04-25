/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjacq <mjacq@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/22 17:53:06 by mjacq             #+#    #+#             */
/*   Updated: 2022/04/25 14:33:18 by mjacq            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_HPP
# define LEXER_HPP

# include "Token.hpp"
# include <vector>
# include <fstream>

// @brief turns file into a list of tokens
class Lexer {
public:
	// CANONICAL FORM
	Lexer(std::string filename);
	virtual ~Lexer(void);

	void	print() const;
	size_t	size() const;
	Token const	&next();
	Token const	&peek() const;
	Token const	&peek_next() const;

private:
	// CANONICAL FORM
	Lexer(void);
	Lexer(Lexer const &copy);
	Lexer	&operator=(Lexer const &copy);

	void	_open(std::string &filename);

	void	_tokenize_file();
	void	_tokenize_line();
	void	_tokenize_special_char();
	void	_tokenize_word();

	static const char *_special_chars;

	static bool _is_skipped_char(char c);
	static bool	_is_special_char(char c);
	static bool _is_comment_char(char c);

	void	_skip_comment();

	std::string			_line;
	size_t				_pos;
	std::vector<Token>	_vect;
	std::ifstream		_istream;
	size_t				_token_pos;
};

#endif
