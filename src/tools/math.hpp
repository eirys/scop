/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   math.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/17 18:03:49 by etran             #+#    #+#             */
/*   Updated: 2023/05/17 18:29:35 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

namespace scop {
namespace math {
	
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
 * @brief Computes the linear interpolation between two values.
 *
 * @param x,y The values to interpolate between.
 * @param t The interpolation factor.
 * 
 * @note T must be a basic type (float, int, etc.).
*/
template <typename T>
inline float	lerp(T x, T y, float t) {
	return std::fma(t, y - x, x);
}

} // namespace math
} // namespace scop