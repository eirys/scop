/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 14:23:35 by eli               #+#    #+#             */
/*   Updated: 2023/05/27 01:24:00 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// Std
# include <cmath>
# include <random>
# include <fstream>
# include <vector>
# include <iostream>
# include <cstring>

# define __NL '\n'

# ifdef __DEBUG
#  define LOG(X) std::cerr << X << __NL
# else
#  define LOG(X)
# endif

# include "vector.hpp"
# include "vertex.hpp"

namespace scop {
namespace utils {

/**
 * @brief If true, the system is big endian.
*/
const bool	big_endian = []() -> bool {
	const int	value = 0x01;
	const void*	address = static_cast<const void *>(&value);
	const unsigned char*	least_significant_address =
		static_cast<const unsigned char*>(address);
	return *least_significant_address != 0x01;
}();

/**
 * Read binary file and return in vector of char format.
*/
inline std::vector<char>	readFile(const std::string& filename) {
	// Read file as binary file, at the end of the file
	std::ifstream	file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file: " + filename);
	}

	std::size_t			file_size = static_cast<std::size_t>(file.tellg());
	std::vector<char>	buffer(file_size);
	file.seekg(0);
	file.read(buffer.data(), file_size);
	file.close();
	return buffer;
}

/**
 * @brief Computes the barycenter of a list of vertices.
 * 
 * @param vertices The list of vertices.
*/
inline Vect3	computeBarycenter(const std::vector<Vertex>& vertices) noexcept {
	Vect3	barycenter{};

	for (const Vertex& vertex : vertices) {
		barycenter += vertex.pos;
	}
	if (vertices.size() > 0) {
		barycenter /= static_cast<float>(vertices.size());
	}
	return barycenter;
}

} // namespace utils
} // namespace scop