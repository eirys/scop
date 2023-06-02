/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vertex_input.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 20:43:53 by etran             #+#    #+#             */
/*   Updated: 2023/06/02 17:06:48 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// Graphics
# ifndef GLFW_INCLUDE_VULKAN
#  define GLFW_INCLUDE_VULKAN
# endif

# include <GLFW/glfw3.h>

// Std
# include <vector>
# include "vertex.hpp"
# include "vector.hpp"

namespace scop {
namespace graphics {
class Engine;
class Device;

class VertexInput {
public:
	friend Engine;

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	VertexInput() = default;
	VertexInput(VertexInput&& x) = default;
	~VertexInput() = default;

	VertexInput(const VertexInput& x) = delete;
	VertexInput&	operator=(VertexInput&& x) = delete;

	/* ========================================================================= */

	void	init(
		Device& device,
		VkCommandPool command_pool,
		const std::vector<Vertex>& vertices,
		const std::vector<uint32_t>& indices
	);
	void	destroy(Device& device);

private:
	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	VkBuffer						vertex_buffer;
	VkDeviceMemory					vertex_buffer_memory;
	VkBuffer						index_buffer;
	VkDeviceMemory					index_buffer_memory;

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	void							createVertexBuffer(
		Device& device,
	VkCommandPool command_pool,
		const std::vector<Vertex>& vertices
	);
	void							createIndexBuffer(
		Device& device,
	VkCommandPool command_pool,
		const std::vector<uint32_t>& indices
	);

}; // class VertexInput

} // namespace graphics
} // namespace scop
