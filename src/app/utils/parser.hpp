/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 15:02:06 by etran             #+#    #+#             */
/*   Updated: 2023/05/07 22:49:29 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

// Std
# include <fstream>
# include <string>

// # include "model.hpp"

# define NB_LINE_TYPES 4

namespace scop {

// TODO remove
class Model {};

namespace obj {

/**
 * List of possible token type in .obj file.
*/
enum TokenType {
	TOKEN_INT,
	TOKEN_FLOAT,
	TOKEN_STRING,
	TOKEN_HASH,
	TOKEN_SLASH,
	TOKEN_WHITESPACE,
	TOKEN_UNKNOWN
};

/**
 * Parser for .obj files.
*/
class Parser {
public:
	typedef		enum TokenType					TokenType;
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
		"#",
		"v",
		"vt",
		"f"
	};

	ParseFunction		parseLineFun[NB_LINE_TYPES] = {
		&Parser::skipComment,
		&Parser::parseCoords,
		&Parser::parseCoords,
		&Parser::parseFace
	};

	// const char*		line_vertex_cooord		= "v";
	// const char*		line_texture_coord		= "vt";
	// const char*		line_face_descriptor	= "f";
	// const char*	line_normal_coord;			= "vn"
	// const char*	line_material_descriptor;	= "mtllib, newmtl, usemtl"
	// const char*	line_option;				= "s (smooth shading)"
	// const char*	line_group_name;			= "o (object name), g (group name) ..."

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	Parser() = default;
	~Parser() = default;
	Parser(const Parser& x) = delete;
	Parser&	operator=(const Parser& x) = delete;

	/* ========================================================================= */

	scop::Model			parseFile(const std::string& file_name);

	/* ========================================================================= */
	/*                                 EXCEPTION                                 */
	/* ========================================================================= */

	class parse_error: public std::exception {
		public:
			parse_error() : error_msg("undefined error") {}
			parse_error(const std::string& error_msg): error_msg(error_msg) {}

			const char* what() const throw() {
				return error_msg.c_str();
			}

		private:
			const std::string	error_msg;
	};

private:
	/* ======================================================================== */
	/*                               CLASS MEMBERS                              */
	/* ======================================================================== */

	scop::Model			model_output;
	std::string			line;
	std::string			token;
	size_t				current_pos = 0;

	/* ======================================================================== */

	void				processLine();
	void				parseCoords();
	void				parseFace();
	void				skipComment();

	bool				getNextToken();
	void				skipWhitespace();
	void				parseNumber();

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