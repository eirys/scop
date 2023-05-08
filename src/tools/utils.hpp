/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 14:23:35 by eli               #+#    #+#             */
/*   Updated: 2023/05/08 10:48:55 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

// Std
# include <cmath>

# define __NL '\n'

# ifdef __DEBUG
#  define LOG(X) std::cerr << X << __NL
# else
#  define LOG(X)
# endif

namespace scop {
namespace utils {

inline float	radians(float degrees) {
	return degrees * M_PI / 180;
}

inline bool		isCharset(char c, const std::string charset) {
	if (charset.find(c) != std::string::npos)
		return true;
	return false;
}

} // namespace utils
} // namespace scop

#endif