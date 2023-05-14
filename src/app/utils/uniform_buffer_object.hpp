/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uniform_buffer_object.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/29 20:12:41 by eli               #+#    #+#             */
/*   Updated: 2023/05/14 10:55:29 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UNIFORM_BUFFER_OBJECT_HPP
# define UNIFORM_BUFFER_OBJECT_HPP

# define __ALIGNMENT 16

# include "matrix.hpp"

namespace scop {

struct alignas(__ALIGNMENT) UniformBufferObject {
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
	static constexpr const size_t	texture_size = __ALIGNMENT; // due to alignas(16) in struct

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	UniformBufferObject() = default;
	UniformBufferObject& operator=(const UniformBufferObject& other) = default;
	UniformBufferObject(UniformBufferObject&& other) = default;
	UniformBufferObject(const UniformBufferObject& other) = default;
	~UniformBufferObject() = default;

};

} // namespace scop

#endif