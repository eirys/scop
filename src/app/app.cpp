/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   app.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/28 11:12:12 by eli               #+#    #+#             */
/*   Updated: 2023/05/06 13:11:31 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app.hpp"
#include "matrix.hpp"

#ifndef STB_IMAGE_IMPLEMENTATION
# define STB_IMAGE_IMPLEMENTATION
# include "stb_image.h"
#endif

#ifndef TINYOBJLOADER_IMPLEMENTATION
# define TINYOBJLOADER_IMPLEMENTATION
# include "tiny_obj_loader.h"
#endif

/* ========================================================================== */
/*                                   PUBLIC                                   */
/* ========================================================================== */

void	App::run() {
	initVulkan();
	mainLoop();
	cleanup();
}

void	App::toggleFrameBufferResized(bool resized) {
	frame_buffer_resized = resized;
}

/* ========================================================================== */
/*                                   PRIVATE                                  */
/* ========================================================================== */

void	App::initVulkan() {
	createInstance();
	setupDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createImageViews();
	createRenderPass();
	createDescriptorSetLayout();
	createGraphicsPipeline();
	createCommandPool();
	createColorResources();
	createDepthResources();
	createFrameBuffers();
	createTextureImage();
	createTextureImageView();
	createTextureSampler();
	loadModel();
	createVertexBuffer();
	createIndexBuffer();
	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
	createCommandBuffers();
	createSyncObjects();
}

void	App::mainLoop() {
	while (window.alive()) {
		// Poll events while not pressing x...
		window.await();
		drawFrame();
	}

	// Wait for logical device to finish executing everything before closing
	vkDeviceWaitIdle(logical_device);
}

void	App::cleanupSwapChain() {
	// Remove msaa resources
	vkDestroyImageView(logical_device, color_image_view, nullptr);
	vkDestroyImage(logical_device, color_image, nullptr);
	vkFreeMemory(logical_device, color_image_memory, nullptr);

	// Remove depth handler
	vkDestroyImageView(logical_device, depth_image_view, nullptr);
	vkDestroyImage(logical_device, depth_image, nullptr);
	vkFreeMemory(logical_device, depth_image_memory, nullptr);
	
	// Remove frame buffers
	for (size_t i = 0; i < swap_chain_frame_buffers.size(); ++i) {
		vkDestroyFramebuffer(logical_device, swap_chain_frame_buffers[i], nullptr);
	}

	// Destroy image view instances
	for (size_t i = 0; i < swap_chain_image_views.size(); ++i) {
		vkDestroyImageView(logical_device, swap_chain_image_views[i], nullptr);
	}

	// Remove swap chain handler
	vkDestroySwapchainKHR(logical_device, swap_chain, nullptr);
}

void	App::cleanup() {
	cleanupSwapChain();

	// Remove texture image
	vkDestroySampler(logical_device, texture_sampler, nullptr);
	vkDestroyImageView(logical_device, texture_image_view, nullptr);
	vkDestroyImage(logical_device, texture_image, nullptr);
	vkFreeMemory(logical_device, texture_image_memory, nullptr);

	// Remove graphics pipeline
	vkDestroyPipeline(logical_device, graphics_pipeline, nullptr);
	vkDestroyPipelineLayout(logical_device, pipeline_layout, nullptr);

	// Remove render pass
	vkDestroyRenderPass(logical_device, render_pass, nullptr);

	// Remove uniform buffers
	for (size_t i = 0; i < max_frames_in_flight; ++i) {
		vkDestroyBuffer(logical_device, uniform_buffers[i], nullptr);
		vkFreeMemory(logical_device, uniform_buffers_memory[i], nullptr);
	}

	// Remove descriptor pool
	vkDestroyDescriptorPool(logical_device, descriptor_pool, nullptr);

	// Remove descriptor set layout
	vkDestroyDescriptorSetLayout(logical_device, descriptor_set_layout, nullptr);

	// Remove index buffer && vertex buffer
	vkDestroyBuffer(logical_device, index_buffer, nullptr);
	vkFreeMemory(logical_device, index_buffer_memory, nullptr);
	vkDestroyBuffer(logical_device, vertex_buffer, nullptr);
	vkFreeMemory(logical_device, vertex_buffer_memory, nullptr);

	// Remove sync objects
	for (size_t i = 0; i < max_frames_in_flight; ++i) {
		vkDestroySemaphore(logical_device, image_available_semaphores[i], nullptr);
		vkDestroySemaphore(logical_device, render_finished_semaphores[i], nullptr);
		vkDestroyFence(logical_device, in_flight_fences[i], nullptr);
	}

	// Remove command pool (and command buffers)
	vkDestroyCommandPool(logical_device, command_pool, nullptr);

	// Remove device
	vkDestroyDevice(logical_device, nullptr);

	// Remove debug object
	if (enable_validation_layers) {
		DestroyDebugUtilsMessengerEXT(vk_instance, debug_messenger, nullptr);
	}

	// Remove vk surface && vk instance
	vkDestroySurfaceKHR(vk_instance, vk_surface, nullptr);
	vkDestroyInstance(vk_instance, nullptr);
}

/**
 * Create a Vulkan instance
*/
void	App::createInstance() {
	// Check if validation layers are available
	if (enable_validation_layers && !checkValidationLayerSupport())
		throw std::runtime_error("validation layers requested but not availalbe");

	// Provides information to driver
	VkApplicationInfo	app_info{};

	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "Hello Triangle";
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.pEngineName = "No engine";
	app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion = VK_API_VERSION_1_0;

	// Pass those informations to the Vulkan driver
	VkInstanceCreateInfo	create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;

	std::vector<const char*>	extensions = getRequiredExtensions();
	create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	create_info.ppEnabledExtensionNames = extensions.data();

	// Setup debug messenger to go along with the instance
	VkDebugUtilsMessengerCreateInfoEXT	debug_create_info{};
	if (enable_validation_layers) {
		create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
		create_info.ppEnabledLayerNames = validation_layers.data();
		populateDebugMessengerCreateInfo(debug_create_info);
		create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;
	} else {
		create_info.enabledLayerCount = 0;
		create_info.pNext = nullptr;
	}

	// Create the instance
	if (vkCreateInstance(&create_info, nullptr, &vk_instance) != VK_SUCCESS)
		throw std::runtime_error("failed to create vk_instance");
}

/**
 * Create a debug messenger to handle Vulkan errors
*/
void	App::setupDebugMessenger() {
	if (!enable_validation_layers) return;

	VkDebugUtilsMessengerCreateInfoEXT	create_info{};
	populateDebugMessengerCreateInfo(create_info);

	if (CreateDebugUtilsMessengerEXT(vk_instance, &create_info, nullptr, &debug_messenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to setup debug messenger");
	}
}

/**
 * Explicit which debug messages are to be handled
*/
void	App::populateDebugMessengerCreateInfo(
	VkDebugUtilsMessengerCreateInfoEXT& create_info
) {
	create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	create_info.messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	#ifdef __VERBOSE
	create_info.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
	#endif

	create_info.messageType =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	create_info.pfnUserCallback = debugCallback;
	create_info.pUserData = nullptr;
}

/**
 * Check if all required extensions are available for validation layers
*/
bool	App::checkValidationLayerSupport() {
	uint32_t	layer_count;
	vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

	std::vector<VkLayerProperties>	available_layers(layer_count);
	vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

	for (const char* layer_name: validation_layers) {
		bool	found = false;
		for (const VkLayerProperties& layer_properties: available_layers) {
			if (!strcmp(layer_name, layer_properties.layerName)) {
				found = true;
				break;
			}
		}
		if (!found)
			return false;
	}
	return true;
}

/**
 * Pick a physical device (GPU) to use
*/
void	App::pickPhysicalDevice() {
	uint32_t	device_count = 0;
	vkEnumeratePhysicalDevices(vk_instance, &device_count, nullptr);

	if (!device_count)
		throw std::runtime_error("failed to find GPUs with vulkan support");

	std::vector<VkPhysicalDevice>	devices(device_count);
	vkEnumeratePhysicalDevices(vk_instance, &device_count, devices.data());

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
 * Verify that the selected physical device is suitable for the app needs
*/
bool	App::isDeviceSuitable(const VkPhysicalDevice& device) {
	QueueFamilyIndices	indices = findQueueFamilies(device);
	bool	extensions_supported = checkDeviceExtensionSupport(device);
	bool	swap_chain_adequate = false;

	if (extensions_supported) {
		SwapChainSupportDetails	swap_chain_support = querySwapChainSupport(device);
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

/**
 * Check if all required extensions are available for the physical device
*/
bool	App::checkDeviceExtensionSupport(const VkPhysicalDevice& device) {
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
 * Retrieve queue families that are appropriate for the physical device and the app needs.
*/
App::QueueFamilyIndices	App::findQueueFamilies(const VkPhysicalDevice& device) {
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

/**
 * Setup logical device, the interface between the app and the physical device (GPU).
*/
void	App::createLogicalDevice() {
	// Indicate that we want to create a single queue, with graphics capabilities
	QueueFamilyIndices	indices = findQueueFamilies(physical_device);

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
		queue_create_infos.push_back(queue_create_info);
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
	if (enable_validation_layers) {
		create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
		create_info.ppEnabledLayerNames = validation_layers.data();
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
 * Establish connection between Vulkan and the window system
*/
void	App::createSurface() {
	if (glfwCreateWindowSurface(vk_instance, window.getWindow(), nullptr, &vk_surface) != VK_SUCCESS)
		throw std::runtime_error("failed to create window surface");
}

App::SwapChainSupportDetails	App::querySwapChainSupport(
	const VkPhysicalDevice& device
) const {
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

VkSurfaceFormatKHR	App::chooseSwapSurfaceFormat(
	const std::vector<VkSurfaceFormatKHR>& available_formats
) const {
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

VkPresentModeKHR	App::chooseSwapPresentMode(
	const std::vector<VkPresentModeKHR>& available_present_modes
) const {
	// Prevalent: fifo mailbox mode, expansive on energy but efficient
	for (const auto& available_present_mode: available_present_modes) {
		if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return available_present_mode;
		}
	}

	// Default case, doesn't strain on battery
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D	App::chooseSwapExtent(
	const VkSurfaceCapabilitiesKHR& capabilities
) {
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

void	App::createSwapChain() {
	SwapChainSupportDetails	swap_chain_support = querySwapChainSupport(
		physical_device
	);

	// Setup options for functionning swap chain
	VkSurfaceFormatKHR	surface_format = chooseSwapSurfaceFormat(
		swap_chain_support.formats
	);
	VkPresentModeKHR	present_mode = chooseSwapPresentMode(
		swap_chain_support.present_modes
	);
	VkExtent2D			swap_extent = chooseSwapExtent(
		swap_chain_support.capabilities
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
	create_info.surface = vk_surface;
	create_info.minImageCount = image_count;
	create_info.imageFormat = surface_format.format;
	create_info.imageColorSpace = surface_format.colorSpace;
	create_info.imageExtent = swap_extent;
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	// Queue family swap handling:
	// - graphics queue -> drawing to swap chain
	// - present queue -> get passed the swap chain to be submitted
	QueueFamilyIndices	indices = findQueueFamilies(physical_device);
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
	if (vkCreateSwapchainKHR(logical_device, &create_info, nullptr, &swap_chain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain");
	}

	// Retrieve image handles
	vkGetSwapchainImagesKHR(logical_device, swap_chain, &image_count, nullptr);
	swap_chain_images.resize(image_count);
	vkGetSwapchainImagesKHR(logical_device, swap_chain, &image_count, swap_chain_images.data());

	swap_chain_image_format = surface_format.format;
	swap_chain_extent = swap_extent;
}

/**
 * Color targets
*/
void	App::createImageViews() {
	// Create image view for each images
	swap_chain_image_views.resize(swap_chain_images.size());

	for (size_t i = 0; i < swap_chain_images.size(); ++i) {
		swap_chain_image_views[i] = createImageView(
			swap_chain_images[i],
			swap_chain_image_format,
			VK_IMAGE_ASPECT_COLOR_BIT,
			1
		);
	}
}

VkImageView	App::createImageView(
	VkImage image,
	VkFormat format,
	VkImageAspectFlags aspect_flags,
	uint32_t mip_level
) const {
	VkImageViewCreateInfo	view_info{};
	view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_info.image = image;
	view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	view_info.format = format;
	view_info.subresourceRange.aspectMask = aspect_flags;
	view_info.subresourceRange.baseMipLevel = 0;
	view_info.subresourceRange.levelCount = mip_level;
	view_info.subresourceRange.baseArrayLayer = 0;
	view_info.subresourceRange.layerCount = 1;

	VkImageView	image_view;

	if (vkCreateImageView(logical_device, &view_info, nullptr, &image_view) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture image view");
	}
	return image_view;
}

void	App::createRenderPass() {
	// Color attachment creation
	VkAttachmentDescription	color_attachment{};
	color_attachment.format = swap_chain_image_format;
	color_attachment.samples = msaa_samples;
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
	depth_attachment.format = findDepthFormat();
	depth_attachment.samples = msaa_samples;
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

	if (vkCreateRenderPass(logical_device, &create_info, nullptr, &render_pass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass");
	}
}

void	App::createGraphicsPipeline() {
	std::vector<char>	vert_shader_code = readFile(SCOP_VERTEX_SHADER_BINARY);
	std::vector<char>	frag_shader_code = readFile(SCOP_FRAGMENT_SHADER_BINARY);

	// Create shader modules to be used for shader stages
	VkShaderModule		vert_shader_module = createShaderModule(vert_shader_code);
	VkShaderModule		frag_shader_module = createShaderModule(frag_shader_code);

	VkPipelineShaderStageCreateInfo	vert_stage_info{};
	vert_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vert_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vert_stage_info.module = vert_shader_module;
	vert_stage_info.pName = "main";

	VkPipelineShaderStageCreateInfo	frag_stage_info{};
	frag_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	frag_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	frag_stage_info.module = frag_shader_module;
	frag_stage_info.pName = "main";

	VkPipelineShaderStageCreateInfo	shader_stages[] = {
		vert_stage_info,
		frag_stage_info
	};

	// Fixed function state

	// Vertex data input handler
	VkPipelineVertexInputStateCreateInfo	vertex_input_info{};
	auto	binding_description = Vertex::getBindingDescription();
	auto	attribute_descriptions = Vertex::getAttributeDescriptions();

	vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_info.vertexBindingDescriptionCount = 1;
	vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(
		attribute_descriptions.size()
	);
	vertex_input_info.pVertexBindingDescriptions = &binding_description;
	vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.data();

	// Vertex input assembly descriptor: regular triangles here
	VkPipelineInputAssemblyStateCreateInfo	input_assembly_info{};
	input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly_info.primitiveRestartEnable = VK_FALSE;

	// Viewport state
	VkPipelineViewportStateCreateInfo	viewport_state{};
	viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_state.viewportCount = 1;
	viewport_state.scissorCount = 1;

	// Rasterizer setup
	VkPipelineRasterizationStateCreateInfo	rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f;
	rasterizer.depthBiasClamp = 0.0f;
	rasterizer.depthBiasSlopeFactor = 0.0f;

	// Disable multisampling
	VkPipelineMultisampleStateCreateInfo	multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = msaa_samples;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	// Color blending for a single framebuffer setup
	VkPipelineColorBlendAttachmentState	color_blend_attachment{};
	color_blend_attachment.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;
	color_blend_attachment.blendEnable = VK_FALSE;
	color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
	color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo	color_blending{};
	color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blending.logicOpEnable = VK_FALSE;
	color_blending.logicOp = VK_LOGIC_OP_COPY;
	color_blending.attachmentCount = 1;
	color_blending.pAttachments = &color_blend_attachment;
	color_blending.blendConstants[0] = 0.0f;
	color_blending.blendConstants[1] = 0.0f;
	color_blending.blendConstants[2] = 0.0f;
	color_blending.blendConstants[3] = 0.0f;

	// Define depth stencil used for depth buffer
	VkPipelineDepthStencilStateCreateInfo	depth_stencil{};
	depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depth_stencil.depthTestEnable = VK_TRUE;			// fragment depth compared to depth buffer enabled
	depth_stencil.depthWriteEnable = VK_TRUE;			// if test passed, new depth saved in buffer enabled
	depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;	// depth low = object closer
	depth_stencil.depthBoundsTestEnable = VK_FALSE;		// unused. specifies min/max depth bounds
	depth_stencil.minDepthBounds = 0.0f;
	depth_stencil.maxDepthBounds = 1.0f;
	depth_stencil.stencilTestEnable = VK_FALSE;			// unused. typically used for reflection, shadow...
	depth_stencil.front = {};
	depth_stencil.back = {};

	// Enable dynamic states
	std::vector<VkDynamicState>	dynamic_states = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo	dynamic_state{};
	dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
	dynamic_state.pDynamicStates = dynamic_states.data();

	// Pipeline layout setup
	VkPipelineLayoutCreateInfo	pipeline_layout_info{};
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_info.setLayoutCount = 1;
	pipeline_layout_info.pSetLayouts = &descriptor_set_layout;
	pipeline_layout_info.pushConstantRangeCount = 0;
	pipeline_layout_info.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(logical_device, &pipeline_layout_info, nullptr, &pipeline_layout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout");
	}

	// Graphics pipeline
	VkGraphicsPipelineCreateInfo	pipeline_info{};
	pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_info.stageCount = 2;
	pipeline_info.pStages = shader_stages;
	pipeline_info.pVertexInputState = &vertex_input_info;
	pipeline_info.pInputAssemblyState = &input_assembly_info;
	pipeline_info.pViewportState = &viewport_state;
	pipeline_info.pRasterizationState = &rasterizer;
	pipeline_info.pMultisampleState = &multisampling;
	pipeline_info.pDepthStencilState = &depth_stencil;
	pipeline_info.pColorBlendState = &color_blending;
	pipeline_info.pDynamicState = &dynamic_state;
	pipeline_info.layout = pipeline_layout;
	pipeline_info.renderPass = render_pass;
	pipeline_info.subpass = 0;
	pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
	pipeline_info.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(logical_device, VK_NULL_HANDLE, 1, &pipeline_info,
	nullptr, &graphics_pipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline");
	}

	vkDestroyShaderModule(logical_device, frag_shader_module, nullptr);
	vkDestroyShaderModule(logical_device, vert_shader_module, nullptr);
}

/**
 * Read compiled file in binary mode and return in vector of char format.
 * Avoids problems with text files and end of line characters presence.
*/
std::vector<char>	App::readFile(const std::string& filename) {
	// Read file as binary file, at the end of the file
	std::ifstream	file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file: " + filename);
	}

	size_t				file_size = static_cast<size_t>(file.tellg());
	std::vector<char>	buffer(file_size);
	file.seekg(0);
	file.read(buffer.data(), file_size);
	file.close();
	return buffer;
}

/**
 * Create a shader module, from a GLSL shader file, that will be used in the pipeline
*/
VkShaderModule	App::createShaderModule(const std::vector<char>& code) {
	// Create a shader module from code
	VkShaderModuleCreateInfo	create_info{};

	create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.codeSize = code.size();
	create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule	shader_module;
	if (vkCreateShaderModule(logical_device, &create_info, nullptr, &shader_module) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module");
	}
	return shader_module;
}

/**
 * Create frame buffers wrapping each swap chain image view
*/
void	App::createFrameBuffers() {
	swap_chain_frame_buffers.resize(swap_chain_image_views.size());
	for (size_t i = 0; i < swap_chain_image_views.size(); ++i) {
		std::array<VkImageView, 3>	attachments = {
			color_image_view,
			depth_image_view,
			swap_chain_image_views[i]
		};

		// Create frame buffer from image view, associate with a render pass
		VkFramebufferCreateInfo	create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		create_info.renderPass = render_pass;
		create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
		create_info.pAttachments = attachments.data();
		create_info.width = swap_chain_extent.width;
		create_info.height = swap_chain_extent.height;
		create_info.layers = 1;

		if (vkCreateFramebuffer(logical_device, &create_info, nullptr, &swap_chain_frame_buffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create frame buffer");
		}
	}
}

/**
 * Command buffers memory handler
*/
void	App::createCommandPool() {
	QueueFamilyIndices	queue_family_indices = findQueueFamilies(physical_device);

	VkCommandPoolCreateInfo	pool_info{};
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	pool_info.queueFamilyIndex = queue_family_indices.graphics_family.value();

	if (vkCreateCommandPool(logical_device, &pool_info, nullptr, &command_pool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool");
	}
}

void	App::createCommandBuffers() {
	command_buffers.resize(max_frames_in_flight);

	// Command buffers
	VkCommandBufferAllocateInfo	alloc_info{};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.commandPool = command_pool;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandBufferCount = static_cast<uint32_t>(command_buffers.size());

	if (vkAllocateCommandBuffers(logical_device, &alloc_info, command_buffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers");
	}
}

/**
 *  Write commands to command buffer to be subimtted to queue.
 */
void	App::recordCommandBuffer(VkCommandBuffer command_buffer, uint32_t image_index) {
	VkCommandBufferBeginInfo	begin_info{};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = 0;
	begin_info.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer");
	}

	// Define what corresponds to 'clear color'
	std::array<VkClearValue, 2>	clear_values{};
	clear_values[0].color = {{ 0.0f, 0.0f, 0.0f, 1.0f }};
	clear_values[1].depthStencil = { 1.0f, 0 };

	// Spectify to render pass how to handle the command buffer,
	// and which framebuffer to render to
	VkRenderPassBeginInfo	render_pass_info{};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_info.renderPass = render_pass;
	render_pass_info.framebuffer = swap_chain_frame_buffers[image_index];
	render_pass_info.renderArea.offset = { 0, 0 };
	render_pass_info.renderArea.extent = swap_chain_extent;
	render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
	render_pass_info.pClearValues = clear_values.data();

	// Begin rp and bind pipeline
	vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);

	// Set viewport and scissors
	VkViewport	viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swap_chain_extent.width);
	viewport.height = static_cast<float>(swap_chain_extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(command_buffer, 0, 1, &viewport);

	VkRect2D	scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swap_chain_extent;
	vkCmdSetScissor(command_buffer, 0, 1, &scissor);

	// Bind vertex buffer && index buffer
	VkBuffer		vertex_buffers[] = { vertex_buffer };
	VkDeviceSize	offsets[] = { 0 };
	vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);
	vkCmdBindIndexBuffer(command_buffer, index_buffer, 0, VK_INDEX_TYPE_UINT32);

	// Bind descriptor sets
	vkCmdBindDescriptorSets(
		command_buffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipeline_layout,
		0,
		1,
		&descriptor_sets[current_frame],
		0,
		nullptr
	);

	// Issue draw command for triangle
	vkCmdDrawIndexed(command_buffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

	// End render pass
	vkCmdEndRenderPass(command_buffer);

	// Finish recording
	if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer");
	}
}

/**
 * Fence awaited: the cpu waits until the frame is ready to be retrieved.
 *
 * Semaphores awaited: the gpu waits until the command buffer
 * is done executing, aka the image is available in the swap chain.
*/
void	App::drawFrame() {
	// Wait fence available, lock it
	vkWaitForFences(logical_device, 1, &in_flight_fences[current_frame], VK_TRUE, UINT64_MAX);

	// Retrieve available image from swap chain
	uint32_t	image_index;
	VkResult	result = vkAcquireNextImageKHR(
		logical_device,
		swap_chain,
		UINT64_MAX,
		image_available_semaphores[current_frame],
		VK_NULL_HANDLE,
		&image_index
	);
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		// Swap chain incompatible for rendering (resize?)
		recreateSwapChain();
		return;
	} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image");
	}

	// Work is done, unlock fence
	vkResetFences(logical_device, 1, &in_flight_fences[current_frame]);

	// Record buffer
	vkResetCommandBuffer(command_buffers[current_frame], 0);
	recordCommandBuffer(command_buffers[current_frame], image_index);

	updateUniformBuffer(current_frame);

	// Set synchronization objects
	VkSemaphore				wait_semaphore[] = {
		image_available_semaphores[current_frame]
	};
	VkSemaphore				signal_semaphores[] = {
		render_finished_semaphores[current_frame]
	};
	VkPipelineStageFlags	wait_stages[] = {
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
	};
	VkSubmitInfo			submit_info{};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_semaphore;
	submit_info.pWaitDstStageMask = wait_stages;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffers[current_frame];
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphores;

	// Submit command buffer
	if (vkQueueSubmit(graphics_queue, 1, &submit_info, in_flight_fences[current_frame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer");
	}

	// Set presentation (swap chain)
	VkPresentInfoKHR	present_info{};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = signal_semaphores;

	VkSwapchainKHR	swap_chains[] = { swap_chain };
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swap_chains;
	present_info.pImageIndices = &image_index;
	present_info.pResults = nullptr;

	// Submit to swap chain, check if swap chain is still compatible
	result = vkQueuePresentKHR(present_queue, &present_info);

	if (
		result == VK_ERROR_OUT_OF_DATE_KHR ||
		result == VK_SUBOPTIMAL_KHR ||
		frame_buffer_resized
	) {
		frame_buffer_resized = false;
		recreateSwapChain();
	} else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swapchain image");
	}

	// Update current frame
	current_frame = (current_frame + 1) % max_frames_in_flight;
}

/**
 * Create semaphores and fences
*/
void	App::createSyncObjects() {
	image_available_semaphores.resize(max_frames_in_flight);
	render_finished_semaphores.resize(max_frames_in_flight);
	in_flight_fences.resize(max_frames_in_flight);

	VkSemaphoreCreateInfo	semaphore_info{};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo	fence_info{};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < max_frames_in_flight; ++i) {
		if (vkCreateSemaphore(logical_device, &semaphore_info, nullptr, &image_available_semaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(logical_device, &semaphore_info, nullptr, &render_finished_semaphores[i]) != VK_SUCCESS ||
			vkCreateFence(logical_device, &fence_info, nullptr, &in_flight_fences[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create semaphore");
		}
	}
}

/**
 *  Retrieve list of extensions if validation layers enabled
 */
std::vector<const char*>	App::getRequiredExtensions() {
	uint32_t		glfw_extension_count = 0;
	const char**	glfw_extensions;
	glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

	std::vector<const char*>	extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

	if (enable_validation_layers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	return extensions;
}

/**
 * Debug callback function, used by validation layers
*/
VKAPI_ATTR VkBool32 VKAPI_CALL	App::debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
	VkDebugUtilsMessageTypeFlagsEXT message_type,
	const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
	void* p_user_data
) {
	std::cerr << "[validation layer] " << p_callback_data->pMessage << std::endl;
	return VK_FALSE;
}

/**
 * Force recreation of swap chain when not compatible with window
*/
void	App::recreateSwapChain() {
	// int width = 0, height = 0;
	// glfwGetFramebufferSize(window, &width, &height);

	// "Pausing" frame refreshing until window is in foreground
	// while (width == 0 || height == 0) {
	// 	glfwGetFramebufferSize(window, &width, &height);
	// 	glfwWaitEvents();
	// }
	window.pause();
	vkDeviceWaitIdle(logical_device);

	cleanupSwapChain();

	createSwapChain();
	createImageViews();
	createColorResources();
	createDepthResources();
	createFrameBuffers();
}

/**
 * Create the vertex buffer that'll be used to store the vertices of the triangle.
*/
void	App::createVertexBuffer() {
	VkDeviceSize	buffer_size = sizeof(vertices[0]) * vertices.size();

	// Create staging buffer to upload cpu memory to
	VkBuffer		staging_buffer;
	VkDeviceMemory	staging_buffer_memory;

	// Cpu accessible memory
	createBuffer(
		buffer_size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		staging_buffer,
		staging_buffer_memory
	);

	// Fill staging buffer
	void*	data;
	vkMapMemory(logical_device, staging_buffer_memory, 0, buffer_size, 0, &data);
	memcpy(data, vertices.data(), static_cast<size_t>(buffer_size));
	vkUnmapMemory(logical_device, staging_buffer_memory);

	// Create vertex buffer that'll interact with gpu
	createBuffer(
		buffer_size,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		vertex_buffer,
		vertex_buffer_memory
	);

	// Now transfer data from staging buffer to vertex buffer
	copyBuffer(staging_buffer, vertex_buffer, buffer_size);

	// Cleanup staging buffer
	vkDestroyBuffer(logical_device, staging_buffer, nullptr);
	vkFreeMemory(logical_device, staging_buffer_memory, nullptr);
}

/**
 *  Create index buffer (pointers into the vertex buffer)
 */
void	App::createIndexBuffer() {
	VkDeviceSize	buffer_size = sizeof(indices[0]) * indices.size();
	VkBuffer		staging_buffer;
	VkDeviceMemory	staging_buffer_memory;

	createBuffer(
		buffer_size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		staging_buffer,
		staging_buffer_memory
	);

	// Fill staging buffer with indices
	void*	data;
	vkMapMemory(logical_device, staging_buffer_memory, 0, buffer_size, 0, &data);
	memcpy(data, indices.data(), static_cast<size_t>(buffer_size));
	vkUnmapMemory(logical_device, staging_buffer_memory);

	createBuffer(
		buffer_size,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		index_buffer,
		index_buffer_memory
	);

	// Transfer data from staging buffer to index buffer
	copyBuffer(staging_buffer, index_buffer, buffer_size);

	// Flush temporary buffers
	vkDestroyBuffer(logical_device, staging_buffer, nullptr);
	vkFreeMemory(logical_device, staging_buffer_memory, nullptr);
}

/**
 * Create a vk buffer and allocate memory for it
*/
void	App::createBuffer(
	VkDeviceSize size,
	VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties,
	VkBuffer& buffer,
	VkDeviceMemory& buffer_memory
) const {
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

/**
 * Record commands to copy data from one buffer to another,
 * and submit them to the graphics queue.
*/
void	App::copyBuffer(
	VkBuffer src_buffer,
	VkBuffer dst_buffer,
	VkDeviceSize size
) const {
	VkCommandBuffer	command_buffer = beginSingleTimeCommands();

	VkBufferCopy	copy_region{};
	copy_region.srcOffset = 0;
	copy_region.dstOffset = 0;
	copy_region.size = size;
	vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

	endSingleTimeCommands(command_buffer);
}

/**
 * Map memory and find one suitable with filter and properties
*/
uint32_t	App::findMemoryType(
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
 * Descriptor set layout for uniform buffer and combined image sampler
*/
void	App::createDescriptorSetLayout() {
	// Uniform buffer layout: used during vertex shading
	VkDescriptorSetLayoutBinding	ubo_layout_binding{};
	ubo_layout_binding.binding = 0;
	ubo_layout_binding.descriptorCount = 1;
	ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	ubo_layout_binding.pImmutableSamplers = nullptr;
	ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	// Sampler descriptor layout: used during fragment shading
	VkDescriptorSetLayoutBinding	sampler_layout_binding{};
	sampler_layout_binding.binding = 1;
	sampler_layout_binding.descriptorCount = 1;
	sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	sampler_layout_binding.pImmutableSamplers = nullptr;
	sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 2>	bindings = {
		ubo_layout_binding,
		sampler_layout_binding
	};

	VkDescriptorSetLayoutCreateInfo	layout_info{};
	layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
	layout_info.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(logical_device, &layout_info, nullptr, &descriptor_set_layout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout");
	}
}

/**
 * Create uniform buffers
*/
void	App::createUniformBuffers() {
	VkDeviceSize	buffer_size = sizeof(UniformBufferObject);

	uniform_buffers.resize(max_frames_in_flight);
	uniform_buffers_memory.resize(max_frames_in_flight);
	uniform_buffers_mapped.resize(max_frames_in_flight);

	for (size_t i = 0; i < max_frames_in_flight; ++i) {
		createBuffer(
			buffer_size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			uniform_buffers[i],
			uniform_buffers_memory[i]
		);
		vkMapMemory(
			logical_device,
			uniform_buffers_memory[i],
			0,
			buffer_size,
			0,
			&uniform_buffers_mapped[i]
		);
	}
}

/**
 * Update transformation of vertices
*/
void	App::updateUniformBuffer(uint32_t current_image) {
	// Ensure 90deg rotation/sec
	static auto	start_time = std::chrono::high_resolution_clock::now();

	auto	current_time = std::chrono::high_resolution_clock::now();
	float	time = std::chrono::duration<float, std::chrono::seconds::period>(
					current_time - start_time
				).count();

	UniformBufferObject	ubo{};

	// Define model: continuous rotation around z axis
	ubo.model = scop::rotate(
		time * scop::utils::radians(90.0f),	// rotation angle
		scop::Vect3(0.0f, 0.0f, 1.0f)		// axis (z axis)
	);

	// Model: static
	// ubo.model = scop::Mat4(1.0f);

	// Define view transformation: above, 45deg angle
	ubo.view = scop::lookAt(
		scop::Vect3(2.0f, 2.0f, 2.0f),		// eye position
		scop::Vect3(0.0f, 0.0f, 0.0f),		// center position
		scop::Vect3(0.0f, 0.0f, 1.0f)		// up axis (z axis)
	);
	// Define persp. projection (clip space?)
	ubo.proj = scop::perspective(
		scop::utils::radians(45.0f),		// FOV angle
		swap_chain_extent.width				// aspect ratio
		/ static_cast<float>(
				swap_chain_extent.height
		),
		0.1f,								// near view plane
		10.0f								// far view plane
	);
	// Invert y axis (OpenGL has y axis inverted)
	ubo.proj[5] *= -1;
	memcpy(uniform_buffers_mapped[current_image], &ubo, sizeof(ubo));
}

/**
 * Handler for descriptor sets (like command pool)
*/
void	App::createDescriptorPool() {
	std::array<VkDescriptorPoolSize, 2>	pool_sizes{};
	pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pool_sizes[0].descriptorCount = static_cast<uint32_t>(max_frames_in_flight);
	pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	pool_sizes[1].descriptorCount = static_cast<uint32_t>(max_frames_in_flight);

	VkDescriptorPoolCreateInfo	pool_info{};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
	pool_info.pPoolSizes = pool_sizes.data();
	pool_info.maxSets = static_cast<uint32_t>(max_frames_in_flight);

	if (vkCreateDescriptorPool(logical_device, &pool_info, nullptr, &descriptor_pool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool");
	}
}

void	App::createDescriptorSets() {
	std::vector<VkDescriptorSetLayout>	layouts(max_frames_in_flight, descriptor_set_layout);
	VkDescriptorSetAllocateInfo			alloc_info{};
	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.descriptorPool = descriptor_pool;
	alloc_info.descriptorSetCount = static_cast<uint32_t>(max_frames_in_flight);
	alloc_info.pSetLayouts = layouts.data();

	descriptor_sets.resize(max_frames_in_flight);

	if (vkAllocateDescriptorSets(logical_device, &alloc_info, descriptor_sets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets");
	}

	// Populate descriptors
	for (size_t i = 0; i < max_frames_in_flight; ++i) {
		// Uniform buffers
		VkDescriptorBufferInfo	buffer_info{};
		buffer_info.buffer = uniform_buffers[i];
		buffer_info.offset = 0;
		buffer_info.range = sizeof(UniformBufferObject);

		VkDescriptorImageInfo	image_info{};
		image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		image_info.imageView = texture_image_view;
		image_info.sampler = texture_sampler;

		// Update buffer using descriptor write
		std::array<VkWriteDescriptorSet, 2>	descriptor_writes{};
		descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptor_writes[0].dstSet = descriptor_sets[i];
		descriptor_writes[0].dstBinding = 0;
		descriptor_writes[0].dstArrayElement = 0;
		descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptor_writes[0].descriptorCount = 1;
		descriptor_writes[0].pBufferInfo = &buffer_info;
		descriptor_writes[0].pImageInfo = nullptr;
		descriptor_writes[0].pTexelBufferView = nullptr;

		descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptor_writes[1].dstSet = descriptor_sets[i];
		descriptor_writes[1].dstBinding = 1;
		descriptor_writes[1].dstArrayElement = 0;
		descriptor_writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptor_writes[1].descriptorCount = 1;
		descriptor_writes[1].pBufferInfo = nullptr;
		descriptor_writes[1].pImageInfo = &image_info;
		descriptor_writes[1].pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(
			logical_device,
			static_cast<uint32_t>(descriptor_writes.size()),
			descriptor_writes.data(),
			0,
			nullptr
		);
	}
}

/**
 * Texture loader
*/
void	App::createTextureImage() {
	int	tex_width, tex_height, tex_channels;

	// Load image
	stbi_uc*	pixels = stbi_load(
		SCOP_TEXTURE_FILE_VIKING_PNG,
		&tex_width,
		&tex_height,
		&tex_channels,
		STBI_rgb_alpha
	);
	if (!pixels) {
		throw std::runtime_error("failed to load texture image");
	}

	mip_levels = 1 + static_cast<uint32_t>(
		std::floor(std::log2(std::max(
			tex_width,
			tex_height
		)))
	);

	VkDeviceSize	image_size = tex_width * tex_height * 4;
	VkBuffer		staging_buffer;
	VkDeviceMemory	staging_buffer_memory;

	createBuffer(
		image_size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		staging_buffer,
		staging_buffer_memory
	);

	// Map buffer, copy image load into buffer
	void*	data;
	vkMapMemory(logical_device, staging_buffer_memory, 0, image_size, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(image_size));
	vkUnmapMemory(logical_device, staging_buffer_memory);

	// Free image loaded
	stbi_image_free(pixels);

	// Create texture image to be filled
	createImage(
		tex_width,
		tex_height,
		mip_levels,
		VK_SAMPLE_COUNT_1_BIT,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
		VK_IMAGE_USAGE_TRANSFER_DST_BIT |
		VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		texture_image,
		texture_image_memory
	);

	// Copy staging buffer to texture image
	transitionImageLayout(
		texture_image,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		mip_levels
	);
	copyBufferToImage(
		staging_buffer,
		texture_image,
		static_cast<uint32_t>(tex_width),
		static_cast<uint32_t>(tex_height)
	);

	// Fill mipmaps images (directly handled by gpu)
	generateMipmaps(
		texture_image,
		VK_FORMAT_R8G8B8A8_SRGB,
		tex_width,
		tex_height,
		mip_levels
	);

	vkDestroyBuffer(logical_device, staging_buffer, nullptr);
	vkFreeMemory(logical_device, staging_buffer_memory, nullptr);
}

/**
 * Create image object for vulkan
*/
void	App::createImage(
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
) const {
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

VkCommandBuffer	App::beginSingleTimeCommands() const {
	// Allocate temporary command buffer for memory transfer
	VkCommandBufferAllocateInfo	alloc_info{};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandPool = command_pool;
	alloc_info.commandBufferCount = 1;

	VkCommandBuffer	command_buffer;
	vkAllocateCommandBuffers(logical_device, &alloc_info, &command_buffer);

	VkCommandBufferBeginInfo	begin_info{};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(command_buffer, &begin_info);

	return command_buffer;
}

void	App::endSingleTimeCommands(VkCommandBuffer command_buffer) const {
	// Submit to graphics queue to execute transfer
	vkEndCommandBuffer(command_buffer);

	VkSubmitInfo	submit_info{};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffer;

	vkQueueSubmit(graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphics_queue);

	// Deallocate temporary command buffer
	vkFreeCommandBuffers(logical_device, command_pool, 1, &command_buffer);
}

void	App::transitionImageLayout(
	VkImage image,
	VkFormat format,
	VkImageLayout old_layout,
	VkImageLayout new_layout,
	uint32_t mip_level
) const {
	VkCommandBuffer	command_buffer = beginSingleTimeCommands();

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

	endSingleTimeCommands(command_buffer);
}

void	App::copyBufferToImage(
	VkBuffer buffer,
	VkImage image,
	uint32_t width,
	uint32_t height
) const {
	VkCommandBuffer	command_buffer = beginSingleTimeCommands();

	// Specify part of buffer to be copied to image
	VkBufferImageCopy	region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { width, height, 1 };

	vkCmdCopyBufferToImage(
		command_buffer,
		buffer,
		image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);

	endSingleTimeCommands(command_buffer);
}

/**
 * Same concept as swap chain image views
*/
void	App::createTextureImageView() {
	texture_image_view = createImageView(
		texture_image,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_ASPECT_COLOR_BIT,
		mip_levels
	);
}

/**
 * Create sampler, that'll apply transformations to image when sampling
*/
void	App::createTextureSampler() {
	VkPhysicalDeviceProperties	properties{};
	vkGetPhysicalDeviceProperties(physical_device, &properties);

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

	if (vkCreateSampler(logical_device, &sampler_info, nullptr, &texture_sampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler");
	}
}

void	App::createDepthResources() {
	VkFormat	depth_format = findDepthFormat();

	createImage(
		swap_chain_extent.width,
		swap_chain_extent.height,
		1,
		msaa_samples,
		depth_format,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		depth_image,
		depth_image_memory
	);

	depth_image_view = createImageView(
		depth_image,
		depth_format,
		VK_IMAGE_ASPECT_DEPTH_BIT,
		1
	);
}

/**
 * Find best suited format for depth image
*/
VkFormat	App::findSupportedFormat(
	const std::vector<VkFormat>& candidates,
	VkImageTiling tiling,
	VkFormatFeatureFlags features
) const {
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

VkFormat	App::findDepthFormat() const {
	return findSupportedFormat(
		{
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT
		},
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

bool	App::hasStencilCompotent(VkFormat format) const {
	return (
		format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
		format == VK_FORMAT_D24_UNORM_S8_UINT
	);
}

void	App::loadModel() {
	tinyobj::attrib_t					attributes;
	std::vector<tinyobj::shape_t>		shapes;
	std::vector<tinyobj::material_t>	materials;
	std::string							warn, err;

	bool	ret = tinyobj::LoadObj(
		&attributes,
		&shapes,
		&materials,
		&warn,
		&err,
		SCOP_MODEL_FILE_VIKING_OBJ
	);
	if (!ret) {
		throw std::runtime_error(warn + err);
	}

	std::unordered_map<Vertex, uint32_t>	unique_vertices{};

	for (const auto& shape: shapes) {
		for (const auto& index: shape.mesh.indices) {
			Vertex	vertex{};

			vertex.pos = {
				attributes.vertices[3 * index.vertex_index + 0],
				attributes.vertices[3 * index.vertex_index + 1],
				attributes.vertices[3 * index.vertex_index + 2]
			};
			vertex.tex_coord = {
				attributes.texcoords[2 * index.texcoord_index + 0],
				1.0f - attributes.texcoords[2 * index.texcoord_index + 1]
			};
			vertex.color = { 1.0f, 1.0f, 1.0f };

			if (unique_vertices.count(vertex) == 0) {
				unique_vertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}
			indices.push_back(unique_vertices[vertex]);
		}
	}
}

/**
 * Record command to generate mipmaps levels
*/
void	App::generateMipmaps(
	VkImage image,
	VkFormat image_format,
	int32_t tex_width,
	int32_t tex_height,
	uint32_t mip_level
) const {
	// Check if support blitting
	VkFormatProperties	properties;
	vkGetPhysicalDeviceFormatProperties(physical_device, image_format, &properties);

	if (!(properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
		throw std::runtime_error("texture image format doesn't support linear blitting");
	}
	
	VkCommandBuffer	command_buffer = beginSingleTimeCommands();

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

	endSingleTimeCommands(command_buffer);
}

/**
 * Retrieve max sample count for MSAA (multisample antialiasing)
*/
VkSampleCountFlagBits	App::getMaxUsableSampleCount() const {
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
 * Create color buffer for MSAA
*/
void	App::createColorResources() {
	createImage(
		swap_chain_extent.width,
		swap_chain_extent.height,
		1,
		msaa_samples,
		swap_chain_image_format,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		color_image,
		color_image_memory
	);
	color_image_view = createImageView(
		color_image,
		swap_chain_image_format,
		VK_IMAGE_ASPECT_COLOR_BIT,
		1
	);
}

/* ========================================================================== */
/*                                    OTHER                                   */
/* ========================================================================== */

/**
 * Load the debug object creation function if avail
*/
VkResult	CreateDebugUtilsMessengerEXT(
	VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* p_create_info,
	const VkAllocationCallbacks* p_allocator,
	VkDebugUtilsMessengerEXT* p_debug_messenger
) {
	auto	func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
		instance,
		"vkCreateDebugUtilsMessengerEXT"
	);

	if (func != nullptr) {
		return func(instance, p_create_info, p_allocator, p_debug_messenger);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

/**
 * Load the debug object destructor function if available
*/
void	DestroyDebugUtilsMessengerEXT(
	VkInstance instance,
	VkDebugUtilsMessengerEXT debug_messenger,
	const VkAllocationCallbacks* p_allocator
) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
		instance,
		"vkDestroyDebugUtilsMessengerEXT"
	);

	if (func != nullptr) {
		func(instance, debug_messenger, p_allocator);
	}
}