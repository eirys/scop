/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vk_swap_chain.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 17:41:07 by etran             #+#    #+#             */
/*   Updated: 2023/05/15 17:54:17 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "render_target.hpp"
#include "device.hpp"
#include "engine.hpp"
#include "window.hpp"

#include <algorithm> // std::min, std::max
#include <stdexcept> // std::runtime_error

namespace scop {
namespace graphics {

/* ========================================================================== */
/*                                   PUBLIC                                   */
/* ========================================================================== */

void	RenderTarget::init(
	Device& device,
	scop::Window& window
) {
	createSwapChain(device, window);
	createRenderPass(device);
	createFrameBuffers(device);
}

void	RenderTarget::destroy(
	Device& device
) {
	destroySwapChain(device);
	vkDestroyRenderPass(device.logical_device, vk_render_pass, nullptr);
}

void	RenderTarget::updateSwapChain(
	Device& device,
	scop::Window& window
) {
	window.pause();
	device.idle();

	destroySwapChain(device);
	createSwapChain(device, window);
	createFrameBuffers(device);
}

/* ========================================================================== */
/*                                   PRIVATE                                  */
/* ========================================================================== */

void	RenderTarget::createSwapChain(
	Device& device,
	scop::Window& window
) {
	createSwapChainObject(device, window);
	createImageViews(device);
	resources.init(device, swap_chain_extent, swap_chain_image_format);
}

void	RenderTarget::createRenderPass(Device& device) {
	// Color attachment creation
	VkAttachmentDescription	color_attachment{};
	color_attachment.format = swap_chain_image_format;
	color_attachment.samples = device.msaa_samples;
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// Resolve multisampled image to regular image (
	// 	VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	// 	-> VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	// )
	VkAttachmentDescription	color_attachment_resolve{};
	color_attachment_resolve.format = swap_chain_image_format;
	color_attachment_resolve.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attachment_resolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment_resolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment_resolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment_resolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attachment_resolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment_resolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	// Depth attachment creation
	VkAttachmentDescription	depth_attachment{};
	depth_attachment.format = findDepthFormat(device.physical_device);
	depth_attachment.samples = device.msaa_samples;
	depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// Create single subpass of render pass
	VkAttachmentReference	color_attachment_ref{};
	color_attachment_ref.attachment = 0;
	color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference	depth_attachment_ref{};
	depth_attachment_ref.attachment = 1;
	depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference	color_attachment_resolve_ref{};
	color_attachment_resolve_ref.attachment = 2;
	color_attachment_resolve_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription	subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment_ref;
	subpass.pDepthStencilAttachment = &depth_attachment_ref;
	subpass.pResolveAttachments = &color_attachment_resolve_ref;

	std::array<VkAttachmentDescription, 3>	attachments = {
		color_attachment,
		depth_attachment,
		color_attachment_resolve
	};

	// Subpass dependency
	VkSubpassDependency	dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask =
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
		VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask =
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
		VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask =
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
		VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	// Create render pass
	VkRenderPassCreateInfo	create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
	create_info.pAttachments = attachments.data();
	create_info.subpassCount = 1;
	create_info.pSubpasses = &subpass;
	create_info.dependencyCount = 1;
	create_info.pDependencies = &dependency;

	if (vkCreateRenderPass(device.logical_device, &create_info, nullptr, &vk_render_pass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass");
	}
}

/* ========================================================================== */

void	RenderTarget::createSwapChainObject(
	Device& device,
	scop::Window& window
) {
	SwapChainSupportDetails	swap_chain_support = querySwapChainSupport(
		device.physical_device,
		device.vk_surface
	);

	// Setup options for functionning swap chain
	VkSurfaceFormatKHR	surface_format = chooseSwapSurfaceFormat(
		swap_chain_support.formats
	);
	VkPresentModeKHR	present_mode = chooseSwapPresentMode(
		swap_chain_support.present_modes
	);
	VkExtent2D			swap_extent = chooseSwapExtent(
		swap_chain_support.capabilities,
		window
	);

	// Nb of images in the swap chain
	uint32_t	image_count = swap_chain_support.capabilities.minImageCount + 1;

	if (swap_chain_support.capabilities.maxImageCount > 0
	&& image_count > swap_chain_support.capabilities.maxImageCount) {
		// Avoid value exceeding max
		image_count = swap_chain_support.capabilities.maxImageCount;
	}

	// Setup the creation of the swap chain object, tied to the vk_surface
	VkSwapchainCreateInfoKHR	create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = device.vk_surface;
	create_info.minImageCount = image_count;
	create_info.imageFormat = surface_format.format;
	create_info.imageColorSpace = surface_format.colorSpace;
	create_info.imageExtent = swap_extent;
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	// Queue family swap handling:
	// - graphics queue -> drawing to swap chain
	// - present queue -> get passed the swap chain to be submitted
	QueueFamilyIndices	indices = findQueueFamilies(device.physical_device, device.vk_surface);
	uint32_t			queue_family_indices[] = {
		indices.graphics_family.value(),
		indices.present_family.value()
	};

	if (indices.graphics_family != indices.present_family) {
		// No image ownership transfer
		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		create_info.queueFamilyIndexCount = 2;
		create_info.pQueueFamilyIndices = queue_family_indices;
	} else {
		// Explicit image ownership needed between queue families
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = 0;
		create_info.pQueueFamilyIndices = nullptr;
	}

	create_info.preTransform = swap_chain_support.capabilities.currentTransform;
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	create_info.presentMode = present_mode;
	create_info.clipped = VK_TRUE;
	create_info.oldSwapchain = VK_NULL_HANDLE;

	// Create the object and images
	if (vkCreateSwapchainKHR(device.logical_device, &create_info, nullptr, &vk_swap_chain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain");
	}

	// Retrieve image handles
	vkGetSwapchainImagesKHR(device.logical_device, vk_swap_chain, &image_count, nullptr);
	swap_chain_images.resize(image_count);
	vkGetSwapchainImagesKHR(device.logical_device, vk_swap_chain, &image_count, swap_chain_images.data());

	swap_chain_image_format = surface_format.format;
	swap_chain_extent = swap_extent;
}

void	RenderTarget::destroySwapChain(Device& device) {
	resources.destroy(device);

	for (std::size_t i = 0; i < swap_chain_frame_buffers.size(); ++i) {
		vkDestroyFramebuffer(
			device.logical_device,
			swap_chain_frame_buffers[i],
			nullptr
		);
	}
	for (std::size_t i = 0; i < swap_chain_image_views.size(); ++i) {
		vkDestroyImageView(
			device.logical_device,
			swap_chain_image_views[i],
			nullptr
		);
	}

	// Remove swap chain handler
	vkDestroySwapchainKHR(device.logical_device, vk_swap_chain, nullptr);
}

/**
 * Create frame buffers wrapping each swap chain image view
*/
void	RenderTarget::createFrameBuffers(Device& device) {
	swap_chain_frame_buffers.resize(swap_chain_image_views.size());
	for (std::size_t i = 0; i < swap_chain_image_views.size(); ++i) {
		std::array<VkImageView, 3>	attachments = {
			resources.color_image_view,
			resources.depth_image_view,
			swap_chain_image_views[i]
		};

		// Create frame buffer from image view, associate with a render pass
		VkFramebufferCreateInfo	create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		create_info.renderPass = vk_render_pass;
		create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
		create_info.pAttachments = attachments.data();
		create_info.width = swap_chain_extent.width;
		create_info.height = swap_chain_extent.height;
		create_info.layers = 1;

		if (vkCreateFramebuffer(device.logical_device, &create_info, nullptr, &swap_chain_frame_buffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create frame buffer");
		}
	}
}

/**
 * Color targets
*/
void	RenderTarget::createImageViews(Device& device) {
	// Create image view for each images
	swap_chain_image_views.resize(swap_chain_images.size());

	for (std::size_t i = 0; i < swap_chain_images.size(); ++i) {
		swap_chain_image_views[i] = createImageView(
			device.logical_device,
			swap_chain_images[i],
			swap_chain_image_format,
			VK_IMAGE_ASPECT_COLOR_BIT,
			1
		);
	}
}

VkSurfaceFormatKHR	RenderTarget::chooseSwapSurfaceFormat(
	const std::vector<VkSurfaceFormatKHR>& available_formats
) const noexcept {
	// check if surface format is adequate
	// Prevalent: sRGB format for non linear color representation
	for (const auto& available_format: available_formats) {
		if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB
		&& available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return available_format;
	}

	// Default case (no wanted format)
	return available_formats[0];
}

VkPresentModeKHR	RenderTarget::chooseSwapPresentMode(
	const std::vector<VkPresentModeKHR>& available_present_modes
) const noexcept {
	// Prevalent: fifo mailbox mode, expansive on energy but efficient
	for (const auto& available_present_mode: available_present_modes) {
		if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return available_present_mode;
		}
	}

	// Default case, doesn't strain on battery
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D	RenderTarget::chooseSwapExtent(
	VkSurfaceCapabilitiesKHR capabilities,
	scop::Window& window
) const {
	// Pick swap extent (~ resolution of the window, in px)
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	} else {
		int	width, height;
		window.retrieveSize(width, height);
		// glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D	actual_extent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actual_extent.width = std::clamp(
			actual_extent.width,
			capabilities.minImageExtent.width,
			capabilities.maxImageExtent.width
		);
		actual_extent.height = std::clamp(
			actual_extent.height,
			capabilities.minImageExtent.height,
			capabilities.maxImageExtent.height
		);
		return actual_extent;
	}
}

/* ========================================================================== */
/*                                    OTHER                                   */
/* ========================================================================== */

SwapChainSupportDetails	querySwapChainSupport(
	VkPhysicalDevice device,
	VkSurfaceKHR vk_surface
) {
	// Make sure the swap chain support is available for this device
	SwapChainSupportDetails	details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vk_surface, &details.capabilities);

	// Query supported surface formats
	uint32_t	format_count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, vk_surface, &format_count, nullptr);

	if (format_count != 0) {
		details.formats.resize(format_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(
			device,
			vk_surface,
			&format_count,
			details.formats.data()
		);
	}

	// Query supported presentation modes
	uint32_t	present_mode_count;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, vk_surface, &present_mode_count, nullptr);

	if (present_mode_count != 0) {
		details.present_modes.resize(present_mode_count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(
			device, vk_surface,
			&present_mode_count,
			details.present_modes.data()
		);
	}

	return details;
}

} // namespace graphics
} // namespace scop