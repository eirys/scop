/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   obj_parser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 15:02:06 by etran             #+#    #+#             */
/*   Updated: 2023/05/28 01:07:56 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// Std
# include <string> // std::string

# include "model.hpp"
# include "vertex.hpp"
# include "parser.hpp"

namespace scop {
class Image;

namespace obj {

// TODO:
// - [ ] Add support for multiple objects in one file. (o <name>)
// - [ ] Add support for multiple groups in one file. (g <name>)
// - [ ] Add support for multiple mtllib in one file. (mtllib <name>)
// - [ ] Add support for multiple materials in one file. (usemtl <name>)
// - [ ] Add support for smoothing groups. (s <group>)

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

	std::string			mtl_path;
	std::string			mtl_name;

	/* ========================================================================= */
	/*                               CONST MEMBERS                               */
	/* ========================================================================= */

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
		"#",
		"v",
		"vn",
		"vt",
		"f",
		"mtllib",
		"usemtl",
		"s",
		"o",		// TODO
		"g"			// TODO
	};

	const ParseFunction		parseLineFun[nb_line_types] = {
		&ObjParser::ignore,
		&ObjParser::parseVertex,
		&ObjParser::parseNormal,
		&ObjParser::parseTexture,
		&ObjParser::parseFace,
		&ObjParser::parseMtlPath,
		&ObjParser::parseMtlName,
		&ObjParser::parseSmoothShading,
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
	void				parseMtlPath();
	void				parseMtlName();
	void				parseSmoothShading();

	void				storeTriangles(const std::vector<Model::Index>& indices);
	void				ignore() noexcept;
	uint8_t				getFormat() const noexcept;
	void				fixMissingIndices() noexcept;
	void				checkMtl();

}; // class ObjParser

} // namespace obj
} // namespace scop