/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 03:53:55 by eli               #+#    #+#             */
/*   Updated: 2023/05/11 15:39:27 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app.hpp"

int main(int ac, char** av) {
	try {
		std::string	texture_path;

		if (ac == 1) {
			throw std::invalid_argument("No model path provided");
		} else if (ac == 3) {
			texture_path = av[2];
		} else if (ac > 3) {
			throw std::invalid_argument("Too many arguments");
		} else { 
			//TODO: remove
			throw std::invalid_argument("No texture path provided");
		}

	// TODO: process file names

		scop::App		app(av[1], texture_path);
		app.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << __NL;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}