/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mtl_parser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/26 13:15:11 by etran             #+#    #+#             */
/*   Updated: 2023/05/28 00:42:21 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "material.hpp"
# include "parser.hpp"

# define SCOP_TEXTURE_PATH "assets/textures/"
# define SCOP_MTL_PATH "assets/materials/"
# define SCOP_TEXTURE_FILE_DEFAULT "assets/textures/hammy.ppm"

namespace scop {
namespace mtl {

/**
 * MtlParser for .mtl files.
*/
class MtlParser: private scop::Parser {
public:
	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	MtlParser() = default;
	MtlParser(MtlParser&& x) = default;
	~MtlParser() = default;

	MtlParser(const MtlParser& x) = delete;
	MtlParser&	operator=(const MtlParser& x) = delete;

	/* ========================================================================= */

	/**
	 * @brief Parse a .mtl file.
	 *
	 * @param path Path to the .mtl file.
	*/
	Material			parseFile(const std::string& path);

private:
	/* ========================================================================= */
	/*                                  TYPEDEFS                                 */
	/* ========================================================================= */

	typedef		scop::Parser		base;
	typedef		void (MtlParser::*ParseFunction)();

	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	Material			material_output{};

	/* ========================================================================= */
	/*                               CONST MEMBERS                               */
	/* ========================================================================= */

	static constexpr
	const std::size_t	nb_line_size = 21;

	const std::string	line_begin[nb_line_size] = {
		"#",
		"newmtl",
		"Ka",
		"Kd",
		"Ks",
		"Ke",
		"Tr",
		"d",
		"Ns",
		"illum",
		"map_Ka",

		// Unused
		"Ni",
		"map_Kd",
		"map_Ks",
		"map_Ke",
		"map_d",
		"map_bump",
		"bump",
		"disp",
		"decal",
		"refl"
	};

	const ParseFunction	parseLineFn[nb_line_size] = {
		&MtlParser::ignore,
		&MtlParser::parseNewmtl,
		&MtlParser::parseKa,
		&MtlParser::parseKd,
		&MtlParser::parseKs,
		&MtlParser::parseKe,
		&MtlParser::parseTr,
		&MtlParser::parseTr,
		&MtlParser::parseNs,
		&MtlParser::parseIllum,
		&MtlParser::parseTexture,

		&MtlParser::ignore,
		&MtlParser::ignore,
		&MtlParser::ignore,
		&MtlParser::ignore,
		&MtlParser::ignore,
		&MtlParser::ignore,
		&MtlParser::ignore,
		&MtlParser::ignore,
		&MtlParser::ignore,
		&MtlParser::ignore
	};

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	void				checkFile(const std::string& path) const override;
	void				processLine() override;

	void				parseNewmtl();
	void				parseKa();
	void				parseKd();
	void				parseKs();
	void				parseKe();
	void				parseTr();
	void				parseNs();
	void				parseIllum();
	void				parseTexture();

	void				ignore();

	scop::Vect3			parseColors();


}; // class MtlParser

} // namespace mtl
} // namespace scop