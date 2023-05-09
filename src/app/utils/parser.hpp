/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 15:02:06 by etran             #+#    #+#             */
/*   Updated: 2023/05/09 17:23:42 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

// Std
# include <string>

# include "model.hpp"
# include "vertex.hpp"

# define VERTEX			1 << 2 // 4
# define TEXTURE		1 << 1 // 2
# define NORMAL			1 << 0 // 1

# define NB_LINE_TYPES	10

namespace scop {
namespace obj {

/**
 * List of possible token type in .obj file.
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
 * Parser for .obj files.
*/
class Parser {
private:
	typedef		enum TokenType			TokenType;
	typedef		void (Parser::*ParseFunction)();

	/* ========================================================================= */
	/*                               CONST MEMBERS                               */
	/* ========================================================================= */

	/**
	 * Charsets.
	*/
	const std::string	cs_digit		= "0123456789";
	const std::string	cs_dot			= ".";
	const std::string	cs_negate		= "-";
	const std::string	cs_lowercase	= "abcdefghijklmnopqrstuvwxyz";
	const std::string	cs_uppercase	= "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	const std::string	cs_hash			= "#";
	const std::string	cs_slash		= "/";
	const std::string	cs_whitespaces	= " \t";

	/**
	 * List of possible line type in .obj file.
	*/
	const std::string	line_begin[NB_LINE_TYPES] = {
		"v",
		"vn",
		"vt",
		"f",
		"#",
		"mtllib",	// TODO
		"usemtl", 	// TODO
		"o",		// TODO
		"g",		// TODO
		"s"			// TODO
	};

	ParseFunction		parseLineFun[NB_LINE_TYPES] = {
		&Parser::parseVertex,
		&Parser::parseVertex,
		&Parser::parseTexture,
		&Parser::parseFace,
		&Parser::skipComment,
		&Parser::skipComment,
		&Parser::skipComment,
		&Parser::skipComment,
		&Parser::skipComment,
		&Parser::skipComment
	};

public:
	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	Parser() = default;
	~Parser() = default;
	Parser(const Parser& x) = delete;
	Parser&	operator=(const Parser& x) = delete;

	/* ========================================================================= */

	scop::Model			parseFile(const std::string& file_name);
	
private:
	/* ========================================================================= */
	/*                                 EXCEPTION                                 */
	/* ========================================================================= */

	class parse_error: public std::exception {
		public:
			parse_error(): error_msg("undefined error") {}
			parse_error(const std::string& error_msg): error_msg(error_msg) {}

			const char* what() const noexcept override {
				return error_msg.c_str();
			}

		private:
			const std::string	error_msg;
	};

	/* ======================================================================== */
	/*                               CLASS MEMBERS                              */
	/* ======================================================================== */

	scop::Model			model_output;
	size_t				current_pos;
	std::string			line;
	std::string			token;

	/* ======================================================================== */

	void				processLine();

	void				parseVertex();
	void				parseTexture();
	void				parseNormal();
	void				parseFace();

	bool				getWord();
	void				skipComment() noexcept;
	void				skipWhitespace() noexcept;
	TokenType			checkNumberType(const std::string& word) const;
	void				checkJunkAfterNumber(
		const std::string& word,
		size_t pos
	) const;
	uint8_t				getFormat() const noexcept;

}; // class Parser

} // namespace obj

namespace mtl {

/**
 * Parser for .mtl files.
*/
class Parser {
}; // class Parser

} // namespace mtl
} // namespace scop

#endif