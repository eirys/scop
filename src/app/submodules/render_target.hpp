/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vk_swap_chain.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 17:18:40 by etran             #+#    #+#             */
/*   Updated: 2023/05/15 17:52:12 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// Graphics
# ifndef GLFW_INCLUDE_VULKAN
#  define GLFW_INCLUDE_VULKAN
# endif

# include <GLFW/glfw3.h>

// Std
# include <optional>	// std::optional
# include <vector>		// std::vector

# include "render_target_resources.hpp"

namespace scop {
class Window;
	
namespace graphics {
class Engine;

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR		capabilities;
	std::vector<VkSurfaceFormatKHR>	formats;
	std::vector<VkPresentModeKHR>	present_modes;
};

class RenderTarget {
public:

	friend Engine;

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	RenderTarget() = default;
	RenderTarget(RenderTarget&& other) = default;
	~RenderTarget() = default;

	RenderTarget(const RenderTarget& other) = delete;
	RenderTarget& operator=(const RenderTarget& other) = delete;

	/* ========================================================================= */

	void							init(
		Device& device,
		scop::Window& window
	);
	void							destroy(Device& device);
	void							updateSwapChain(
		Device& device,
		scop::Window& window
	);

private:
	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	VkSwapchainKHR					vk_swap_chain;
	VkRenderPass					vk_render_pass;

	std::vector<VkImage>			swap_chain_images;
	VkFormat						swap_chain_image_format;
	VkExtent2D						swap_chain_extent;

	std::vector<VkImageView>		swap_chain_image_views;
	std::vector<VkFramebuffer>		swap_chain_frame_buffers;

	RenderTargetResources			resources;

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	void							createSwapChain(
		Device& device,
		scop::Window& window
	);
	void							createRenderPass(Device& device);

	/* ========================================================================= */

	void							createSwapChainObject(
		Device& device,
		scop::Window& window
	);
	void							destroySwapChain(Device& device);
	void							createFrameBuffers(Device& device);
	void							createImageViews(Device& device);

	VkSurfaceFormatKHR				chooseSwapSurfaceFormat(
		const std::vector<VkSurfaceFormatKHR>& available_formats
	) const noexcept;
	VkPresentModeKHR				chooseSwapPresentMode(
		const std::vector<VkPresentModeKHR>& available_present_modes
	) const noexcept;
	VkExtent2D						chooseSwapExtent(
		VkSurfaceCapabilitiesKHR capabilities,
		scop::Window& window
	) const;

}; // class RenderTarget

/* ========================================================================== */
/*                                    OTHER                                   */
/* ========================================================================== */

SwapChainSupportDetails	querySwapChainSupport(
	VkPhysicalDevice device,
	VkSurfaceKHR vk_surface
);

} // namespace graphics
} // namespace scop

