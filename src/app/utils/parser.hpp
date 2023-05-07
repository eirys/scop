/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 15:02:06 by etran             #+#    #+#             */
/*   Updated: 2023/05/07 08:24:46 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

// Std
# include <fstream>
# include <string>

# include "model.hpp"

namespace scop {
namespace obj {

enum SymbolType {
	
};

enum TokenType {
	INT,
	FLOAT,
	CHARACTER,
	STRING,
	SYMBOL,
	UNDEFINED
};

enum LineType {
	VERTEX_COOORD,
	TEXTURE_COORD,
	NORMAL_COORD,
	COMMAND_NAME,
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
	LineType			getLineType();
	void				getNextToken(TokenType type);
	void				skipWhitespace();

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