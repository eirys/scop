/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 15:02:06 by etran             #+#    #+#             */
/*   Updated: 2023/05/06 23:55:25 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

// Std
# include <fstream>
# include <string>

# include "model.hpp"

namespace scop {

enum TokenType {

};

enum LineType {
	VERTEX_COOORD,
	TEXTURE_COORD,
	NORMAL_COORD,
	MATERIAL_NAME,
	GROUP_NAME,
	UNKNOWN
};

/**
 * Parser for .obj files.
*/
class Parser {
public:
	typedef		enum TokenType		TokenType;
	typedef		enum LineType		LineType;

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
	/* ======================================================================== */
	/*                               CLASS MEMBERS                              */
	/* ======================================================================== */

	std::string			line;
	std::string			token;
	size_t				current_pos = 0;
	size_t				current_line = 0;

	/* ======================================================================== */

	bool				processLine(Model& model);
	void				getLineType();
	void				getNextToken(TokenType type);
	void				skipWhitespace();

};

} // namespace scop

#endif