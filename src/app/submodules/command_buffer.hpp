/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_buffer.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/16 12:44:56 by etran             #+#    #+#             */
/*   Updated: 2023/06/02 17:06:48 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// Graphics
# ifndef GLFW_INCLUDE_VULKAN
#  define GLFW_INCLUDE_VULKAN
# endif

# include <GLFW/glfw3.h>

namespace scop {
namespace graphics {

class Engine;
class Device;

class CommandBuffer {
public:

	friend Engine;

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	CommandBuffer() = default;
	CommandBuffer(CommandBuffer&& other) = default;
	~CommandBuffer() = default;

	CommandBuffer(const CommandBuffer& other) = delete;
	CommandBuffer& operator=(const CommandBuffer& other) = delete;

	/* ========================================================================= */

	void							initPool(Device& device);
	void							initBuffer(Device& device);
	void							destroy(Device& device);

private:
	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	VkCommandPool					vk_command_pool;
	VkCommandBuffer					command_buffers;
	
	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	void							createCommandPool(Device& device);
	void							createCommandBuffers(Device& device);

}; // class CommandBuffer

} // namespace graphics
} // namespace scop

