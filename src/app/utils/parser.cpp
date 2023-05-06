/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 15:06:05 by etran             #+#    #+#             */
/*   Updated: 2023/05/06 17:08:11 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.hpp"

/* ========================================================================== */
/*                                   PUBLIC                                   */
/* ========================================================================== */

Model	Parser::parseFile(const std::string& file_name) {
	Model	model_object;
	size_t	nb_lines = 0;
	
	file.open(file_name);
	if (!file.is_open()) {
		throw std::invalid_argument("Could not open file " + file_name);
	}

	while (file.good()) {
		++nb_lines;
		processLine();
	}

	return model_object;
}

/* ========================================================================== */
/*                                   PRIVATE                                  */
/* ========================================================================== */

void	Parser::processLine() {
	std::getline(file, current_line);
}

void	Parser::getNextToken() {
	
}