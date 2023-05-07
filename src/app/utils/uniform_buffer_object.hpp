/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uniform_buffer_object.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/29 20:12:41 by eli               #+#    #+#             */
/*   Updated: 2023/05/07 08:17:56 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UNIFORM_BUFFER_OBJECT_HPP
# define UNIFORM_BUFFER_OBJECT_HPP

# include "matrix.hpp"

namespace scop {
struct UniformBufferObject {
	alignas(16) scop::Mat4	model;
	alignas(16) scop::Mat4	view;
	alignas(16) scop::Mat4	proj;
	bool					is_textured;
};
} // namespace scop

#endif