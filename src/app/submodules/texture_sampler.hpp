/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture_sampler.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 20:18:58 by etran             #+#    #+#             */
/*   Updated: 2023/05/16 17:29:51 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# ifndef GLFW_INCLUDE_VULKAN
#  define GLFW_INCLUDE_VULKAN
# endif

# include <GLFW/glfw3.h>

# include <chrono>

namespace scop {
class Image;

namespace graphics {
class Device;
class DescriptorSet;

class TextureSampler {
public:

	friend DescriptorSet;

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	TextureSampler() = default;
	TextureSampler(TextureSampler&& other) = default;
	~TextureSampler() = default;

	TextureSampler(const TextureSampler& other) = delete;
	TextureSampler& operator=(const TextureSampler& other) = delete;

	/* ========================================================================= */

	void							init(
		Device& device,
		VkCommandPool command_pool,
		const scop::Image& image
	);

	void							destroy(Device& device);
	
private:
	/* ========================================================================= */
	/*                                CLASS MEMBER                               */
	/* ========================================================================= */

	uint32_t						mip_levels;
	VkImage							vk_texture_image;
	VkDeviceMemory					vk_texture_image_memory;
	VkImageView						vk_texture_image_view;
	VkSampler 						vk_texture_sampler;

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	void							createTextureImage(
		Device& device,
		VkCommandPool command_pool,
		const scop::Image& image
	);
	void							createTextureImageView(Device& device);
	void							createTextureSampler(
		Device& device
	);
	void							generateMipmaps(
		Device& device,
		VkCommandPool command_pool,
		VkImage image,
		VkFormat image_format,
		int32_t tex_width,
		int32_t tex_height,
		uint32_t mip_level
	) const;

}; // class TextureSampler

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
);

} // namespace graphics
}  // namespace scop

