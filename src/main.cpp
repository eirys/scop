/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 03:53:55 by eli               #+#    #+#             */
/*   Updated: 2023/04/26 15:26:56 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app.hpp"
#include "vertex.hpp"

int main() {
	App		app;

	#ifdef NDEBUG
	std::cout << "In debug mode" << NL;
	#endif

	try {
		app.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << NL;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}