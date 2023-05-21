/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uniform_buffer_object.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/29 20:12:41 by eli               #+#    #+#             */
/*   Updated: 2023/05/21 11:09:01 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# define __ALIGNMENT 64

# include "matrix.hpp"

namespace scop {

struct UniformBufferObject {
public:
	/* ========================================================================= */
	/*                               HELPER OBJECTS                              */
	/* ========================================================================= */

	struct Camera {
		scop::Mat4	model;
		scop::Mat4	view;
		scop::Mat4	proj;
		scop::Mat4	zoom;
	};

	struct Texture {
		bool	enabled;
		float	mix;
	};

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	UniformBufferObject() = default;
	UniformBufferObject(const UniformBufferObject& other) = default;
	UniformBufferObject(UniformBufferObject&& other) = default;
	UniformBufferObject& operator=(const UniformBufferObject& other) = default;
	~UniformBufferObject() = default;

	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	alignas(__ALIGNMENT) Camera		camera;
	alignas(__ALIGNMENT) Texture	texture;

};

} // namespace scop