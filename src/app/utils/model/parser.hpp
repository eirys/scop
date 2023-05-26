/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/26 23:30:12 by etran             #+#    #+#             */
/*   Updated: 2023/05/26 23:45:10 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <string>

namespace scop {

/**
 * List of possible token type in the files.
*/
enum TokenType {
	TOKEN_INT,
	TOKEN_FLOAT,
	// TOKEN_STRING,
	// TOKEN_HASH,
	// TOKEN_SLASH,
	// TOKEN_UNKNOWN
};

/**
 * Abstract class for parsers.
*/
class Parser {
protected:
	/* ========================================================================= */
	/*                               CONST MEMBERS                               */
	/* ========================================================================= */

	/**
	 * Charsets.
	*/
	const std::string	cs_digit		= "0123456789";
	const std::string	cs_dot			= ".";
	const std::string	cs_negate		= "-";
	const std::string	cs_slash		= "/";
	const std::string	cs_whitespaces	= " \t";

	/* ========================================================================= */
	/*                                 EXCEPTION                                 */
	/* ========================================================================= */

	class parse_error: public std::exception {
		public:
			parse_error() = delete;
			parse_error(const std::string& error_msg): error_msg(error_msg) {}

			const char* what() const noexcept override {
				return error_msg.c_str();
			}

		private:
			const std::string	error_msg;
	};

	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	size_t				current_pos;
	std::string			line;
	std::string			token;

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	Parser() = default;
	Parser(Parser&& x) = default;
	~Parser() = default;

	Parser(const Parser& x) = delete;
	Parser&	operator=(const Parser& x) = delete;

	/* ========================================================================= */

	virtual void		checkFile(const std::string& file) const = 0;
	virtual void		processLine() = 0;

	bool				getWord();
	void				skipComment() noexcept;
	void				skipWhitespace() noexcept;
	TokenType			checkNumberType(const std::string& word) const;
	void				checkJunkAfterNumber(
		const std::string& word,
		size_t pos
	) const;

}; // class Parser

} // namespace scop