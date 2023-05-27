/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mtl_parser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/26 13:32:56 by etran             #+#    #+#             */
/*   Updated: 2023/05/28 00:47:01 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mtl_parser.hpp"
#include "utils.hpp"
#include "ppm_loader.hpp"

#include <fstream> // std::ifstream
#include <stdexcept> // std::invalid_argument

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

	for (std::size_t current_line = 1; file.good(); ++current_line) {
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
	} else if (path.substr(extension_pos) != ".mtl") {
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
		if (token == line_begin[i]) {
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
	if (!getWord()) {
		throw base::parse_error("Expected material name");
	}
	material_output.name = token;
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
 * @brief Parses a `Ke` line (emissive color).
 * 
 * @note The line should be in the format `Ke <r> <g> <b>`.
*/
void	MtlParser::parseKe() {
	material_output.emissive_color = parseColors();
}

/**
 * @brief Parses opacity.
 * 
 * @note The line should be in the format `Tr <float>`
 * or `d <float>`.
 * @note If `d` is used, the value is inverted.
*/
void	MtlParser::parseTr() {
	bool	is_d = token == "d";
	if (!getWord())
		throw base::parse_error("Expected transparency value");
	checkNumberType(token);
	float value = std::stof(token);
	material_output.opacity = is_d ? value : 1.0f - value;
}

/**
 * @brief Parses a `Ns` line (shininess exponent).
 * 
 * @note The line should be in the format `Ns <num>`.
*/
void	MtlParser::parseNs() {
	if (!getWord())
		throw base::parse_error("Expected transparency value");
	checkNumberType(token);
	material_output.shininess = static_cast<std::size_t>(std::stof(token));
}

/**
 * @brief Parses a `illum` line (illumination model).
 * 
 * @note The line should be in the format `illum <num>`.
*/
void	MtlParser::parseIllum() {
	if (!getWord())
		throw base::parse_error("Expected transparency value");
	checkNumberType(token);
	const std::size_t	illum = static_cast<std::size_t>(std::stof(token));
	if (illum > 10)
		throw base::parse_error("Illumination model out of range");
	material_output.illum = static_cast<IlluminationModel>(illum);
}

/**
 * @brief Parses a `map_Ka` line (texture filename).
 * 
 * @note - The line should be in the format `map_Ka <filename>`.
 * @note - The texture file name is just the file name.
 * @note - For now, the texture file should be a .ppm file.
*/
void	MtlParser::parseTexture() {
	if (!getWord())
		throw base::parse_error("Expected texture path");

	// Check file format.
	std::size_t	extension_pos = token.rfind('.');
	if (extension_pos == std::string::npos) {
		throw base::parse_error(
			"No extention found for texture file (must be .ppm)"
		);
	} else if (token.find("ppm", extension_pos) == std::string::npos) {
		throw base::parse_error(
			"Texture file must be a ppm file (.ppm)"
		);
	}

	// Load image.
	try {
		scop::PpmLoader	loader(SCOP_TEXTURE_PATH + token);
		material_output.ambient_texture.reset(new scop::Image(loader.load()));
	} catch (const scop::ImageLoader::FailedToLoadImage& e) {
		throw base::parse_error(e.what());
	}
}

void	MtlParser::ignore() {
	if (token != "#") {
		// Line type contains a material component that is not supported.
		LOG(token << " is not supported by this renderer.");
	}
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