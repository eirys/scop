/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture_sampler.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 20:25:44 by etran             #+#    #+#             */
/*   Updated: 2023/06/02 17:07:01 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "texture_sampler.hpp"
#include "engine.hpp"
#include "image_handler.hpp"
#include "device.hpp"
#include "utils.hpp"

#include <cmath> // std::floor
#include <algorithm> // std::max
#include <stdexcept> // std::runtime_error
#include <cstring> // memcpy

namespace scop {
namespace graphics {

/* ========================================================================== */
/*                                   PUBLIC                                   */
/* ========================================================================== */

void	TextureSampler::init(
	Device& device,
	VkCommandPool command_pool,
	const scop::Image& image
) {
	createTextureImage(device, command_pool, image);
	createTextureImageView(device);
	createTextureSampler(device);
}

void	TextureSampler::destroy(Device& device) {
	vkDestroySampler(device.logical_device, vk_texture_sampler, nullptr);
	vkDestroyImageView(device.logical_device, vk_texture_image_view, nullptr);
	vkDestroyImage(device.logical_device, vk_texture_image, nullptr);
	vkFreeMemory(device.logical_device, vk_texture_image_memory, nullptr);
}

/* ========================================================================== */
/*                                   PRIVATE                                  */
/* ========================================================================== */

/**
 * Texture loader
*/
void	TextureSampler::createTextureImage(
	Device& device,
	VkCommandPool command_pool,
	const scop::Image& image
) {
	mip_levels = 1 + static_cast<uint32_t>(
		std::floor(std::log2(std::max(
			image.getWidth(),
			image.getHeight()
		)))
	);

	VkDeviceSize	image_size = image.getWidth() * image.getHeight() * sizeof(uint32_t);
	VkBuffer		staging_buffer;
	VkDeviceMemory	staging_buffer_memory;

	device.createBuffer(
		image_size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		staging_buffer,
		staging_buffer_memory
	);

	// Map buffer, copy image load into buffer
	void*	data;
	vkMapMemory(device.logical_device, staging_buffer_memory, 0, image_size, 0, &data);
	memcpy(data, image.getPixels(), static_cast<std::size_t>(image_size));
	vkUnmapMemory(device.logical_device, staging_buffer_memory);

	// Create texture image to be filled
	device.createImage(
		image.getWidth(),
		image.getHeight(),
		mip_levels,
		VK_SAMPLE_COUNT_1_BIT,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
		VK_IMAGE_USAGE_TRANSFER_DST_BIT |
		VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		vk_texture_image,
		vk_texture_image_memory
	);

	// Copy staging buffer to texture image
	transitionImageLayout(
		device.logical_device,
		command_pool,
		device.graphics_queue,
		vk_texture_image,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		mip_levels
	);
	copyBufferToImage(
		device.logical_device,
		device.graphics_queue,
		command_pool,
		staging_buffer,
		vk_texture_image,
		static_cast<uint32_t>(image.getWidth()),
		static_cast<uint32_t>(image.getHeight())
	);

	// Fill mipmaps images (directly handled by gpu)
	generateMipmaps(
		device,
		command_pool,
		vk_texture_image,
		VK_FORMAT_R8G8B8A8_SRGB,
		image.getWidth(),
		image.getHeight(),
		mip_levels
	);

	vkDestroyBuffer(device.logical_device, staging_buffer, nullptr);
	vkFreeMemory(device.logical_device, staging_buffer_memory, nullptr);
}

/**
 * Same concept as swap chain image views
*/
void	TextureSampler::createTextureImageView(Device& device) {
	vk_texture_image_view = createImageView(
		device.logical_device,
		vk_texture_image,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_ASPECT_COLOR_BIT,
		mip_levels
	);
}

/**
 * Create sampler, that'll apply transformations to image when sampling
*/
void	TextureSampler::createTextureSampler(
	Device& device
) {
	VkPhysicalDeviceProperties	properties{};
	vkGetPhysicalDeviceProperties(device.physical_device, &properties);

	VkSamplerCreateInfo	sampler_info{};
	sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler_info.magFilter = VK_FILTER_LINEAR;
	sampler_info.minFilter = VK_FILTER_LINEAR;
	sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.anisotropyEnable = VK_TRUE;
	sampler_info.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	sampler_info.unnormalizedCoordinates = VK_FALSE;
	sampler_info.compareEnable = VK_FALSE;
	sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
	sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler_info.mipLodBias = 0.0f;
	sampler_info.minLod = 0.0f;
	sampler_info.maxLod = static_cast<float>(mip_levels);

	if (vkCreateSampler(device.logical_device, &sampler_info, nullptr, &vk_texture_sampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler");
	}
}

/**
 * Record command to generate mipmaps levels
*/
void	TextureSampler::generateMipmaps(
	Device& device,
	VkCommandPool command_pool,
	VkImage image,
	VkFormat image_format,
	int32_t tex_width,
	int32_t tex_height,
	uint32_t mip_level
) const {
	// Check if support blitting
	VkFormatProperties	properties;
	vkGetPhysicalDeviceFormatProperties(device.physical_device, image_format, &properties);

	if (!(properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
		throw std::runtime_error("texture image format doesn't support linear blitting");
	}

	VkCommandBuffer	command_buffer = beginSingleTimeCommands(
		device.logical_device,
		command_pool
	);

	VkImageMemoryBarrier	barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image = image;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.levelCount = 1;

	int32_t	mip_width = tex_width;
	int32_t	mip_height = tex_height;

	// Redefine parts of barrier for each level
	for (uint32_t i = 1; i < mip_level; ++i) {
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(
			command_buffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&barrier
		);

		// Define blit region
		VkImageBlit	blit{};
		blit.srcOffsets[0] = { 0, 0, 0 };
		blit.srcOffsets[1] = { mip_width, mip_height, 1 };
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		blit.dstOffsets[0] = { 0, 0, 0 };
		blit.dstOffsets[1] = {
			mip_width > 1 ? mip_width / 2 : 1,
			mip_height > 1 ? mip_height / 2: 1,
			1
		};
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		// Record blit command
		vkCmdBlitImage(
			command_buffer,
			image,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&blit,
			VK_FILTER_LINEAR
		);

		// Transition to shader readable layout
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(
			command_buffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&barrier
		);

		// Set next mip level size
		if (mip_width > 1) {
			mip_width /= 2;
		}
		if (mip_height > 1) {
			mip_height /= 2;
		}
	}

	// Last mip level
	barrier.subresourceRange.baseMipLevel = mip_level - 1;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(
		command_buffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		0,
		0,
		nullptr,
		0,
		nullptr,
		1,
		&barrier
	);

	endSingleTimeCommands(
		device.logical_device,
		device.graphics_queue,
		command_pool,
		command_buffer
	);
}

/* ========================================================================== */
/*                                    OTHER                                   */
/* ========================================================================== */

void	transitionImageLayout(
	VkDevice device,
	VkCommandPool command_pool,
	VkQueue graphics_queue,
	VkImage image,
	VkFormat format,
	VkImageLayout old_layout,
	VkImageLayout new_layout,
	uint32_t mip_level
) {
	(void)format;
	VkCommandBuffer	command_buffer = beginSingleTimeCommands(
		device,
		command_pool
	);

	// Create image memory barrier to synchronize proper access to resources
	VkImageMemoryBarrier	barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = old_layout;
	barrier.newLayout = new_layout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = mip_level;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = 0;

	// Set access maks depending on layout in transition,
	// cause multiple actions will be performed during pipeline execution
	VkPipelineStageFlags	src_stage;
	VkPipelineStageFlags	dst_stage;

	if (
		old_layout == VK_IMAGE_LAYOUT_UNDEFINED
		&& new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
	) {
		// Transfer destination doesn't need waiting, so do it in the beginning
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	} else if (
		old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
		&& new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	) {
		// Shader reading needs to wait for transfer to finish
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	} else {
		throw std::invalid_argument("unsupported layout transition");
	}

	// Submit barrier
	vkCmdPipelineBarrier(
		command_buffer,
		src_stage,
		dst_stage,
		0,
		0,
		nullptr,
		0,
		nullptr,
		1,
		&barrier
	);

	endSingleTimeCommands(
		device,
		graphics_queue,
		command_pool,
		command_buffer
	);
}

} // namespace graphics
} // namespace scop