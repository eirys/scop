/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mtl_parser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/26 13:15:11 by etran             #+#    #+#             */
/*   Updated: 2023/05/27 00:43:34 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "material.hpp"
# include "parser.hpp"

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

	Material			material_output;

	/* ========================================================================= */
	/*                               CONST MEMBERS                               */
	/* ========================================================================= */

	static constexpr
	const std::size_t	nb_line_size = 8;

	const std::string	line_begin[nb_line_size] = {
		"#",
		"newmtl",
		"Ka",
		"Kd",
		"Ks",
		"Tr",
		"Ns",
		"illum"
	};

	const ParseFunction	parseLineFn[nb_line_size] = {
		
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
	void				parseTr();
	void				parseNs();
	scop::Vect3			parseColors();

}; // class MtlParser

} // namespace mtl
} // namespace scop