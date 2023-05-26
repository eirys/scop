/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mtl_parser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/26 13:32:56 by etran             #+#    #+#             */
/*   Updated: 2023/05/26 15:13:07 by etran            ###   ########.fr       */
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
		} catch (const MtlParser::parse_error& error) {
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
	
}

void	MtlParser::skipComment() noexcept {
	current_pos = std::string::npos;
}

/**
 * Retrieve word (chunk of non-whitespace character),
 * or up until the charset parameter.
 * Returns false if line is empty.
*/
bool	MtlParser::getWord() {
	if (current_pos == std::string::npos) {
		return false;
	}
	size_t	end_pos = line.find_first_of(whitespaces, current_pos);
	token = line.substr(current_pos, end_pos - current_pos);
	current_pos = end_pos;
	return true;
}


} // namespace mtl
} // namespace scop