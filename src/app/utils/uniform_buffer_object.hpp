/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uniform_buffer_object.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/29 20:12:41 by eli               #+#    #+#             */
/*   Updated: 2023/05/12 23:31:31 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UNIFORM_BUFFER_OBJECT_HPP
# define UNIFORM_BUFFER_OBJECT_HPP

# include "matrix.hpp"

namespace scop {
struct UniformBufferObject {
	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	/* VERTEX SHADER =========================================================== */
	alignas(16) scop::Mat4	model;
	alignas(16) scop::Mat4	view;
	alignas(16) scop::Mat4	proj;

	/* FRAGMENT SHADER ========================================================= */
	bool	texture_enabled;
	float	texture_mix;

	/* ========================================================================= */
	/*                               STATIC MEMBERS                              */
	/* ========================================================================= */

	static constexpr size_t	vertex_shader_part = 3 * sizeof(scop::Mat4);
	static constexpr size_t	fragment_shader_part = sizeof(bool) + sizeof(float);
};
} // namespace scop

#endif