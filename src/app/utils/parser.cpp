/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 15:06:05 by etran             #+#    #+#             */
/*   Updated: 2023/05/07 22:49:42 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.hpp"
// TODO Remove
#include <iostream>
#include "utils.hpp"

namespace scop {
namespace obj {

/* ========================================================================== */
/*                                   PUBLIC                                   */
/* ========================================================================== */

scop::Model	Parser::parseFile(const std::string& file_name) {
	std::ifstream	file;

	file.open(file_name);
	if (!file.is_open()) {
		throw std::invalid_argument("Could not open file " + file_name);
	}

	for (size_t current_line = 0; file.good(); ++current_line) {
		std::getline(file, line);

		try {
			processLine();
		} catch (const Parser::parse_error& error) {
			throw std::invalid_argument(
				"Error while parsing '" +
				file_name +
				"' at line " +
				std::to_string(current_line) +
				": " +
				error.what()
			);
		}
	}

	return model_output;
}

/* ========================================================================== */
/*                                   PRIVATE                                  */
/* ========================================================================== */

void	Parser::processLine() {
	if (line.empty()) {
		return;
	}

	// Check line beginning
	current_pos = 0;
	getNextToken();

	for (size_t i = 0; i < NB_LINE_TYPES; ++i) {
		if (token == line_begin[i]) {
			return (this->*parseLineFun[i])();
		}
	}
	throw Parser::parse_error("undefined line type");
}

void	Parser::parseCoords() {
}

void	Parser::parseFace() {
}

void	Parser::skipComment() {
}

/* ========================================================================== */

/**
 * Retrieve word.
 * Returns false if line is empty.
*/
bool	Parser::getNextToken() {
	if (current_pos == std::string::npos) {
		return false;
	}
	size_t	next_word_pos = line.find_first_of(cs_whitespaces, current_pos);
	token = line.substr(current_pos, next_word_pos - current_pos);
	current_pos = next_word_pos;
	return true;
}

void	Parser::skipWhitespace() {
	current_pos = line.find_first_not_of(cs_whitespaces, current_pos);
}

void	Parser::parseNumber() {
	
}

} // namespace obj
} // namespace scop

// TODO remove
int main() {
	scop::obj::Parser	parser;

	try {
	parser.parseFile("testfile");
	} catch (const std::exception& e) {
		std::cerr << e.what() <<__NL;
	}
}