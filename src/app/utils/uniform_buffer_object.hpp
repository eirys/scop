/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uniform_buffer_object.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/29 20:12:41 by eli               #+#    #+#             */
/*   Updated: 2023/05/20 19:43:35 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# define __ALIGNMENT 64

# include "matrix.hpp"

namespace scop {

class UniformBufferObject {
public:
	/* ========================================================================= */
	/*                               HELPER OBJECTS                              */
	/* ========================================================================= */

	struct Camera {
		scop::Mat4	model;
		scop::Mat4	view;
		scop::Mat4	proj;
		scop::Mat4	zoom;

		static constexpr size_t size() noexcept {
			return sizeof(Camera);
		}
	};

	struct Texture {
		bool	enabled;
		float	mix;

		static constexpr size_t size() noexcept {
			return sizeof(Texture);
		}
	};

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	UniformBufferObject() = default;
	UniformBufferObject(const UniformBufferObject& other) = default;
	UniformBufferObject(UniformBufferObject&& other) = default;
	UniformBufferObject& operator=(const UniformBufferObject& other) = default;
	~UniformBufferObject() = default;

	static constexpr size_t size() noexcept {
		return sizeof(UniformBufferObject);
	}
	
	static constexpr size_t offset_camera() noexcept {
		return 0;
	}

	static size_t offset_texture() noexcept {
		static UniformBufferObject* ptr = nullptr;
		return reinterpret_cast<size_t>(&ptr->texture);
	}

	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	/* VERTEX SHADER =========================================================== */

	alignas(__ALIGNMENT) Camera		camera;

	/* FRAGMENT SHADER ========================================================= */

	alignas(__ALIGNMENT) Texture	texture;

};

} // namespace scop