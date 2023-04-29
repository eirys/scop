/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   app.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/10 18:21:34 by eli               #+#    #+#             */
/*   Updated: 2023/04/29 20:13:47 by eli              ###   ########.fr       */
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
# include "ubo.hpp"

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
);

void	DestroyDebugUtilsMessengerEXT(
	VkInstance instance,
	VkDebugUtilsMessengerEXT debug_messenger,
	const VkAllocationCallbacks* p_allocator
);

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

	void	run();

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
		{{ -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }},
		{{  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }},
		{{  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }},
		{{ -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f }}
	};
	const std::vector<uint16_t>		indices = {
		0, 1, 2, 2, 3, 0
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
	VkBuffer						index_buffer;
	VkDeviceMemory					index_buffer_memory;

	uint32_t						current_frame = 0;

	/* ========================================================================= */
	/*                                 CORE SETUP                                */
	/* ========================================================================= */

	void									initWindow();
	static void								framebufferResizeCallback(
		GLFWwindow* window,
		int width,
		int height
	);
	void									initVulkan();
	void									mainLoop();

	void									cleanupSwapChain();
	void									cleanup();

	void									createInstance();

	void									setupDebugMessenger();

	void									populateDebugMessengerCreateInfo(
		VkDebugUtilsMessengerCreateInfoEXT& create_info
	);
	bool									checkValidationLayerSupport();
	void									pickPhysicalDevice();
	bool									isDeviceSuitable(
		const VkPhysicalDevice& device
	);
	bool									checkDeviceExtensionSupport(
		const VkPhysicalDevice& device
	);
	QueueFamilyIndices						findQueueFamilies(
		const VkPhysicalDevice& device
	);
	void									createLogicalDevice();
	void									createSurface();
	SwapChainSupportDetails					querySwapChainSupport(
		const VkPhysicalDevice& device
	) const;
	VkSurfaceFormatKHR						chooseSwapSurfaceFormat(
		const std::vector<VkSurfaceFormatKHR>& available_formats
	) const;
	VkPresentModeKHR						chooseSwapPresentMode(
		const std::vector<VkPresentModeKHR>& available_present_modes
	) const;
	VkExtent2D								chooseSwapExtent(
		const VkSurfaceCapabilitiesKHR& capabilities
	);
	void									createSwapChain();
	void									createImageViews();
	void									createRenderPass();
	void									createGraphicsPipeline();
	static std::vector<char>				readFile(const std::string& filename);
	VkShaderModule							createShaderModule(
		const std::vector<char>& code
	);
	void									createFrameBuffers();
	void									createCommandPool();
	void									createCommandBuffers();
	void									recordCommandBuffer(
		VkCommandBuffer command_buffer,
		uint32_t image_index
	);
	void									drawFrame();
	void									createSyncObjects();
	std::vector<const char*>				getRequiredExtensions();
	static VKAPI_ATTR VkBool32 VKAPI_CALL	debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
		VkDebugUtilsMessageTypeFlagsEXT message_type,
		const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
		void* p_user_data
	);
	void									recreateSwapChain();
	void									createVertexBuffer();
	void									createIndexBuffer();
	void									createBuffer(
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkBuffer& buffer,
		VkDeviceMemory& buffer_memory
	) const;
	void									copyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size) const;
	uint32_t								findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties) const;

}; // class App

#endif