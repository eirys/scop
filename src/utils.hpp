/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 14:23:35 by eli               #+#    #+#             */
/*   Updated: 2023/04/30 22:12:31 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# define __NL '\n'

# ifdef __VERBOSE
#  define LOG(X) std::cerr << X << __NL
# endif

namespace utils {

} // namespace utils

#endif