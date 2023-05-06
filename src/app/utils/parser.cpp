/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 15:06:05 by etran             #+#    #+#             */
/*   Updated: 2023/05/06 23:55:34 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.hpp"

namespace scop {

/* ========================================================================== */
/*                                   PUBLIC                                   */
/* ========================================================================== */

Model	Parser::parseFile(const std::string& file_name) {
	std::ifstream	file;
	Model			model_object;

	file.open(file_name);
	if (!file.is_open()) {
		throw std::invalid_argument("Could not open file " + file_name);
	}

	while (file.good()) {
		std::getline(file, line);
		if (!processLine(model_object)) {
			throw std::invalid_argument(
				"Error parsing file " +
				file_name +
				" at line " +
				std::to_string(current_line)
			);
		}
		++current_line;
	}

	return model_object;
}

/* ========================================================================== */
/*                                   PRIVATE                                  */
/* ========================================================================== */

bool	Parser::processLine(Model& model) {
	current_pos = 0;

	LineType	line_type = getLineType();
	if (line_type == UNKNOWN) {
		return false;
	}

	skipWhitespace();
	return true;
}

void	Parser::getLineType() {

}

void	Parser::getNextToken(TokenType type) {
	// token = line.substr(current_pos);
}

void	Parser::skipWhitespace() {
	static constexpr char*	whitespaces = " \t";
	current_pos = line.find_first_not_of(whitespaces, current_pos);
}

} // namespace scop