/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/26 23:38:54 by etran             #+#    #+#             */
/*   Updated: 2023/05/27 01:18:04 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.hpp"

namespace scop {

/* ========================================================================== */
/*                                  PROTECTED                                 */
/* ========================================================================== */

/**
 * @brief Retrieve a word. A word is a sequence of characters that are not
 * whitespace characters.
*/
bool	Parser::getWord() {
	if (current_pos == std::string::npos) {
		return false;
	}
	std::size_t	end_pos = line.find_first_of(cs_whitespaces, current_pos);
	token = line.substr(current_pos, end_pos - current_pos);
	current_pos = end_pos;
	return true;
}

/**
 * @brief Skip comment. Moves current_pos to the end of the line.
*/
void	Parser::skipComment() noexcept {
	current_pos = std::string::npos;
}

/**
 * @brief Skip whitespace characters. Moves current_pos to the next
 * non-whitespace character.
*/
void	Parser::skipWhitespace() noexcept {
	current_pos = line.find_first_not_of(cs_whitespaces, current_pos);
}

/**
 * @brief Check if the value is a number. If so, return its type.
 * 
 * @param word The value to check.
*/
TokenType	Parser::checkNumberType(const std::string& word) const {
	if (word.empty()) {
		throw Parser::parse_error("expecting number");
	}

	std::size_t	pos_checked = word.find(cs_negate);

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
	std::size_t	dot_pos = word.find(cs_dot, pos_checked);
	if (dot_pos == std::string::npos) {
		checkJunkAfterNumber(word, pos_checked);
		return TokenType::TOKEN_INT;
	}
	checkJunkAfterNumber(word, dot_pos + 1);
	return TokenType::TOKEN_FLOAT;
}

/**
 * @brief Check if there are junk characters after a number.
*/
void	Parser::checkJunkAfterNumber(
	const std::string& word,
	std::size_t pos
) const {
	if (word.find_first_not_of(cs_digit, pos) != std::string::npos) {
		throw Parser::parse_error("unexpected character after value");
	}
}

} // namespace scop