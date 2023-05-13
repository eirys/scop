/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 14:23:35 by eli               #+#    #+#             */
/*   Updated: 2023/05/13 10:31:08 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

// Std
# include <cmath>
# include <random>
# include <fstream>
# include <vector>
# include <iostream>

# define __NL '\n'

# ifdef __DEBUG
#  define LOG(X) std::cerr << X << __NL
# else
#  define LOG(X)
# endif

namespace scop {
namespace utils {

/**
 * @brief If true, the system is big endian.
*/
const bool	big_endian = []() -> bool {
	const int	value = 0x01;
	const void*	address = static_cast<const void *>(&value);
	const unsigned char*	least_significant_address = static_cast<const unsigned char*>(address);
	return *least_significant_address != 0x01;
}();

/**
 * Converts degrees to radians.
*/
inline float	radians(float degrees) noexcept {
	return degrees * M_PI / 180;
}

/**
 * @brief Generates a random float between 0.0f and 1.0f.
 * @note Uses a Mersenne Twister pseudo-random generator.
*/
inline float	generateRandomFloat() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<float> dis(0.0f, 1.0f);
	return dis(gen);
}

/**
 * Generates a random vibrant color.
*/
inline void	generateVibrantColor(float& red, float& green, float& blue) {
	red = generateRandomFloat();
	green = generateRandomFloat();
	blue = generateRandomFloat();

	// Boost the saturation and value to make it vibrant
	const float maxChannel = std::max(std::max(red, green), blue);
	const float minChannel = std::min(std::min(red, green), blue);
	const float delta = maxChannel - minChannel;
	
	if (delta > 0.0f) {
		if (maxChannel == red) {
			green = (green - minChannel) / delta;
			blue = (blue - minChannel) / delta;
		}
		else if (maxChannel == green) {
			red = (red - minChannel) / delta;
			blue = (blue - minChannel) / delta;
		}
		else if (maxChannel == blue) {
			red = (red - minChannel) / delta;
			green = (green - minChannel) / delta;
		}
	}
}

/**
 * Read binary file and return in vector of char format.
*/
inline std::vector<char>	readFile(const std::string& filename) {
	// Read file as binary file, at the end of the file
	std::ifstream	file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file: " + filename);
	}

	size_t				file_size = static_cast<size_t>(file.tellg());
	std::vector<char>	buffer(file_size);
	file.seekg(0);
	file.read(buffer.data(), file_size);
	file.close();
	return buffer;
}

} // namespace utils
} // namespace scop

#endif