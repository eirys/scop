/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 14:23:35 by eli               #+#    #+#             */
/*   Updated: 2023/04/27 13:18:59 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# define NL	'\n'

#ifdef __VERBOSE
# define LOG(X) std::cerr << X << NL
#endif

namespace utils {

} // namespace utils

#endif