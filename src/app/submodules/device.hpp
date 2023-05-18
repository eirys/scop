/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   device.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/16 00:55:41 by etran             #+#    #+#             */
/*   Updated: 2023/05/18 14:06:34 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// Graphics
# ifndef GLFW_INCLUDE_VULKAN
#  define GLFW_INCLUDE_VULKAN
# endif
# include <GLFW/glfw3.h>

# include <vector>

namespace scop {
class Window;

namespace graphics {
struct QueueFamilyIndices;
class GraphicsPipeline;
class RenderTarget;
class RenderTargetResources;
class DescriptorSet;
class CommandBuffer;
class TextureSampler;
class VertexInput;

class Device {
public:
	friend GraphicsPipeline;
	friend RenderTarget;
	friend RenderTargetResources;
	friend DescriptorSet;
	friend CommandBuffer;
	friend TextureSampler;
	friend VertexInput;

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	Device() = default;
	Device(Device&& other) = default;
	~Device() = default;

	Device(const Device& other) = delete;
	Device& operator=(const Device& other) = delete;

	/* ========================================================================= */

	void							init(scop::Window& window, VkInstance instance);
	void							destroy(VkInstance instance);
	void							idle();


	uint32_t						findMemoryType(
		uint32_t type_filter,
		VkMemoryPropertyFlags properties
	) const;
	void							createImage(
		uint32_t width,
		uint32_t height,
		uint32_t mip_level,
		VkSampleCountFlagBits num_samples,
		VkFormat format,
		VkImageTiling tiling,
		VkImageUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkImage& image,
		VkDeviceMemory& image_memory
	);
	void							createBuffer(
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkBuffer& buffer,
		VkDeviceMemory& buffer_memory
	);

private:
	/* ========================================================================= */
	/*                               CONST MEMBERS                               */
	/* ========================================================================= */

	const std::vector<const char*>	device_extensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	VkSurfaceKHR					vk_surface;

	VkPhysicalDevice				physical_device = VK_NULL_HANDLE;
	VkDevice						logical_device;

	VkQueue							graphics_queue;
	VkQueue							present_queue;

	VkSampleCountFlagBits			msaa_samples;


	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	void							createSurface(VkInstance instance, scop::Window& window);
	void							pickPhysicalDevice(VkInstance vk_instance);
	void							createLogicalDevice();

	/* ========================================================================= */

	VkSampleCountFlagBits			getMaxUsableSampleCount() const;
	bool							checkDeviceExtensionSupport(
		VkPhysicalDevice device
	);
	bool							isDeviceSuitable(
		VkPhysicalDevice device
	);

}; // class Device

/* ========================================================================== */
/*                                    OTHER                                   */
/* ========================================================================== */

VkFormat	findSupportedFormat(
	VkPhysicalDevice physical_device,
	const std::vector<VkFormat>& candidates,
	VkImageTiling tiling,
	VkFormatFeatureFlags features
);

QueueFamilyIndices	findQueueFamilies(
	VkPhysicalDevice device,
	VkSurfaceKHR vk_surface
);

} // namespace graphics
} // namespace scop

