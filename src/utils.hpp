/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 14:23:35 by eli               #+#    #+#             */
/*   Updated: 2023/05/03 16:17:56 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

// Std
# include <cmath>

# define __NL '\n'

# ifdef __VERBOSE
#  define LOG(X) std::cerr << X << __NL
# endif

namespace scop {

namespace utils {

inline float	radians(float degrees) {
	return degrees * M_PI / 180;
}

} // namespace utils

} // namespace scop

#endif