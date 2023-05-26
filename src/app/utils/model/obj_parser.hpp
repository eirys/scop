/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   obj_parser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 15:02:06 by etran             #+#    #+#             */
/*   Updated: 2023/05/27 00:03:46 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// Std
# include <string>

# include "model.hpp"
# include "vertex.hpp"
# include "parser.hpp"

namespace scop {
class Image;

namespace obj {

/**
 * ObjParser for .obj files.
*/
class ObjParser: private scop::Parser {
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

	Model			parseFile(const std::string& file_name);

private:
	/* ========================================================================= */
	/*                                  TYPEDEF                                  */
	/* ========================================================================= */

	typedef		scop::Parser	base;
	typedef		void (ObjParser::*ParseFunction)();

	/* ======================================================================== */
	/*                               CLASS MEMBERS                              */
	/* ======================================================================== */

	Model				model_output;

	// /* ========================================================================= */
	// /*                               CONST MEMBERS                               */
	// /* ========================================================================= */

	/**
	 * Bitmask for indice type.
	*/
	const uint8_t		vertex_bit = 1 << 0;// 1
	const uint8_t		texture_bit = 1 << 1; // 2
	const uint8_t		normal_bit = 1 << 2; // 4

	static constexpr
	const std::size_t	nb_line_types = 10;
	
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

	const ParseFunction		parseLineFun[nb_line_types] = {
		&ObjParser::parseVertex,
		&ObjParser::parseNormal,
		&ObjParser::parseTexture,
		&ObjParser::parseFace,
		&ObjParser::ignore,
		&ObjParser::ignore,
		&ObjParser::ignore,
		&ObjParser::ignore,
		&ObjParser::ignore,
		&ObjParser::ignore
	};

	/* ======================================================================== */

	void				checkFile(const std::string& file) const override;
	void				processLine() override;

	void				parseVertex();
	void				parseTexture();
	void				parseNormal();
	void				parseFace();
	void				storeTriangles(const std::vector<Model::Index>& indices);
	void				ignore() noexcept;

	uint8_t				getFormat() const noexcept;

	void				fixMissingIndices() noexcept;

}; // class ObjParser

} // namespace obj
} // namespace scop