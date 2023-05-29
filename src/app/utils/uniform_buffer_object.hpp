/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uniform_buffer_object.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/29 20:12:41 by eli               #+#    #+#             */
/*   Updated: 2023/05/29 10:53:34 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# define __ALIGNMENT_MAT4 16
# define __ALIGNMENT_VEC3 16
# define __ALIGNMENT_SCAL 4
# define __ALIGNMENT_BUFF 64

# include "matrix.hpp"

namespace scop {

struct UniformBufferObject {
public:
	/* ========================================================================= */
	/*                               HELPER OBJECTS                              */
	/* ========================================================================= */

	struct Camera {
		alignas(__ALIGNMENT_MAT4) scop::Mat4	model;
		alignas(__ALIGNMENT_MAT4) scop::Mat4	view;
		alignas(__ALIGNMENT_MAT4) scop::Mat4	proj;
	};

	struct Texture {
		alignas(__ALIGNMENT_SCAL) int32_t		state;
		alignas(__ALIGNMENT_SCAL) float			mix;
	};

	struct Light {
		alignas(__ALIGNMENT_VEC3) scop::Vect3	ambient_color;
		alignas(__ALIGNMENT_VEC3) scop::Vect3	light_pos;
		alignas(__ALIGNMENT_VEC3) scop::Vect3	light_color;
		alignas(__ALIGNMENT_VEC3) scop::Vect3	diffuse_color;
		alignas(__ALIGNMENT_VEC3) scop::Vect3	eye_position;
		alignas(__ALIGNMENT_VEC3) scop::Vect3	specular_color;
		alignas(__ALIGNMENT_SCAL) int32_t		shininess;
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

	alignas(__ALIGNMENT_BUFF) Camera		camera;
	alignas(__ALIGNMENT_BUFF) Texture		texture;
	alignas(__ALIGNMENT_BUFF) Light			light;

};

} // namespace scop