/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 14:23:35 by eli               #+#    #+#             */
/*   Updated: 2023/04/10 18:19:48 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# define NL	'\n'

#ifdef NDEBUG
# define LOG(X) std::cerr << X << NL
#endif

namespace utils {

} // namespace utils

#endif