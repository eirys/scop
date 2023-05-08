/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 15:06:05 by etran             #+#    #+#             */
/*   Updated: 2023/05/08 18:25:51 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.hpp"
#include <memory>

// TODO Remove
#include <iostream>
#include "utils.hpp"
#include <vector>
#include <optional>
#include <algorithm>

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

	for (size_t current_line = 1; file.good(); ++current_line) {
		std::getline(file, line);

		try {
			processLine();
			if (current_pos != std::string::npos) {
				throw Parser::parse_error("unexpected token");
			}
		} catch (const Parser::parse_error& error) {
			throw std::invalid_argument(
				"Error while parsing '" + file_name +
				"' at line " + std::to_string(current_line) + ": " +
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
		return skipComment();
	}

	current_pos = 0;
	getWord();
	for (size_t i = 0; i < NB_LINE_TYPES; ++i) {
		if (token == line_begin[i]) {
			return (this->*parseLineFun[i])();
		}
	}
	throw Parser::parse_error("undefined line type");
}

/**
 * Format expected:
 * "	vx vy vz"
*/
void	Parser::parseVertex() {
	scop::Vect3	vertex;

	for (size_t i = 0; i < 3; ++i) {
		skipWhitespace();
		if (!getWord())
			throw Parser::parse_error("expecting 3 coordinates");
		checkNumberType(token);
		vertex[i] = std::stof(token);
		// LOG(vertex[i]);
	}
	// model_output.addVertex(vertex);
}

/**
 * Format expected:
 * "	vx vy"
 * Retrieves a texture coordinates.
*/
void	Parser::parseTexture() {
	scop::Vect2	texture;

	for (size_t i = 0; i < 2; ++i) {
		skipWhitespace();
		if (!getWord())
			throw Parser::parse_error("expecting 2 coordinates");
		checkNumberType(token);
		texture[i] = std::stof(token);
		// LOG(texture[i]);
	}
	// model_output.addTexture(texture);
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
	std::vector<std::array<uint32_t, 3>>	indices;

	std::optional<size_t>	nb_indices;
	std::optional<uint8_t>	format;

	// Parse all indices chunks
	while (current_pos != std::string::npos) {
		skipWhitespace();
		getWord();

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
		std::array<uint32_t, 3>	index{};

		size_t	begin_pos = 0;
		for (size_t i = 0; i < 3; ++i) {
			if (format.value() & (1 << i)) {
				size_t	end_pos = token.find(cs_slash, begin_pos);
				std::string	index_str = token.substr(begin_pos, end_pos - begin_pos);
				if (checkNumberType(index_str) != TOKEN_INT) {
					throw Parser::parse_error("expecting integer index");
				}
				index[i] = std::stoul(index_str);
				begin_pos = end_pos + 1;
			} else {
				begin_pos += 1;
			}
		}
		indices.emplace_back(index);
	}

	if (indices.size() < 3) {
		throw Parser::parse_error("expecting at least 3 vertices");
	}

	for (const auto& index: indices) {
		LOG(index[0] << " " << index[1] << " " << index[2]);
	}

// 	size_t	nb_triangles = indices.size() - 2;
// 	for (size_t i = 0; i < nb_triangles; ++i) {
// 		Model::Triangle	triangle{};

// 		triangle.vertex_indices[0] = indices[i][0].value();
// 	}
}


void	Parser::skipComment() {
	current_pos = std::string::npos;
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

void	Parser::skipWhitespace() {
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
		return TOKEN_INT;
	}
	checkJunkAfterNumber(word, dot_pos + 1);
	return TOKEN_FLOAT;
}

/**
 * Check if there's junk after digits in word.
*/
void	Parser::checkJunkAfterNumber(const std::string& word, size_t pos) const {
	if (word.find_first_not_of(cs_digit, pos) != std::string::npos) {
		throw Parser::parse_error("unexpected character after value");
	}
}

uint8_t	Parser::getFormat() const {
	size_t	first_slash = token.find(cs_slash);
	size_t	last_slash = token.rfind(cs_slash);

	if (first_slash == std::string::npos && last_slash == std::string::npos) {
		return VERTEX;
	} else if (first_slash == last_slash) {
		return VERTEX | TEXTURE;
	} else if (first_slash == last_slash - 1) {
		return VERTEX | NORMAL;
	} else {
		return VERTEX | TEXTURE | NORMAL;
	}
}

} // namespace obj
} // namespace scop

// TODO remove
int main() {
	scop::obj::Parser	parser;

	try {
		typedef scop::obj::TokenType TokenType;

		parser.parseFile("testfile");
	} catch (const std::exception& e) {
		std::cerr << e.what() <<__NL;
	}
}