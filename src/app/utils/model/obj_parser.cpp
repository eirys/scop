/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   obj_parser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 15:06:05 by etran             #+#    #+#             */
/*   Updated: 2023/05/29 00:38:30 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "obj_parser.hpp"
#include "utils.hpp"	// LOG
#include "mtl_parser.hpp"
#include "ppm_loader.hpp"
#include "material.hpp"

#include <fstream>		// std::ifstream
#include <vector>		// std::vector
#include <optional>		// std::optional
#include <algorithm>	// std::count

namespace scop {
namespace obj {

/* ========================================================================== */
/*                                   PUBLIC                                   */
/* ========================================================================== */

Model	ObjParser::parseFile(const std::string& file_name) {
	checkFile(file_name);
	std::ifstream	file;

	file.open(file_name);
	if (!file.is_open()) {
		throw std::invalid_argument("Could not open file " + file_name);
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

	// Make sure the model is valid
	fixMissingComponents();
	checkMtl();

	return std::move(model_output);
}

/* ========================================================================== */
/*                                   PRIVATE                                  */
/* ========================================================================== */

void	ObjParser::checkFile(const std::string& file) const {
	if (file.empty()) {
		throw std::invalid_argument("Empty file name");
	}
	std::size_t	extension_pos = file.rfind('.');

	if (extension_pos == std::string::npos) {
		throw std::invalid_argument("File '" + file + "' has no extension");
	} else if (file.substr(extension_pos) != ".obj") {
		throw std::invalid_argument("File '" + file + "' is not a .obj file");
	}
}

void	ObjParser::processLine() {
	if (line.empty()) {
		return;
	}
	current_pos = 0;

	// Check line type
	getWord();
	if (token[0] == '#') {
		return;
	}
	for (std::size_t i = 0; i < nb_line_types; ++i) {
		if (token == line_begin[i]) {
			// Parse the line
			skipWhitespace();
			try {
				(this->*parseLineFun[i])();
			} catch (const std::out_of_range& oor) {
				throw base::parse_error("value overflow");
			}
			// Verify that the line is finished or contains comments
			if (current_pos != std::string::npos && line[current_pos] != '#') {
				throw base::parse_error("unexpected token");
			}
			return;
		}
	}
	throw base::parse_error("undefined line type");
}

/* ========================================================================== */

/**
 * Format expected:
 * "vx vy vz "
 *
 * Retrieves a vertex.
*/
void	ObjParser::parseVertex() {
	scop::Vect3	vertex{};

	for (std::size_t i = 0; i < 3; ++i) {
		if (!getWord())
			throw base::parse_error("expecting 3 coordinates");
		checkNumberType(token);
		vertex[i] = std::stof(token);
		skipWhitespace();
	}
	model_output.addVertex(vertex);
}

/**
 * Format expected:
 * "vx vy "
 *
 * Retrieves a texture coordinates.
*/
void	ObjParser::parseTexture() {
	scop::Vect2	texture{};

	for (std::size_t i = 0; i < 2; ++i) {
		if (!getWord())
			throw base::parse_error("expecting 2 coordinates");
		checkNumberType(token);
		texture[i] = std::stof(token);
		skipWhitespace();
	}
	model_output.addTexture(texture);
}

/**
 * Format expected:
 * "vx vy vz "
 *
 * Retrieves a normal.
*/
void	ObjParser::parseNormal() {
	scop::Vect3	normal{};

	for (std::size_t i = 0; i < 3; ++i) {
		if (!getWord())
			throw base::parse_error("expecting 3 coordinates");
		checkNumberType(token);
		normal[i] = std::stof(token);
		skipWhitespace();
	}
	model_output.addNormal(normal);
}

/**
 * Format expected:
 * case 100: "v1 v2 v3 {...}"
 * case 110: "v1/vt1 v2/vt2 v3/vt3 {...}"
 * case 101: "v1//vn1 v2//vn2 v3//vn3 {...}"
 * case 111: "v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 {...}"
 *
 * Retrives at least one triangle.
*/
void	ObjParser::parseFace() {
	std::optional<uint8_t>			format;
	std::vector<Model::Index>		indices;

	// Parse all indices chunks
	while (getWord()) {
		// Verify nb indices
		size_t	nb_slashes = std::count(token.begin(), token.end(), '/');
		if (nb_slashes > 2) {
			throw base::parse_error("expecting at most 3 indices");
		}

		// If first chunk, determine format
		if (!format.has_value()) {
			format = getFormat();
		} else if (format.value() != getFormat()) {
			throw base::parse_error("inconsistent format");
		}

		// Extract expected indices from chunk
		Model::Index	index{};
		std::size_t	begin_pos = 0;
		for (std::size_t i = 0; i < 3; ++i) {
			if (format.value() & (1 << i)) {
				std::size_t	end_pos = token.find(cs_slash, begin_pos);
				if (end_pos == std::string::npos) {
					end_pos = token.size();
				}
				std::string	index_str = token.substr(begin_pos, end_pos - begin_pos);
				if (checkNumberType(index_str) != TOKEN_INT) {
					throw base::parse_error("expecting integer index");
				}
				index[i] = std::stoi(index_str);
				begin_pos = end_pos + 1;
			} else if (nb_slashes) {
				begin_pos += 1;
			}
		}
		if (index.vertex == 0) {
			throw base::parse_error("expecting vertex index");
		}
		model_output.addIndex(index);
		indices.emplace_back(index);
		skipWhitespace();
	}

	if (indices.size() < 3) {
		throw base::parse_error("expecting at least 3 vertices");
	}

	// Store the new triangles
	storeTriangles(indices);
}

/**
 * @brief Retrieves the material library file name.
 * 
 * @note - Format expected: "mtllib <filename>"
 * @note - The path is just the name of the file.
 * @note - Only one material library file name is stored.
*/
void	ObjParser::parseMtlPath() {
	if (!getWord()) {
		throw base::parse_error("expecting material filename");
	}
	mtl_path = token;
}

/**
 * @brief Retrieves the material name.
 * 
 * @note - Format expected: "usemtl <name>"
 * @note - Only one material name is stored.
*/
void	ObjParser::parseMtlName() {
	if (!getWord()) {
		throw base::parse_error("expecting material name");
	}
	mtl_name = token;
}

/**
 * @brief Parses smooth shading enable.
 * 
 * @note Format expected: "s <0 | 1 | off | on>"
*/
void	ObjParser::parseSmoothShading() {
	if (!getWord()) {
		throw base::parse_error("expecting smooth shading value");
	}
	// Verify that the smoothing group is valid
	if (token == "1" || token == "on") {
		model_output.toggleSmoothShading();
	} else if (token != "0" && token != "off") {
		throw base::parse_error("expecting 0, 1, off or on");
	}
}

/* ========================================================================== */

void	ObjParser::storeTriangles(
	const std::vector<Model::Index>& indices
) {
	std::size_t	attr_sizes[3] = {
		model_output.getVertexCoords().size(),
		model_output.getTextureCoords().size(),
		model_output.getNormalCoords().size()
	};
	std::size_t	nb_triangles = indices.size() - 2;

	for (std::size_t i = 0; i < nb_triangles; ++i) {
		// Replace occurence of -1 by last element of corresponding list
		auto	selectIndex =
			[indices, attr_sizes](std::size_t pos, std::size_t attr) -> int {
				if (indices[pos][attr] < 0) {
					return attr_sizes[attr] - 1;
				} else {
					return indices[pos][attr] - 1;
				}
			};

		Model::Triangle	triangle{};

		triangle.indices[0] = {
			.vertex = selectIndex(0, 0),
			.texture = selectIndex(0, 1),
			.normal = selectIndex(0, 2)
		};
		triangle.indices[1] = {
			.vertex = selectIndex(i + 1, 0),
			.texture = selectIndex(i + 1, 1),
			.normal = selectIndex(i + 1, 2)
		};
		triangle.indices[2] = {
			.vertex = selectIndex(i + 2, 0),
			.texture = selectIndex(i + 2, 1),
			.normal = selectIndex(i + 2, 2)
		};
		model_output.addTriangle(triangle);
	}
}

void	ObjParser::ignore() noexcept {
	if (token != "#") {
		// Line type contains a material component that is not supported.
		LOG(token << " is not supported by this renderer.");
	}
	return skipComment();
}

uint8_t	ObjParser::getFormat() const noexcept {
	std::size_t	first_slash = token.find(cs_slash);
	std::size_t	last_slash = token.rfind(cs_slash);

	if (first_slash == std::string::npos && last_slash == std::string::npos) {
		return vertex_bit;
	} else if (first_slash == last_slash) {
		return vertex_bit | texture_bit;
	} else if (first_slash == last_slash - 1) {
		return vertex_bit | normal_bit;
	} else {
		return vertex_bit | texture_bit | normal_bit;
	}
}

/**
 * @brief Check if there are missing model components,
 * and add default ones.
*/
void	ObjParser::fixMissingComponents() noexcept {
	if (model_output.getTextureCoords().empty()) {
		model_output.setDefaultTextureCoords();
	}
	if (model_output.getNormalCoords().empty()) {
		model_output.setDefaultNormalCoords();
	}
}

/**
 * @brief Check if the model has a valid material.
 * If not, load a default texture.
*/
void	ObjParser::checkMtl() {
	if (!mtl_path.empty() && !mtl_name.empty()){
		scop::mtl::MtlParser	mtl_parser;
		model_output.setMaterial(mtl_parser.parseFile(SCOP_MTL_PATH + mtl_path));

		if (mtl_name != model_output.getMaterial().name) {
			throw std::invalid_argument("Unknown material: " + mtl_name);
		} else if (
			!model_output.getMaterial().shininess &&
			!model_output.getMaterial().specular_color &&
			model_output.getMaterial().illum == scop::mtl::IlluminationModel::ILLUM_LAMBERT_PHONG
		) {
			throw std::invalid_argument("Specular component is incomplete");
		}
	} else if (mtl_path.empty() && !mtl_name.empty()) {
		throw std::invalid_argument("No library file specified for " + mtl_name);
	} else if (mtl_name.empty() && !mtl_path.empty()) {
		throw std::invalid_argument("No material name specified");
	} else {
		model_output.setMaterial(scop::mtl::Material());
	}
}

} // namespace obj
} // namespace scop