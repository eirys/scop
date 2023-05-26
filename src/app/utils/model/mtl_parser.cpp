/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mtl_parser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/26 13:32:56 by etran             #+#    #+#             */
/*   Updated: 2023/05/27 01:31:25 by etran            ###   ########.fr       */
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
	for (std::size_t i = 0; i < nb_line_size; ++i) {
		if (line == line_begin[i]) {
			skipWhitespace();
			try {
				(this->*parseLineFn[i])();
			} catch (const std::out_of_range& oor) {
				throw base::parse_error("Value overflow");
			}
			if (current_pos != std::string::npos) {
				if (line[current_pos] == '#') {
					skipComment();
				} else {
					throw base::parse_error("unexpected token");
				}
			}
			return;
		}
	}
	throw base::parse_error("unknown line type");
}

/* ========================================================================== */

/**
 * @brief Parses a `newmtl` line (name).
 * 
 * @note The line should be in the format `newmtl <name>`.
*/
void	MtlParser::parseNewmtl() {
	material_output.name = getWord();
}

/**
 * @brief Parses a `Ka` line (ambient color).
 * 
 * @note The line should be in the format `Ka <r> <g> <b>`.
*/
void	MtlParser::parseKa() {
	material_output.ambient_color = parseColors();
}

/**
 * @brief Parses a `Kd` line (diffuse color).
 * 
 * @note The line should be in the format `Kd <r> <g> <b>`.
*/
void	MtlParser::parseKd() {
	material_output.diffuse_color = parseColors();
}

/**
 * @brief Parses a `Ks` line (specular color).
 * 
 * @note The line should be in the format `Ks <r> <g> <b>`.
*/
void	MtlParser::parseKs() {
	material_output.specular_color = parseColors();
}

/**
 * @brief Parses transparency.
 * 
 * @note The line should be in the format `d <float>`.
*/
void	MtlParser::parseTr() {
	if (!getWord())
		throw base::parse_error("Expected transparency value");
	if (checkNumberType(token) != TokenType::TOKEN_FLOAT)
		throw base::parse_error("Expected float value");
	material_output.transparency = std::stof(token);
}

/**
 * @brief Parses a `Ns` line (shininess exponent).
 * 
 * @note The line should be in the format `Ns <int>`.
*/
void	MtlParser::parseNs() {
	if (!getWord())
		throw base::parse_error("Expected transparency value");
	if (checkNumberType(token) != TokenType::TOKEN_INT)
		throw base::parse_error("Expected integer value");
	material_output.shininess = std::stoul(token);
}

/**
 * @brief Parses a `illum` line (illumination model).
 * 
 * @note The line should be in the format `illum <int>`.
*/
void	MtlParser::parseIllum() {
	if (!getWord())
		throw base::parse_error("Expected transparency value");
	if (checkNumberType(token) != TokenType::TOKEN_INT)
		throw base::parse_error("Expected integer value");
	const std::size_t	illum = std::stoul(token);
	if (illum > 10)
		throw base::parse_error("Illumination model out of range");
	material_output.illum = static_cast<IlluminationModel>(illum);
}

void	MtlParser::ignore() noexcept {
	return skipComment();
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

} // namespace mtl
} // namespace scop