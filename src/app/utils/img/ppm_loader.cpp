/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ppm_loader.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/12 15:00:15 by eli               #+#    #+#             */
/*   Updated: 2023/05/12 23:29:33 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ppm_loader.hpp"
#include "utils.hpp"

namespace scop {

/* ========================================================================== */
/*                                   PUBLIC                                   */
/* ========================================================================== */

PpmLoader::PpmLoader(const char* _path):
	base(_path, ImageType::PPM) {
		base::data = utils::readFile(_path);
}

/* ========================================================================== */

Image	PpmLoader::load() {
	try {
		parseHeader();

		ParseBodyFn	parseBodyFn[2] = {
			&PpmLoader::parseBodyP3,
			&PpmLoader::parseBodyP6
		};

		Pixels	pixels = (this->*parseBodyFn[static_cast<int>(format)])();
		return Image(
			base::path,
			base::type,
			std::move(pixels),
			width,
			height
		);
	} catch (const PpmParseError& e) {
		throw base::FailedToLoadImage(
			base::path,
			std::string(e.what()) + "(line " + std::to_string(line) + ")"
		);
	} catch (const std::exception& e) {
		throw std::runtime_error(
			"unexpected error at line " + std::to_string(line) + ": " + e.what()
		);
	}
}

/* ========================================================================== */
/*                                   PRIVATE                                  */
/* ========================================================================== */

/**
 * Parses the body of a file.
 * 
 * Expected format:
 * [comment]
 * {Format}[ws/comment]
 * {Width}[ws/comment]
 * {Height}[ws/comment]
 * {Max_color}[ws]
*/
void	PpmLoader::parseHeader() {
	skipComment();
	format = expectFormat();
	ignoreChunk();
	base::width = expectNumber();
	ignoreChunk();
	base::height = expectNumber();
	ignoreChunk();
	max_color = expectNumber();
	if (!skipWhitespace()) {
		throw PpmParseError("expecting whitespace");
	}
}

PpmLoader::Pixels	PpmLoader::parseBodyP3() {
	// TODO
	return PpmLoader::Pixels();
}

/**
 * Parses the image body of a P6 file.
*/
PpmLoader::Pixels	PpmLoader::parseBodyP6() {
	auto	readExcept = [this]() -> uint32_t {
		if (cursor > base::data.size()) {
			throw PpmParseError("unexpected end of file");
		}
		return static_cast<uint32_t>(base::data[cursor++]);
	};

	PpmLoader::Pixels	pixels;
	size_t	row = 0;
	// For each line, read 3 * width bytes
	while (
		cursor < base::data.size() &&
		base::data[cursor] != '\n' &&
		base::data[cursor] != '\r'
	) {
		uint32_t	r, g, b;

		for (size_t i = 0; i < base::width; ++i) {
			r = readExcept();
			g = readExcept();
			b = readExcept();
		}
		pixels.emplace_back(r);
		pixels.emplace_back(g);
		pixels.emplace_back(b);
		++row;
	}
	if (row != base::height) {
		throw PpmParseError("invalid number of rows");
	}
	return pixels;
}

/* ========================================================================== */

/**
 * Expect P3 or P6.
 * 
 * Expected format:
 * {"P3" | "P6"}
*/
PpmLoader::Format	PpmLoader::expectFormat() {
	if (
		cursor > base::data.size() ||
		base::data[cursor] != 'P' || (
			base::data[cursor + 1] != '3' &&
			base::data[cursor + 1] != '6'
		)
	) {
		throw PpmParseError("Invalid PPM format");
	}

	Format	file_format = (base::data[cursor + 1] == '3') ? Format::P3 : Format::P6;
	cursor += 2;
	return file_format;
}

/**
 * Returns number.
*/
uint32_t	PpmLoader::expectNumber() {
	if (cursor >= base::data.size()) {
		throw PpmParseError("missing value");
	}

	size_t	start = cursor;
	while (
		base::data[cursor] >= '0' &&
		base::data[cursor] <= '9'
	) {
		++cursor;
	}
	if (start == cursor) {
		throw PpmParseError("expecting number");
	}
	std::string	str(
		base::data.cbegin() + start,
		base::data.cbegin() + cursor + 1
	);
	return std::stoi(str);
}

/* ========================================================================== */

/**
 * Skips single whitespace.
*/
bool	PpmLoader::skipWhitespace() noexcept {
	if (cursor < base::data.size()) {
		if (
			base::data[cursor] == '\n' ||
			base::data[cursor] == '\r'
		) {
			++line;
		} else if (
			base::data[cursor] != ' ' &&
			base::data[cursor] != '\t' &&
			base::data[cursor] != '\v' &&
			base::data[cursor] != '\f'
		) {
			return false;
		}
		++cursor;
		return true;
	}
	return false;
}

/**
 * Skips portion containing comments.
*/
bool	PpmLoader::skipComment() noexcept {
	if (
		cursor < base::data.size() &&
		base::data[cursor] == '#'
	) {
		while (cursor < base::data.size()) {
			if (
				base::data[cursor] == '\n' ||
				base::data[cursor] == '\r'
			) {
				++line;
				++cursor;
				return true;
			}
			++cursor;
		}
	}
	return false;
}

/**
 * Skips portion containing comments and whitespace.
*/
void	PpmLoader::ignoreChunk() noexcept {
	while (skipComment() || skipWhitespace()) { ; }
}

} // namespace scop

#include <iostream>
int main() {
	try {

	scop::PpmLoader	loader("/home/eli/random/hammy.ppm");
	loader.load();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}