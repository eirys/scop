/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 15:06:05 by etran             #+#    #+#             */
/*   Updated: 2023/05/14 13:49:27 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.hpp"
#include "utils.hpp"	// LOG

#include <fstream>		// ifstream
#include <vector>		// vector
#include <optional>		// optional
#include <algorithm>	// count

namespace scop {
namespace obj {

/* ========================================================================== */
/*                                   PUBLIC                                   */
/* ========================================================================== */

Model	Parser::parseFile(const std::string& file_name) {
	checkFile(file_name);
	std::ifstream	file;

	file.open(file_name);
	if (!file.is_open()) {
		throw std::invalid_argument("Could not open file " + file_name);
	}

	for (size_t current_line = 1; file.good(); ++current_line) {
		std::getline(file, line);

		try {
			processLine();
		} catch (const Parser::parse_error& error) {
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
	// Fix empty indices in face
	fixMissingIndices();

	return model_output;
}

/* ========================================================================== */
/*                                   PRIVATE                                  */
/* ========================================================================== */

void	Parser::checkFile(const std::string& file) const {
	if (file.empty()) {
		throw std::invalid_argument("Empty file name");
	}
	size_t	extension_pos = file.rfind('.');
	
	if (extension_pos == std::string::npos) {
		throw std::invalid_argument("File '" + file + "' has no extension");
	} else if (file.substr(extension_pos) != ".obj") {
		throw std::invalid_argument("File '" + file + "' is not a .obj file");
	}
}

void	Parser::processLine() {
	if (line.empty()) {
		return skipComment();
	}
	current_pos = 0;

	// Check line type
	getWord();
	for (size_t i = 0; i < nb_line_types; ++i) {
		if (token == line_begin[i]) {
			skipWhitespace();
			try {
				(this->*parseLineFun[i])();
			} catch (const std::out_of_range& oor) {
				throw Parser::parse_error("value overflow");
			}
			if (current_pos != std::string::npos) {
				if (line[current_pos] == '#') {
					skipComment();
				} else {
					throw Parser::parse_error("unexpected token");
				}
			}
			return;
		}
	}
	throw Parser::parse_error("undefined line type");
}

/**
 * Format expected:
 * "vx vy vz "
 *
 * Retrieves a vertex.
*/
void	Parser::parseVertex() {
	scop::Vect3	vertex{};

	for (size_t i = 0; i < 3; ++i) {
		if (!getWord())
			throw Parser::parse_error("expecting 3 coordinates");
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
void	Parser::parseTexture() {
	scop::Vect2	texture{};

	for (size_t i = 0; i < 2; ++i) {
		if (!getWord())
			throw Parser::parse_error("expecting 2 coordinates");
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
void	Parser::parseNormal() {
	scop::Vect3	normal{};

	for (size_t i = 0; i < 3; ++i) {
		if (!getWord())
			throw Parser::parse_error("expecting 3 coordinates");
		checkNumberType(token);
		normal[i] = std::stof(token);
		skipWhitespace();
	}
	model_output.addNormal(normal);
}

/**
 * Format expected:
 * case 100: "	v1 v2 v3 {...}"
 * case 110: "	v1/vt1 v2/vt2 v3/vt3 {...}"
 * case 101: "	v1//vn1 v2//vn2 v3//vn3 {...}"
 * case 111: "	v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 {...}"
 *
 * Retrives at least one triangle.
*/
void	Parser::parseFace() {
	std::optional<uint8_t>			format;
	std::vector<Model::Index>		indices;

	// Parse all indices chunks
	skipWhitespace();
	while (getWord()) {
		// Verify nb indices
		size_t	nb_slashes = std::count(token.begin(), token.end(), '/');
		if (nb_slashes > 2) {
			throw Parser::parse_error("expecting at most 3 indices");
		}

		// If first chunk, determine format
		if (!format.has_value()) {
			format = getFormat();
		} else if (format.value() != getFormat()) {
			throw Parser::parse_error("inconsistent format");
		}

		// Extract expected indices from chunk
		Model::Index	index{};
		size_t	begin_pos = 0;
		for (size_t i = 0; i < 3; ++i) {
			if (format.value() & (1 << i)) {
				size_t	end_pos = token.find(cs_slash, begin_pos);
				if (end_pos == std::string::npos) {
					end_pos = token.size();
				}
				std::string	index_str = token.substr(begin_pos, end_pos - begin_pos);
				if (checkNumberType(index_str) != TOKEN_INT) {
					throw Parser::parse_error("expecting integer index");
				}
				index[i] = std::stoi(index_str);
				begin_pos = end_pos + 1;
			} else if (nb_slashes) {
				begin_pos += 1;
			}
		}
		if (index.vertex == 0) {
			throw Parser::parse_error("expecting vertex index");
		}
		model_output.addIndex(index);
		indices.emplace_back(index);
		skipWhitespace();
	}

	if (indices.size() < 3) {
		throw Parser::parse_error("expecting at least 3 vertices");
	}

	// Store triangles
	storeTriangles(indices);
}

/* ========================================================================== */

/**
 * Retrieve word (chunk of non-whitespace character),
 * or up until the charset parameter.
 * Returns false if line is empty.
*/
bool	Parser::getWord() {
	if (current_pos == std::string::npos) {
		return false;
	}
	size_t	end_pos = line.find_first_of(cs_whitespaces, current_pos);
	token = line.substr(current_pos, end_pos - current_pos);
	current_pos = end_pos;
	return true;
}

void	Parser::skipComment() noexcept {
	current_pos = std::string::npos;
}

void	Parser::skipWhitespace() noexcept {
	current_pos = line.find_first_not_of(cs_whitespaces, current_pos);
}

Parser::TokenType	Parser::checkNumberType(const std::string& word) const {
	if (word.empty()) {
		throw Parser::parse_error("expecting number");
	}

	size_t	pos_checked = word.find(cs_negate);

	// Check if first negate
	if (pos_checked != std::string::npos && pos_checked != 0) {
		throw Parser::parse_error("unexpected '-' character");
	} else if (pos_checked == 0) {
		pos_checked += 1;
	} else {
		pos_checked = 0;
	}

	// Check if there are only digits
	pos_checked = word.find_first_of(cs_digit, pos_checked);
	if (pos_checked == std::string::npos) {
		throw Parser::parse_error("expecting digits after '-'");
	}

	// Check if there's a dot after digits
	size_t	dot_pos = word.find(cs_dot, pos_checked);
	if (dot_pos == std::string::npos) {
		checkJunkAfterNumber(word, pos_checked);
		return TokenType::TOKEN_INT;
	}
	checkJunkAfterNumber(word, dot_pos + 1);
	return TokenType::TOKEN_FLOAT;
}

/**
 * Check if there's junk after digits in word.
*/
void	Parser::checkJunkAfterNumber(const std::string& word, size_t pos) const {
	if (word.find_first_not_of(cs_digit, pos) != std::string::npos) {
		throw Parser::parse_error("unexpected character after value");
	}
}

uint8_t	Parser::getFormat() const noexcept {
	size_t	first_slash = token.find(cs_slash);
	size_t	last_slash = token.rfind(cs_slash);

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

void	Parser::storeTriangles(
	const std::vector<Model::Index>& indices
) {
	size_t	attr_sizes[3] = {
		model_output.getVertexCoords().size(),
		model_output.getTextureCoords().size(),
		model_output.getNormalCoords().size()
	};
	size_t	nb_triangles = indices.size() - 2;

	for (size_t i = 0; i < nb_triangles; ++i) {

		// Replace occurence of -1 by last element of corresponding list
		auto	selectIndex =
			[indices, attr_sizes](size_t pos, size_t attr) -> int {
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

/**
 * Check if there are missing indices,
 * and add default ones.
*/
void	Parser::fixMissingIndices() {
	if (model_output.getTextureCoords().empty()) {
		model_output.setDefaultTextureCoords();
	}
	if (model_output.getNormalCoords().empty()) {
		model_output.setDefaultNormalCoords();
	}
}

} // namespace obj
} // namespace scop