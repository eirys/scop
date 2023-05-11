/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vertex.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/26 15:25:04 by etran             #+#    #+#             */
/*   Updated: 2023/05/11 21:29:15 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VERTEX_HPP
# define VERTEX_HPP

// Graphics
# include <vulkan/vulkan.h>

// Std
# include <array>

# include "vector.hpp"

namespace scop {

struct Vertex {
	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	scop::Vect3		pos;
	scop::Vect3		color;
	scop::Vect2		tex_coord;
	// scop::Vect3		normal; TODO

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	/**
	 * Expliciting to vulkan how to pass data to vertex shader.
	*/
	static VkVertexInputBindingDescription	getBindingDescription() {
		VkVertexInputBindingDescription	binding_description{};

		binding_description.binding = 0;
		binding_description.stride = sizeof(Vertex);
		binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return binding_description;
	}

	/**
	 * Expliciting to vulkan the vertex struct format.
	*/
	static std::array<VkVertexInputAttributeDescription, 3>	getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 3>	attribute_descriptions{};

		// `pos` attribute
		attribute_descriptions[0].binding = 0;
		attribute_descriptions[0].location = 0;
		attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[0].offset = offsetof(Vertex, pos);

		// `color` attribute
		attribute_descriptions[1].binding = 0;
		attribute_descriptions[1].location = 1;
		attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[1].offset = offsetof(Vertex, color);

		// `tex_coord` attribute
		attribute_descriptions[2].binding = 0;
		attribute_descriptions[2].location = 2;
		attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attribute_descriptions[2].offset = offsetof(Vertex, tex_coord);

		return attribute_descriptions;
	}

	bool	operator==(const Vertex& rhs) const {
		return (
			pos == rhs.pos &&
			tex_coord == rhs.tex_coord
		);
	}
}; // struct Vertex

} // namespace scop

namespace std {

	template<>
	struct hash<scop::Vertex> {
		size_t	operator()(const scop::Vertex& vertex) const {
			return (
				(hash<scop::Vect3>()(vertex.pos) ^
				(hash<scop::Vect3>()(vertex.color) << 1)) >> 1 ^
				(hash<scop::Vect2>()(vertex.tex_coord) << 1)
			);
		}
	};

} // namespace std

#endif