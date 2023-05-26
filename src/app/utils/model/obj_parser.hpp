/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   obj_parser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 15:02:06 by etran             #+#    #+#             */
/*   Updated: 2023/05/26 13:14:52 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// Std
# include <string>

# include "model.hpp"
# include "vertex.hpp"

static constexpr const uint8_t	vertex_bit = 1 << 0;// 1
static constexpr const uint8_t	texture_bit = 1 << 1; // 2
static constexpr const uint8_t	normal_bit = 1 << 2; // 4

static constexpr const size_t	nb_line_types = 10;

namespace scop {
class Image;

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
 * ObjParser for .obj files.
*/
class ObjParser {
public:
	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	ObjParser() = default;
	ObjParser(ObjParser&& x) = default;
	~ObjParser() = default;

	ObjParser(const ObjParser& x) = delete;
	ObjParser&	operator=(const ObjParser& x) = delete;

	/* ========================================================================= */

	Model			parseFile(
		const std::string& file_name,
		const scop::Image& texture
	);

private:
	/* ========================================================================= */
	/*                                  TYPEDEF                                  */
	/* ========================================================================= */

	typedef		enum TokenType			TokenType;
	typedef		void (ObjParser::*ParseFunction)();

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
		&ObjParser::parseVertex,
		&ObjParser::parseVertex,
		&ObjParser::parseTexture,
		&ObjParser::parseFace,
		&ObjParser::skipComment,
		&ObjParser::skipComment,
		&ObjParser::skipComment,
		&ObjParser::skipComment,
		&ObjParser::skipComment,
		&ObjParser::skipComment
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

	void				checkFile(const std::string& file) const;
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
	void				fixMissingIndices(const scop::Image& img) noexcept;

}; // class ObjParser

} // namespace obj
} // namespace scop