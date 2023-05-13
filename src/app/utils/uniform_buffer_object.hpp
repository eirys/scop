/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uniform_buffer_object.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/29 20:12:41 by eli               #+#    #+#             */
/*   Updated: 2023/05/13 20:53:24 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UNIFORM_BUFFER_OBJECT_HPP
# define UNIFORM_BUFFER_OBJECT_HPP

# include "matrix.hpp"

namespace scop {
struct alignas(16) UniformBufferObject {
	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	/* VERTEX SHADER =========================================================== */

	scop::Mat4	model;
	scop::Mat4	view;
	scop::Mat4	proj;

	/* FRAGMENT SHADER ========================================================= */

	bool	texture_enabled;
	float	texture_mix;

	/* ========================================================================= */
	/*                               STATIC MEMBERS                              */
	/* ========================================================================= */

	static constexpr size_t	camera = 3 * sizeof(scop::Mat4);
	static constexpr size_t	texture = 16; // due to alignas(16) in struct
};
} // namespace scop

#endif