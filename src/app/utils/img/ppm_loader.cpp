/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ppm_loader.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/12 15:00:15 by eli               #+#    #+#             */
/*   Updated: 2023/05/26 13:00:43 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ppm_loader.hpp"
#include "utils.hpp"

namespace scop {

/* ========================================================================== */
/*                                   PUBLIC                                   */
/* ========================================================================== */

PpmLoader::PpmLoader(const std::string& _path):
	base(_path, ImageType::PPM) {
		base::data = utils::readFile(_path);
}

/* ========================================================================== */

Image	PpmLoader::load() {
	try {
		parseHeader();
		Pixels	pixels = parseBody();

		return Image(
			base::path,
			// base::type,
			std::move(pixels),
			width,
			height
		);
	} catch (const PpmParseError& e) {
		throw base::FailedToLoadImage(
			base::path,
			std::string(e.what()) + " (line " + std::to_string(line) + ")"
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
	if (max_color > std::numeric_limits<uint8_t>::max()) {
		throw PpmParseError("max color value is too high, expecting 8 bits (255)");
	}
}

/**
 * Parses the image body.
*/
PpmLoader::Pixels	PpmLoader::parseBody() {
	// Reads a character.
	ParseNumberFn	readExcept = [this]() -> uint8_t {
		if (cursor >= base::data.size()) {
			throw PpmParseError("unexpected end of file");
		}
		return static_cast<uint8_t>(base::data[cursor++]);
	};

	// Reads a number.
	ParseNumberFn	readNb = [this]() -> uint32_t {
		while (skipWhitespace()) {
			;
		}
		uint32_t	nb = 0;
		uint8_t		c;

		while (cursor < base::data.size()) {
			c = static_cast<uint8_t>(base::data[cursor++]);
			if (c >= '0' && c <= '9') {
				nb = nb * 10 + (c - '0');
			} else if (std::isspace(c)) {
				return nb;
			} else {
				throw PpmParseError("expecting number");
			}
		}
		throw PpmParseError("unexpected end of file");
	};

	ParseNumberFn	parseChannelFn(format == Format::P6 ? readExcept : readNb);
	PpmLoader::Pixels	pixels(base::width * base::height * sizeof(uint32_t));
	size_t	row = 0;

	while (row < base::height) {
		uint8_t	r, g, b;

		for (size_t i = 0; i < base::width; ++i) {
			// Read 3 bytes (RGB)
			r = parseChannelFn();
			g = parseChannelFn();
			b = parseChannelFn();

			pixels[row * base::width + i] = createPixel(r, g, b);
		}
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
	return std::stoul(str);
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

/**
 * Builds a pixel from RGB values.
 * 
 * @note	- Alpha is set to 255.
 * @note	- RGB values are reversed (ABGR).
*/
uint32_t	PpmLoader::createPixel(uint8_t r, uint8_t g, uint8_t b) const noexcept {
	return 0xff000000 | (b << 16) | (g << 8) | r;
}

} // namespace scop