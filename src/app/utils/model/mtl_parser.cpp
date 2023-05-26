/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mtl_parser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/26 13:32:56 by etran             #+#    #+#             */
/*   Updated: 2023/05/27 00:40:39 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mtl_parser.hpp"

#include <fstream> // ifstream

namespace scop {
namespace mtl {

/* ========================================================================== */
/*                                   PUBLIC                                   */
/* ========================================================================== */

Material	MtlParser::parseFile(const std::string& file_name) {
	checkFile(file_name);
	std::ifstream	file(file_name);

	if (!file.is_open()) {
		throw std::invalid_argument("Could not open file");
	}

	for (std::size_t current_line = 0; file.good(); ++current_line) {
		std::getline(file, line);

		try {
			processLine();
		} catch (const base::parse_error& error) {
			throw std::invalid_argument(
				"Error while parsing '" + file_name +
				"' at line " + std::to_string(current_line) + ": " +
				error.what()
			);
		}
	}
	if (file.bad()) {
		throw std::invalid_argument("Error while reading file " + file_name);
	}
	return material_output;
}

/* ========================================================================== */
/*                                   PRIVATE                                  */
/* ========================================================================== */

/**
 * @brief Check if the file path is valid.
*/
void	MtlParser::checkFile(const std::string& path) const {
	if (path.empty()) {
		throw std::invalid_argument("File path is empty");
	}
	const std::size_t	extension_pos = path.rfind('.');
	if (extension_pos == std::string::npos) {
		throw std::invalid_argument("File has no extension");
	} else if (path.substr(extension_pos) != "mtl") {
		throw std::invalid_argument("File extension is not .mtl");
	}
}

void	MtlParser::processLine() {
	if (line.empty()) {
		return;
	}
	current_pos = 0;

	// Check line type.
	getWord();
	
}

/* ========================================================================== */

/**
 * @brief Parses a color, in a format of Vect3.
*/
scop::Vect3	MtlParser::parseColors() {
	scop::Vect3	rgb{};

	for (std::size_t i = 0; i < 3; ++i) {
		if (!getWord())
			throw base::parse_error("Expected 3 values");
		checkNumberType(token);
		rgb[i] = std::stof(token);
		skipWhitespace();
	}
	return rgb;
}

/* ========================================================================== */


} // namespace mtl
} // namespace scop