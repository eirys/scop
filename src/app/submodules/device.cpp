/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   device.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/16 01:00:19 by etran             #+#    #+#             */
/*   Updated: 2023/05/17 01:54:59 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "graphics_pipeline.hpp"
#include "device.hpp"
#include "window.hpp"
#include "utils.hpp"

#include <vector> // std::vector
#include <set> // std::set
#include <stdexcept> // std::runtime_error

namespace scop {
namespace graphics {

/* ========================================================================== */
/*                                   PUBLIC                                   */
/* ========================================================================== */

void	Device::init(scop::Window& window, VkInstance instance) {
	createSurface(instance, window);
	pickPhysicalDevice(instance);
	createLogicalDevice();
}

void	Device::destroy(VkInstance instance) {
	vkDestroyDevice(logical_device, nullptr);
	vkDestroySurfaceKHR(instance, vk_surface, nullptr);
}

void	Device::idle() {
	vkDeviceWaitIdle(logical_device);
}

/* ========================================================================== */

/**
 * Map memory and find one suitable with filter and properties
*/
uint32_t	Device::findMemoryType(
	uint32_t type_filter,
	VkMemoryPropertyFlags properties
) const {
	VkPhysicalDeviceMemoryProperties	mem_properties;
	vkGetPhysicalDeviceMemoryProperties(physical_device, &mem_properties);

	for (uint32_t i = 0; i < mem_properties.memoryTypeCount; ++i) {
		if ((mem_properties.memoryTypes[i].propertyFlags & properties) == properties &&
			(type_filter & (1 << i))) {
			return i;
		}
	}
	throw std::runtime_error("failed to find suitable memory type");
}

/**
 * Create image object for vulkan
*/
void	Device::createImage(
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
) {
	VkImageCreateInfo	image_info{};
	image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_info.imageType = VK_IMAGE_TYPE_2D;
	image_info.extent.width = width;
	image_info.extent.height = height;
	image_info.extent.depth = 1;
	image_info.mipLevels = mip_level;
	image_info.arrayLayers = 1;
	image_info.format = format;
	image_info.tiling = tiling;
	image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_info.usage = usage;
	image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	image_info.samples = num_samples;
	image_info.flags = 0;

	if (vkCreateImage(logical_device, &image_info, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image");
	}

	// Allocate memory for image
	VkMemoryRequirements	mem_requirements;
	vkGetImageMemoryRequirements(logical_device, image, &mem_requirements);

	VkMemoryAllocateInfo	alloc_info{};
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.allocationSize = mem_requirements.size;
	alloc_info.memoryTypeIndex = findMemoryType(
		mem_requirements.memoryTypeBits,
		properties
	);

	if (vkAllocateMemory(logical_device, &alloc_info, nullptr, &image_memory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory");
	}

	// Bind memory to instance
	vkBindImageMemory(logical_device, image, image_memory, 0);
}

/**
 * Create a vk buffer and allocate memory for it
*/
void	Device::createBuffer(
	VkDeviceSize size,
	VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties,
	VkBuffer& buffer,
	VkDeviceMemory& buffer_memory
) {
	// Create buffer instance
	VkBufferCreateInfo	buffer_info{};
	buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_info.size = size;
	buffer_info.usage = usage;
	buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(logical_device, &buffer_info, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer");
	}

	// Allocate memory for buffer
	VkMemoryRequirements	mem_requirements;
	vkGetBufferMemoryRequirements(logical_device, buffer, &mem_requirements);

	VkMemoryAllocateInfo	alloc_info{};
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.allocationSize = mem_requirements.size;
	alloc_info.memoryTypeIndex = findMemoryType(
		mem_requirements.memoryTypeBits,
		properties
	);

	if (vkAllocateMemory(logical_device, &alloc_info, nullptr, &buffer_memory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory");
	}

	// Bind memory to instance
	vkBindBufferMemory(logical_device, buffer, buffer_memory, 0);
}

/* ========================================================================== */
/*                                   PRIVATE                                  */
/* ========================================================================== */

/**
 * Establish connection between Vulkan instance and the window system
*/
void	Device::createSurface(
	VkInstance instance,
	scop::Window& window
) {
	if (glfwCreateWindowSurface(instance, window.getWindow(), nullptr, &vk_surface) != VK_SUCCESS)
		throw std::runtime_error("failed to create window surface");
}

/**
 * Pick a physical device (GPU) to use
*/
void	Device::pickPhysicalDevice(VkInstance instance) {
	uint32_t	device_count = 0;
	vkEnumeratePhysicalDevices(instance, &device_count, nullptr);

	if (device_count == 0)
		throw std::runtime_error("failed to find GPUs with vulkan support");

	std::vector<VkPhysicalDevice>	devices(device_count);
	vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

	for (const VkPhysicalDevice& device: devices) {
		if (isDeviceSuitable(device)) {
			physical_device = device;
			msaa_samples = getMaxUsableSampleCount();
			break;
		}
	}

	if (physical_device == VK_NULL_HANDLE)
		throw std::runtime_error("failed to find suitable GPU");
}

/**
 * Setup logical device, the interface between the app and the physical device (GPU).
*/
void	Device::createLogicalDevice() {
	// Indicate that we want to create a single queue, with graphics capabilities
	QueueFamilyIndices	indices = findQueueFamilies(physical_device, vk_surface);

	std::vector<VkDeviceQueueCreateInfo>	queue_create_infos;
	std::set<uint32_t>						unique_queue_families = {
		indices.graphics_family.value(),
		indices.present_family.value()
	};

	float	queue_priority = 1.0f;

	for (uint32_t queue_family: unique_queue_families) {
		VkDeviceQueueCreateInfo	queue_create_info{};

		queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_info.queueFamilyIndex = queue_family;
		queue_create_info.queueCount = 1;
		queue_create_info.pQueuePriorities = &queue_priority;
		queue_create_infos.emplace_back(queue_create_info);
	}

	// Enable device features
	VkPhysicalDeviceFeatures	device_features{};
	device_features.samplerAnisotropy = VK_TRUE;

	VkDeviceCreateInfo			create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_info.pQueueCreateInfos = queue_create_infos.data();
	create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
	create_info.pEnabledFeatures = &device_features;

	// Validation layers
	if (GraphicsPipeline::enable_validation_layers) {
		create_info.enabledLayerCount = static_cast<uint32_t>(
			GraphicsPipeline::validation_layers.size()
		);
		create_info.ppEnabledLayerNames = GraphicsPipeline::validation_layers.data();
	} else {
		create_info.enabledLayerCount = 0;
	}

	// Device extensions enabling, notably for swap chain support
	create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
	create_info.ppEnabledExtensionNames = device_extensions.data();

	if (vkCreateDevice(physical_device, &create_info, nullptr, &logical_device) != VK_SUCCESS)
		throw std::runtime_error("failed to create logical device");

	// Retrieve queue handles
	vkGetDeviceQueue(logical_device, indices.graphics_family.value(), 0, &graphics_queue);
	vkGetDeviceQueue(logical_device, indices.present_family.value(), 0, &present_queue);
}

/**
 * Retrieve max sample count for MSAA (multisample antialiasing)
*/
VkSampleCountFlagBits	Device::getMaxUsableSampleCount() const {
	VkPhysicalDeviceProperties	properties;
	vkGetPhysicalDeviceProperties(physical_device, &properties);

	VkSampleCountFlags	count =
		properties.limits.framebufferColorSampleCounts &
		properties.limits.framebufferDepthSampleCounts;

	VkSampleCountFlagBits	values[] = {
		VK_SAMPLE_COUNT_64_BIT,
		VK_SAMPLE_COUNT_32_BIT,
		VK_SAMPLE_COUNT_16_BIT,
		VK_SAMPLE_COUNT_8_BIT,
		VK_SAMPLE_COUNT_4_BIT,
		VK_SAMPLE_COUNT_2_BIT
	};

	for (VkSampleCountFlagBits value : values) {
		if (count & value) {
			return value;
		}
	}
	return VK_SAMPLE_COUNT_1_BIT;
}

/**
 * Check if all required extensions are available for the physical device
*/
bool	Device::checkDeviceExtensionSupport(VkPhysicalDevice device) {
	// Verify that every device_extensions are available
	uint32_t	extension_count;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

	std::vector<VkExtensionProperties>	available_extensions(extension_count);
	vkEnumerateDeviceExtensionProperties(
		device,
		nullptr,
		&extension_count,
		available_extensions.data()
	);

	std::set<std::string>	required_extensions(
		device_extensions.begin(),
		device_extensions.end()
	);
	for (const auto& extension: available_extensions) {
		required_extensions.erase(extension.extensionName);
	}

	return required_extensions.empty();
}

/**
 * Verify that the selected physical device is suitable for the app needs
*/
bool	Device::isDeviceSuitable(VkPhysicalDevice device) {
	QueueFamilyIndices	indices = findQueueFamilies(device, vk_surface);
	bool	extensions_supported = checkDeviceExtensionSupport(device);
	bool	swap_chain_adequate = false;

	if (extensions_supported) {
		SwapChainSupportDetails	swap_chain_support = querySwapChainSupport(
			device,
			vk_surface
		);
		swap_chain_adequate =
			!swap_chain_support.formats.empty() &&
			!swap_chain_support.present_modes.empty();
	}

	VkPhysicalDeviceFeatures	supported_features;
	vkGetPhysicalDeviceFeatures(device, &supported_features);

	return (
		indices.isComplete() &&
		extensions_supported &&
		swap_chain_adequate &&
		supported_features.samplerAnisotropy
	);
}

/* ========================================================================== */
/*                                    OTHER                                   */
/* ========================================================================== */

/**
 * Find best suited format for depth image
*/
VkFormat	findSupportedFormat(
	VkPhysicalDevice physical_device,
	const std::vector<VkFormat>& candidates,
	VkImageTiling tiling,
	VkFormatFeatureFlags features
) {
	for (VkFormat format: candidates) {
		// Query format properties for candidate
		VkFormatProperties	properties;
		vkGetPhysicalDeviceFormatProperties(physical_device, format, &properties);

		if ((
			tiling == VK_IMAGE_TILING_LINEAR
			&& (properties.linearTilingFeatures & features) == features
		) || (
			tiling == VK_IMAGE_TILING_OPTIMAL
			&& (properties.optimalTilingFeatures & features) == features
		)) {
			return format;
		}
	}
	throw std::runtime_error("failed to find supported format");
}

/**
 * Retrieve queue families that are appropriate for the physical device and the app needs.
*/
QueueFamilyIndices	findQueueFamilies(
	VkPhysicalDevice device,
	VkSurfaceKHR vk_surface
) {
	QueueFamilyIndices	indices;
	uint32_t			queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

	std::vector<VkQueueFamilyProperties>	queue_families(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

	int	i = 0;
	for (const auto& queue_family: queue_families) {
		// Looking for queue family that supports the graphics bit flag
		if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices.graphics_family = i;

		// Looking for queue family that supports presenting
		VkBool32	present_support = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, vk_surface, &present_support);
		if (present_support)
			indices.present_family = i;

		if (indices.isComplete())
			break;
		++i;
	}

	return indices;
}

} // namespace graphics
} // namespace scop