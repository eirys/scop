/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uniform_buffer_object.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/29 20:12:41 by eli               #+#    #+#             */
/*   Updated: 2023/05/28 23:41:12 by etran            ###   ########.fr       */
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
	};

	struct Texture {
		int			state;
		float		mix;
	};

	struct Light {
		scop::Vect3	ambient_color;
		scop::Vect3	light_pos;
		scop::Vect3 light_color;
		scop::Vect3	diffuse_color;
		scop::Vect3	eye_position;
		scop::Vect3	specular_color;
		uint16_t	shininess;
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
	alignas(__ALIGNMENT) Light		light;

};

} // namespace scop