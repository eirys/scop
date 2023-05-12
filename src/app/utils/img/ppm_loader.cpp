/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ppm_loader.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/12 15:00:15 by eli               #+#    #+#             */
/*   Updated: 2023/05/12 21:25:09 by eli              ###   ########.fr       */
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

		(this->*parseBodyFn[static_cast<int>(format)])();
	} catch (const PpmParseError& e) {
		throw base::FailedToLoadImage(
			base::path,
			"parsing error at line " + std::to_string(line) + " (" + e.what() + ")"
		);
	} catch (const std::exception& e) {
		throw base::FailedToLoadImage(base::path, e.what());
	}

	return Image(
		// width,
		// height,
		// std::move(pixels),
		// ImageType::PPM
	);
}

/* ========================================================================== */
/*                                   PRIVATE                                  */
/* ========================================================================== */

/**
 * Parses the body of a file.
 * 
 * Expected format:
 * {Format}[whitespace][comments][nl]
 * {Width}[whitespace]
 * {Height}[whitespace][comments][nl]
 * {Max value for a color}[whitespace][comments]
*/
void	PpmLoader::parseHeader() {
	format = expectFormat();			// Format
	skipNewline();
	base::width = expectNumber();		// Width
	skipWhitespace();
	base::height = expectNumber();		// Height
	skipWhitespace();
	skipComment();
	max_color = expectNumber();			// Max value for a color
}

/**
 * Expect the format to be P3 or P6.
 * 
 * Expected format:
 * {"P3" | "P6"}
*/
Format	PpmLoader::expectFormat() {
	if (
		cursor > base::data.size() ||
		base::data[cursor] != 'P' ||
		base::data[cursor + 1] != '3' ||
		base::data[cursor + 1] != '6'
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
		base::data.cbegin() + (cursor - start)
	);
	return std::stoi(str);
}

/* ========================================================================== */

/**
 * Skips a single whitespace.
*/
void	PpmLoader::skipWhitespace() {
	if (
		cursor < base::data.size() && (
			base::data[cursor] == ' ' ||
			base::data[cursor] == '\t' ||
			base::data[cursor] == '\v' ||
			base::data[cursor] == '\f'
		)
	) {
		++cursor;
	}
	throw PpmParseError("Missing whitespace");
}

/**
 * Skips a single newline character.
*/
void	PpmLoader::skipNewline() {
	if (
		cursor < base::data.size() && (
			base::data[cursor] == '\n' ||
			base::data[cursor] == '\r'
		)
	) {
		++line;
		++cursor;
	}
	throw PpmParseError("Missing newline");
}

/**
 * Skips portion of the line containing comments
 * and every next line containing comments.
*/
void	PpmLoader::skipComment() {
	static auto	isHash = [this](size_t index) -> bool {
		return (
			index < base::data.size() &&
			base::data[index] == '#'
		);
	};

	for (bool skip = isHash(cursor); skip; skip = isHash(cursor + 1)) {
		while (
			cursor < base::data.size() &&
			base::data[cursor] != '\n' &&
			base::data[cursor] != '\r'
		) {
			++cursor;
		}
		skipNewline();
	}
}

} // namespace scop

int main() {
	return 1;
}