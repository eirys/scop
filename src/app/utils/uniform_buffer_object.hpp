/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uniform_buffer_object.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/29 20:12:41 by eli               #+#    #+#             */
/*   Updated: 2023/05/13 21:03:23 by etran            ###   ########.fr       */
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

	static constexpr const size_t	camera_size = 3 * sizeof(scop::Mat4);
	static constexpr const size_t	texture_size = 16; // due to alignas(16) in struct
};
} // namespace scop

#endif