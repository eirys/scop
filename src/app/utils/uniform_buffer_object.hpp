/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uniform_buffer_object.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/29 20:12:41 by eli               #+#    #+#             */
/*   Updated: 2023/05/14 22:08:04 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UNIFORM_BUFFER_OBJECT_HPP
# define UNIFORM_BUFFER_OBJECT_HPP

# define __ALIGNMENT 64

# include "matrix.hpp"

namespace scop {

class UniformBufferObject {
public:
	/* ========================================================================= */
	/*                               HELPER OBJECTS                              */
	/* ========================================================================= */

	class Camera {
		scop::Mat4	model;
		scop::Mat4	view;
		scop::Mat4	proj;
	};

	class Texture {
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

	// TODO
	// void	updateCamera(/* options */);
	// void	updateTexture();

// private:
	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	/* VERTEX SHADER =========================================================== */

	scop::Mat4	rotation;
	scop::Mat4	view;
	scop::Mat4	proj;
	scop::Vect3	translation;	// TODO: remove

	/* FRAGMENT SHADER ========================================================= */

	alignas(__ALIGNMENT) bool	texture_enabled;
	float	texture_mix;

};

} // namespace scop

#endif