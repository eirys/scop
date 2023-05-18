/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vertex_input.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/16 13:03:48 by etran             #+#    #+#             */
/*   Updated: 2023/05/16 17:35:45 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vertex_input.hpp"
#include "graphics_pipeline.hpp"
#include "device.hpp"

#include <cstring> // memcpy

namespace scop {
namespace graphics {

/* ========================================================================== */
/*                                   PUBLIC                                   */
/* ========================================================================== */

void	VertexInput::init(
	Device& device,
	VkCommandPool command_pool,
	const std::vector<Vertex>& vertices,
	const std::vector<uint32_t>& indices
) {
	createVertexBuffer(device, command_pool, vertices);
	createIndexBuffer(device, command_pool, indices);
}

void	VertexInput::destroy(Device& device) {
	vkDestroyBuffer(device.logical_device, index_buffer, nullptr);
	vkFreeMemory(device.logical_device, index_buffer_memory, nullptr);
	vkDestroyBuffer(device.logical_device, vertex_buffer, nullptr);
	vkFreeMemory(device.logical_device, vertex_buffer_memory, nullptr);
}

/* ========================================================================== */
/*                                   PRIVATE                                  */
/* ========================================================================== */

/**
 * Create the vertex buffer that'll be used to store the vertices of the triangle.
*/
void	VertexInput::createVertexBuffer(
	Device& device,
	VkCommandPool command_pool,
	const std::vector<Vertex>& vertices
) {
	VkDeviceSize	buffer_size = sizeof(Vertex) * vertices.size();

	// Create staging buffer to upload cpu memory to
	VkBuffer		staging_buffer;
	VkDeviceMemory	staging_buffer_memory;

	// Cpu accessible memory
	device.createBuffer(
		buffer_size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		staging_buffer,
		staging_buffer_memory
	);

	// Fill staging buffer
	void*	data;
	vkMapMemory(device.logical_device, staging_buffer_memory, 0, buffer_size, 0, &data);
	memcpy(data, vertices.data(), static_cast<size_t>(buffer_size));
	vkUnmapMemory(device.logical_device, staging_buffer_memory);

	// Create vertex buffer that'll interact with gpu
	device.createBuffer(
		buffer_size,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		vertex_buffer,
		vertex_buffer_memory
	);

	// Now transfer data from staging buffer to vertex buffer
	copyBuffer(
		device.logical_device,
		device.graphics_queue,
		command_pool,
		staging_buffer,
		vertex_buffer,
		buffer_size
	);

	// Cleanup staging buffer
	vkDestroyBuffer(device.logical_device, staging_buffer, nullptr);
	vkFreeMemory(device.logical_device, staging_buffer_memory, nullptr);
}

/**
 *  Create index buffer (pointers into the vertex buffer)
 */
void	VertexInput::createIndexBuffer(
	Device& device,
	VkCommandPool command_pool,
	const std::vector<uint32_t>& indices
) {
	VkDeviceSize	buffer_size = sizeof(indices[0]) * indices.size();
	VkBuffer		staging_buffer;
	VkDeviceMemory	staging_buffer_memory;

	device.createBuffer(
		buffer_size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		staging_buffer,
		staging_buffer_memory
	);

	// Fill staging buffer with indices
	void*	data;
	vkMapMemory(device.logical_device, staging_buffer_memory, 0, buffer_size, 0, &data);
	memcpy(data, indices.data(), static_cast<size_t>(buffer_size));
	vkUnmapMemory(device.logical_device, staging_buffer_memory);

	device.createBuffer(
		buffer_size,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		index_buffer,
		index_buffer_memory
	);

	// Transfer data from staging buffer to index buffer
	copyBuffer(
		device.logical_device,
		device.graphics_queue,
		command_pool,
		staging_buffer,
		index_buffer,
		buffer_size
	);

	// Flush temporary buffers
	vkDestroyBuffer(device.logical_device, staging_buffer, nullptr);
	vkFreeMemory(device.logical_device, staging_buffer_memory, nullptr);
}

}  // namespace graphics
}  // namespace scop