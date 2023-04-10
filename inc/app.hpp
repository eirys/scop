/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   app.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/10 18:21:34 by eli               #+#    #+#             */
/*   Updated: 2023/04/10 23:36:22 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef APP_HPP
# define APP_HPP

// Graphics
# define GLFW_INCLUDE_VULKAN
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

/**
 * Un/comment to toggle NDEBUG mode and enable validation layers.
*/
# define NDEBUG
# include <cassert>

# include "utils.hpp"

# define SCOP_VERTEX_SHADER_BINARY		"shaders/vert.spv"
# define SCOP_FRAGMENT_SHADER_BINARY	"shaders/frag.spv"

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

	#ifdef NDEBUG
	const bool						enable_validation_layers = false;
	#else
	const bool						enable_validation_layers = true;
	#endif

	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	GLFWwindow*						window;
	VkInstance						vk_instance;
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

	VkPipelineLayout				pipeline_layout;

	/* ========================================================================= */
	/*                                 CORE SETUP                                */
	/* ========================================================================= */

	void	initWindow() {
		// initialize glfw
		glfwInit();

		// disable OpenGL context creation
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		// disable resizing
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		// create a window pointer
		window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);
	}

	void	initVulkan() {
		createInstance();
		// setupDebugMessenger(); TODO
		createSurface();
		pickPhysicalDevice();
		createLogicalDevice();
		createSwapChain();
		createImageViews();
		createGraphicsPipeline();
	}

	void	mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			// Poll events while not pressing x...
			glfwPollEvents();
		}
	}

	void	cleanup() {
		vkDestroyPipelineLayout(logical_device, pipeline_layout, nullptr);

		// Destroy image view instances
		for (auto& image_view: swap_chain_image_views) {
			vkDestroyImageView(logical_device, image_view, nullptr);
		}

		// Remove swap chain handler
		vkDestroySwapchainKHR(logical_device, swap_chain, nullptr);

		// Remove device
		vkDestroyDevice(logical_device, nullptr);

		// Remove vk surface
		vkDestroySurfaceKHR(vk_instance, vk_surface, nullptr);

		// Remove vk instance
		vkDestroyInstance(vk_instance, nullptr);

		// Remove window instance
		glfwDestroyWindow(window);

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

		if (enable_validation_layers) {
			create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
			create_info.ppEnabledLayerNames = validation_layers.data();
		} else {
			create_info.enabledLayerCount = 0;
		}

		// Explicit which global extensions to use
		uint32_t		glfw_extension_count = 0;
		const char**	glfw_extensions;

		glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

		create_info.enabledExtensionCount = glfw_extension_count;
		create_info.ppEnabledExtensionNames = glfw_extensions;

		// Explicit which global validation layers to enable
		create_info.enabledLayerCount = 0;

		// Create the instance
		if (vkCreateInstance(&create_info, nullptr, &vk_instance) != VK_SUCCESS)
			throw std::runtime_error("failed to create vk_instance");

		// Checking for extension
		uint32_t							extension_count = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);

		// Contains extensions details
		std::vector<VkExtensionProperties>	extensions(extension_count);
		vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());

		// Display them
		std::cout	<< "Available extensions:\n";
		for (const auto& extension: extensions) {
			std::cout << '\t' << extension.extensionName;
			for (uint32_t i = 0; i < glfw_extension_count; ++i) {
				if (!strcmp(extension.extensionName, glfw_extensions[i])) {
					std::cout << " (available in GLFW extensions)";
					break;
				}
			}
			std::cout << NL;
		}
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
		vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertex_input_info.vertexBindingDescriptionCount = 0;
		vertex_input_info.pVertexBindingDescriptions = nullptr;
		vertex_input_info.vertexAttributeDescriptionCount = 0;
		vertex_input_info.pVertexAttributeDescriptions = nullptr;

		// Vertex input assembly descriptor: regular triangles here
		VkPipelineInputAssemblyStateCreateInfo	input_assembly_info{};
		input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		input_assembly_info.primitiveRestartEnable = VK_FALSE;

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

		// Enable dynamic states
		std::vector<VkDynamicState>	dynamic_states = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo	dynamic_state{};
		dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
		dynamic_state.pDynamicStates = dynamic_states.data();

		VkPipelineViewportStateCreateInfo	viewport_state{};
		viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewport_state.viewportCount = 1;
		viewport_state.scissorCount = 1;

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

};	// class App

#endif