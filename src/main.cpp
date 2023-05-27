/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 03:53:55 by eli               #+#    #+#             */
/*   Updated: 2023/05/28 01:46:59 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app.hpp"

int main(int ac, char** av) {
	try {
		if (ac == 1) {
			throw std::invalid_argument("No model path provided");
		} else if (ac != 2) {
			throw std::invalid_argument("Too many arguments");
		}

		scop::App		app(av[1]);
		app.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << __NL;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}