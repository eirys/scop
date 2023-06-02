/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_target_resources.cpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 20:21:48 by etran             #+#    #+#             */
/*   Updated: 2023/06/02 17:07:01 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "render_target_resources.hpp"
#include "engine.hpp"

namespace scop {
namespace graphics {

/* ========================================================================== */
/*                                   PUBLIC                                   */
/* ========================================================================== */

void	RenderTargetResources::init(
	Device& device,
	VkExtent2D extent,
	VkFormat image_format
) {
	createColorResources(device, extent, image_format);
	createDepthResources(device, extent);
}

void	RenderTargetResources::destroy(Device& device) {
	// Remove msaa resources
	vkDestroyImageView(device.logical_device, color_image_view, nullptr);
	vkDestroyImage(device.logical_device, color_image, nullptr);
	vkFreeMemory(device.logical_device, color_image_memory, nullptr);

	// Remove depth handler
	vkDestroyImageView(device.logical_device, depth_image_view, nullptr);
	vkDestroyImage(device.logical_device, depth_image, nullptr);
	vkFreeMemory(device.logical_device, depth_image_memory, nullptr);
}

/* ========================================================================== */
/*                                   PRIVATE                                  */
/* ========================================================================== */

/**
 * Create color buffer for MSAA
*/
void	RenderTargetResources::createColorResources(
	Device& device,
	VkExtent2D extent,
	VkFormat image_format
) {
	device.createImage(
		extent.width,
		extent.height,
		1,
		device.msaa_samples,
		image_format,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		color_image,
		color_image_memory
	);
	color_image_view = createImageView(
		device.logical_device,
		color_image,
		image_format,
		VK_IMAGE_ASPECT_COLOR_BIT,
		1
	);
}

void	RenderTargetResources::createDepthResources(
	Device& device,
	VkExtent2D extent
) {
	VkFormat	depth_format = findDepthFormat(device.physical_device);

	device.createImage(
		extent.width,
		extent.height,
		1,
		device.msaa_samples,
		depth_format,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		depth_image,
		depth_image_memory
	);
	depth_image_view = createImageView(
		device.logical_device,
		depth_image,
		depth_format,
		VK_IMAGE_ASPECT_DEPTH_BIT,
		1
	);
}

/* ========================================================================== */
/*                                    OTHER                                   */
/* ========================================================================== */

VkFormat	findDepthFormat(
	VkPhysicalDevice physical_device
) {
	return findSupportedFormat(
		physical_device,
		{
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT
		},
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

bool	hasStencilCompotent(VkFormat format) noexcept {
	return (
		format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
		format == VK_FORMAT_D24_UNORM_S8_UINT
	);
}

} // namespace graphics
} // namespace scop