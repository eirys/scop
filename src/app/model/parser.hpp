/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 15:02:06 by etran             #+#    #+#             */
/*   Updated: 2023/05/12 22:59:47 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

// Std
# include <string>

# include "model.hpp"
# include "vertex.hpp"

static constexpr const uint8_t	vertex_bit = 1 << 0;// 1
static constexpr const uint8_t	texture_bit = 1 << 1; // 2
static constexpr const uint8_t	normal_bit = 1 << 2; // 4

static constexpr const size_t	nb_line_types = 10;

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
public:
	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	Parser() = default;
	~Parser() = default;

	Parser(const Parser& x) = delete;
	Parser(Parser&& x) = delete;
	Parser&	operator=(const Parser& x) = delete;

	/* ========================================================================= */

	Model			parseFile(const std::string& file_name);

private:
	/* ========================================================================= */
	/*                                  TYPEDEF                                  */
	/* ========================================================================= */

	typedef		enum TokenType			TokenType;
	typedef		void (Parser::*ParseFunction)();

	/* ======================================================================== */
	/*                               CLASS MEMBERS                              */
	/* ======================================================================== */

	Model				model_output;
	size_t				current_pos;
	std::string			line;
	std::string			token;

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
	const std::string	line_begin[nb_line_types] = {
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

	ParseFunction		parseLineFun[nb_line_types] = {
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
	void				storeTriangles(
		const std::vector<Model::Index>& indices
	);
	void				fixMissingIndices();

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