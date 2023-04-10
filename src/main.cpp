/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 03:53:55 by eli               #+#    #+#             */
/*   Updated: 2023/04/10 18:23:12 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app.hpp"

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