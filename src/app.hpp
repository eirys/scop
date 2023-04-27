/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   app.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/10 18:21:34 by eli               #+#    #+#             */
/*   Updated: 2023/04/28 01:44:28 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef APP_HPP
# define APP_HPP

// Graphics
# ifndef GLFW_INCLUDE_VULKAN
#  define GLFW_INCLUDE_VULKAN
# endif
# include <GLFW/glfw3.h>

// Std
# include <iostream>
# include <stdexcept>
# include <cstdlib>
# include <vector>
# include <cstring>
# include <optional>
# include <set>
# include <limits>
# include <algorithm>
# include <fstream>
# include <cassert>

# include "utils.hpp"
# include "vertex.hpp"

# define SCOP_VERTEX_SHADER_BINARY		"shaders/vert.spv"
# define SCOP_FRAGMENT_SHADER_BINARY	"shaders/frag.spv"

/* ========================================================================== */
/*                                    UTILS                                   */
/* ========================================================================== */

VkResult	CreateDebugUtilsMessengerEXT(
	VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* p_create_info,
	const VkAllocationCallbacks* p_allocator,
	VkDebugUtilsMessengerEXT* p_debug_messenger
) {
	// Load the debug object creation function if avail
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

void	DestroyDebugUtilsMessengerEXT(
	VkInstance instance,
	VkDebugUtilsMessengerEXT debug_messenger,
	const VkAllocationCallbacks* p_allocator
) {
	// Load the debug object destructor function if avail
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
		instance,
		"vkDestroyDebugUtilsMessengerEXT"
	);

	if (func != nullptr) {
		func(instance, debug_messenger, p_allocator);
	}
}

/* -------------------------------------------------------------------------- */

class App {
public:
	/* ========================================================================= */
	/*                               HELPER OBJECT                               */
	/* ========================================================================= */

	struct QueueFamilyIndices {
		std::optional<uint32_t>	graphics_family;
		std::optional<uint32_t>	present_family;

		bool	isComplete() {
			return graphics_family.has_value() && present_family.has_value();
		}
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR		capabilities;
		std::vector<VkSurfaceFormatKHR>	formats;
		std::vector<VkPresentModeKHR>	present_modes;
	};

	/* ========================================================================= */
	/*                               MAIN FUNCTION                               */
	/* ========================================================================= */

	void	run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private:
	/* ========================================================================= */
	/*                               CONST MEMBERS                               */
	/* ========================================================================= */

	const uint32_t					width = 800;
	const uint32_t					height = 600;
	const std::vector<const char*>	validation_layers = {
		"VK_LAYER_KHRONOS_validation"
	};
	const std::vector<const char*>	device_extensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	const int						max_frames_in_flight = 2;
	const std::vector<Vertex>		vertices = {
		// pos{},			 color{}
		{{  0.0f, -0.5f }, { 0.9f, 0.4f, 0.0f }},
		{{  0.5f,  0.5f }, { 0.0f, 0.8f, 0.2f }},
		{{ -0.5f,  0.5f }, { 0.0f, 0.1f, 0.9f }}
	};

	#ifndef NDEBUG
	const bool						enable_validation_layers = false;
	#else
	const bool						enable_validation_layers = true;
	#endif

	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	GLFWwindow*						window;
	VkInstance						vk_instance;
	VkDebugUtilsMessengerEXT		debug_messenger;
	VkPhysicalDevice				physical_device = VK_NULL_HANDLE;
	VkDevice						logical_device;

	VkSurfaceKHR					vk_surface;
	VkQueue							graphics_queue;
	VkQueue							present_queue;

	VkSwapchainKHR					swap_chain;
	std::vector<VkImage>			swap_chain_images;
	VkFormat						swap_chain_image_format;
	VkExtent2D						swap_chain_extent;
	std::vector<VkImageView>		swap_chain_image_views;

	VkRenderPass					render_pass;
	VkPipelineLayout				pipeline_layout;
	VkPipeline						graphics_pipeline;
	std::vector<VkFramebuffer>		swap_chain_frame_buffers;

	VkCommandPool					command_pool;
	std::vector<VkCommandBuffer>	command_buffers;

	std::vector<VkSemaphore>		image_available_semaphores;
	std::vector<VkSemaphore>		render_finished_semaphores;
	std::vector<VkFence>			in_flight_fences;
	bool							frame_buffer_resized = false;

	VkBuffer						vertex_buffer;
	VkDeviceMemory					vertex_buffer_memory;

	uint32_t						current_frame = 0;

	/* ========================================================================= */
	/*                                 CORE SETUP                                */
	/* ========================================================================= */

	void	initWindow() {
		// initialize glfw
		glfwInit();

		// disable OpenGL context creation
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		// create a window pointer
		window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);

		// set pointer to window to `this` instance pointer
		glfwSetWindowUserPointer(window, this);

		// handle resizing
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

	static void	framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		(void)width;
		(void)height;
		auto	app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
		app->frame_buffer_resized = true;
	}

	void	initVulkan() {
		createInstance();
		setupDebugMessenger();
		createSurface();
		pickPhysicalDevice();
		createLogicalDevice();
		createSwapChain();
		createImageViews();
		createRenderPass();
		createGraphicsPipeline();
		createFrameBuffers();
		createCommandPool();
		createVertexBuffer();
		createCommandBuffers();
		createSyncObjects();
	}

	void	mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			// Poll events while not pressing x...
			glfwPollEvents();
			drawFrame();
		}

		// Wait for logical device to finish executing everything before closing
		vkDeviceWaitIdle(logical_device);
	}

	void	cleanupSwapChain() {
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

	void	cleanup() {
		cleanupSwapChain();

		// Remove vertex buffer
		vkDestroyBuffer(logical_device, vertex_buffer, nullptr);
		vkFreeMemory(logical_device, vertex_buffer_memory, nullptr);

		// Remove pipeline
		vkDestroyPipeline(logical_device, graphics_pipeline, nullptr);

		// Remove pipeline layout
		vkDestroyPipelineLayout(logical_device, pipeline_layout, nullptr);

		// Remove render pass
		vkDestroyRenderPass(logical_device, render_pass, nullptr);

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

		// Remove vk surface
		vkDestroySurfaceKHR(vk_instance, vk_surface, nullptr);

		// Remove vk instance
		vkDestroyInstance(vk_instance, nullptr);

		// Remove window instance
		glfwDestroyWindow(window);

		// Remove glfw instance
		glfwTerminate();
	}

	void	createInstance() {
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

	void	setupDebugMessenger() {
		if (!enable_validation_layers) return;

		VkDebugUtilsMessengerCreateInfoEXT	create_info{};
		populateDebugMessengerCreateInfo(create_info);

		if (CreateDebugUtilsMessengerEXT(vk_instance, &create_info, nullptr, &debug_messenger) != VK_SUCCESS) {
			throw std::runtime_error("failed to setup debug messenger");
		}
	}

	void	populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& create_info) {
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

	bool	checkValidationLayerSupport() {
		uint32_t						layer_count;
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

	void	pickPhysicalDevice() {
		// Select best suited GPU device
		uint32_t	device_count = 0;
		vkEnumeratePhysicalDevices(vk_instance, &device_count, nullptr);

		if (!device_count)
			throw std::runtime_error("failed to find GPUs with vulkan support");

		std::vector<VkPhysicalDevice>	devices(device_count);
		vkEnumeratePhysicalDevices(vk_instance, &device_count, devices.data());

		for (const VkPhysicalDevice& device: devices) {
			if (isDeviceSuitable(device)) {
				physical_device = device;
				break;
			}
		}

		if (physical_device == VK_NULL_HANDLE)
			throw std::runtime_error("failed to find suitable GPU");
	}

	bool	isDeviceSuitable(const VkPhysicalDevice& device) {
		// Select the best GPU.
		QueueFamilyIndices	indices = findQueueFamilies(device);

		bool	extensions_supported = checkDeviceExtensionSupport(device);

		bool	swap_chain_adequate = false;
		if (extensions_supported) {
			SwapChainSupportDetails	swap_chain_support = querySwapChainSupport(device);
			swap_chain_adequate =
				!swap_chain_support.formats.empty()
				&& !swap_chain_support.present_modes.empty();
		}

		return indices.isComplete() && extensions_supported && swap_chain_adequate;
	}

	bool	checkDeviceExtensionSupport(const VkPhysicalDevice& device) {
		// Verify that every device_extensions are available
		uint32_t	extension_count;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

		std::vector<VkExtensionProperties>	available_extensions(extension_count);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

		std::set<std::string>	required_extensions(device_extensions.begin(), device_extensions.end());
		for (const auto& extension: available_extensions) {
			required_extensions.erase(extension.extensionName);
		}

		return required_extensions.empty();
	}

	QueueFamilyIndices	findQueueFamilies(const VkPhysicalDevice& device) {
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

	void	createLogicalDevice() {
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

		// Logical device creation info
		VkPhysicalDeviceFeatures	device_features{};
		VkDeviceCreateInfo			create_info{};

		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		// Passing queue families
		create_info.pQueueCreateInfos = queue_create_infos.data();
		create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
		create_info.pEnabledFeatures = &device_features;

		// Validation layers
		// Deprecated, but set for compatibility with older Vulkan implementations
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

	void	createSurface() {
		if (glfwCreateWindowSurface(vk_instance, window, nullptr, &vk_surface) != VK_SUCCESS)
			throw std::runtime_error("failed to create window surface");
	}

	SwapChainSupportDetails	querySwapChainSupport(const VkPhysicalDevice& device) {
		// Make sure the swap chain support is available for this device
		SwapChainSupportDetails	details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vk_surface, &details.capabilities);

		// Query supported surface formats
		uint32_t	format_count;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, vk_surface, &format_count, nullptr);

		if (format_count != 0) {
			details.formats.resize(format_count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, vk_surface, &format_count, details.formats.data());
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

	VkSurfaceFormatKHR	chooseSwapSurfaceFormat(
		const std::vector<VkSurfaceFormatKHR>& available_formats
	) {
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

	VkPresentModeKHR	chooseSwapPresentMode(
		const std::vector<VkPresentModeKHR>& available_present_modes
	) {
		// Prevalent: fifo mailbox mode, expansive on energy but efficient
		for (const auto& available_present_mode: available_present_modes) {
			if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return available_present_mode;
			}
		}

		// Default case, doesn't strain on battery
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D	chooseSwapExtent(
		const VkSurfaceCapabilitiesKHR& capabilities
	) {
		// Pick swap extent (~ resolution of the window, in px)
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		} else {
			int	width, height;
			glfwGetFramebufferSize(window, &width, &height);

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

	void	createSwapChain() {
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

	void	createImageViews() {
		// Create image view for each images
		swap_chain_image_views.resize(swap_chain_images.size());

		for (size_t i = 0; i < swap_chain_images.size(); ++i) {
			VkImageViewCreateInfo	create_info{};
			create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			create_info.image = swap_chain_images[i];
			create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			create_info.format = swap_chain_image_format;
			create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			create_info.subresourceRange.baseMipLevel = 0;
			create_info.subresourceRange.levelCount = 1;
			create_info.subresourceRange.baseArrayLayer = 0;
			create_info.subresourceRange.layerCount = 1;

			if (vkCreateImageView(logical_device, &create_info, nullptr, &swap_chain_image_views[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create image views");
			}
		}
	}

	void	createRenderPass() {
		// For a single framebuffer
		VkAttachmentDescription	color_attachment{};
		color_attachment.format = swap_chain_image_format;
		color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		// Create single subpass of render pass
		VkAttachmentReference	color_attachment_ref{};
		color_attachment_ref.attachment = 0;
		color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription	subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment_ref;

		// Create render pass
		VkRenderPassCreateInfo	create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		create_info.attachmentCount = 1;
		create_info.pAttachments = &color_attachment;
		create_info.subpassCount = 1;
		create_info.pSubpasses = &subpass;

		// Subpass dependency
		VkSubpassDependency	dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		create_info.dependencyCount = 1;
		create_info.pDependencies = &dependency;

		if (vkCreateRenderPass(logical_device, &create_info, nullptr, &render_pass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass");
		}
	}

	void	createGraphicsPipeline() {
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
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp = 0.0f;
		rasterizer.depthBiasSlopeFactor = 0.0f;

		// Disable multisampling
		VkPipelineMultisampleStateCreateInfo	multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
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
		pipeline_layout_info.setLayoutCount = 0;
		pipeline_layout_info.pSetLayouts = nullptr;
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
		pipeline_info.pDepthStencilState = nullptr;
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

	static std::vector<char>	readFile(const std::string& filename) {
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

	VkShaderModule	createShaderModule(const std::vector<char>& code) {
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

	void	createFrameBuffers() {
		swap_chain_frame_buffers.resize(swap_chain_image_views.size());
		for (size_t i = 0; i < swap_chain_image_views.size(); ++i) {
			VkImageView	attachments[] = {
				swap_chain_image_views[i]
			};

			// Create frame buffer from image view, associate with a rend pass
			VkFramebufferCreateInfo	create_info{};
			create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			create_info.renderPass = render_pass;
			create_info.attachmentCount = 1;
			create_info.pAttachments = attachments;
			create_info.width = swap_chain_extent.width;
			create_info.height = swap_chain_extent.height;
			create_info.layers = 1;

			if (vkCreateFramebuffer(logical_device, &create_info, nullptr,
			&swap_chain_frame_buffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create frame buffer");
			}
		}
	}

	void	createCommandPool() {
		// Command buffers handler
		QueueFamilyIndices	queue_family_indices = findQueueFamilies(physical_device);

		VkCommandPoolCreateInfo	pool_info{};
		pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		pool_info.queueFamilyIndex = queue_family_indices.graphics_family.value();

		if (vkCreateCommandPool(logical_device, &pool_info, nullptr, &command_pool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create command pool");
		}
	}

	void	createCommandBuffers() {
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

	void	recordCommandBuffer(VkCommandBuffer command_buffer, uint32_t image_index) {
		// Write commands to command buffer
		VkCommandBufferBeginInfo	begin_info{};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = 0;
		begin_info.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer");
		}

		// Begin render pass to start drawing
		VkRenderPassBeginInfo	render_pass_info{};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_info.renderPass = render_pass;
		render_pass_info.framebuffer = swap_chain_frame_buffers[image_index];
		render_pass_info.renderArea.offset = {0, 0};
		render_pass_info.renderArea.extent = swap_chain_extent;
		// Define what corresponds to 'clear color'
		VkClearValue	clear_color = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };
		render_pass_info.clearValueCount = 1;
		render_pass_info.pClearValues = &clear_color;

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

		// Bind vertex buffer
		VkBuffer		vertex_buffers[] = { vertex_buffer };
		VkDeviceSize	offsets[] = { 0 };
		vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);

		// Issue draw command for triangle
		vkCmdDraw(command_buffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);

		// End render pass
		vkCmdEndRenderPass(command_buffer);

		// Finish recording
		if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer");
		}
	}

	void	drawFrame() {
		// Wait fence available, lock it
		vkWaitForFences(logical_device, 1, &in_flight_fences[current_frame], VK_TRUE, UINT64_MAX);

		// Retrieve available image
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

		// Set synchronization
		VkSubmitInfo	submit_info{};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore	wait_semaphore[] = { image_available_semaphores[current_frame] };
		VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = wait_semaphore;
		submit_info.pWaitDstStageMask = wait_stages;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffers[current_frame];

		VkSemaphore	signal_semaphores[] = { render_finished_semaphores[current_frame] };

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

		current_frame = (current_frame + 1) % max_frames_in_flight;
	}

	void	createSyncObjects() {
		// Create semaphores and fence
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

	std::vector<const char*>	getRequiredExtensions() {
		// Retrieve list of extensions if validation layers enabled
		uint32_t		glfw_extension_count = 0;
		const char**	glfw_extensions;
		glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

		std::vector<const char*>	extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

		if (enable_validation_layers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
		return extensions;
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL	debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
		VkDebugUtilsMessageTypeFlagsEXT message_type,
		const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
		void* p_user_data
	) {
		std::cerr << "[validation layer] " << p_callback_data->pMessage << std::endl;
		return VK_FALSE;
	}

	void	recreateSwapChain() {
		// Handle window minimization
		int width = 0, height = 0;

		// "Pausing" frame refreshing until window is in foreground
		glfwGetFramebufferSize(window, &width, &height);
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(window, &width, &height);
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(logical_device);
		cleanupSwapChain();
		createSwapChain();
		createImageViews();
		createFrameBuffers();
	}

	void	createVertexBuffer() {
		VkDeviceSize	buffer_size = sizeof(vertices[0]) * vertices.size();

		// Create staging buffer to upload cpu memory to
		VkBuffer		staging_buffer;
		VkDeviceMemory	staging_buffer_memory;

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

	void	createBuffer(
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

	void	copyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size) {
		// Allocate temporary command buffer for memory transfer
		VkCommandBufferAllocateInfo	alloc_info{};
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		alloc_info.commandPool = command_pool;
		alloc_info.commandBufferCount = 1;

		VkCommandBuffer	command_buffer;
		vkAllocateCommandBuffers(logical_device, &alloc_info, &command_buffer);

		// Record commands
		VkCommandBufferBeginInfo	begin_info{};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(command_buffer, &begin_info);

		VkBufferCopy	copy_region{};
		copy_region.srcOffset = 0;
		copy_region.dstOffset = 0;
		copy_region.size = size;
		vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

		vkEndCommandBuffer(command_buffer);

		// Submit to graphics queue to execute transfer
		VkSubmitInfo	submit_info{};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;
		
		vkQueueSubmit(graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphics_queue);

		// Deallocate command buffer
		vkFreeCommandBuffers(logical_device, command_pool, 1, &command_buffer);
	}

	uint32_t	findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties) {
		// Map memory and find one suitable with filter and properties
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

}; // class App

#endif