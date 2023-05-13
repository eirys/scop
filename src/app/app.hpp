/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   app.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/10 18:21:34 by eli               #+#    #+#             */
/*   Updated: 2023/05/13 02:40:37 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef APP_HPP
# define APP_HPP

// Graphics
# ifndef GLFW_INCLUDE_VULKAN
#  define GLFW_INCLUDE_VULKAN
# endif

// Window handler
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
# include <cassert>
# include <chrono>
# include <unordered_map>
# include <memory>

# include "window.hpp"
# include "utils.hpp"
# include "vertex.hpp"
# include "uniform_buffer_object.hpp"
# include "image_handler.hpp"
# include "ppm_loader.hpp"

# define SCOP_VERTEX_SHADER_BINARY		"shaders/vert.spv"
# define SCOP_FRAGMENT_SHADER_BINARY	"shaders/frag.spv"

# define SCOP_TEXTURE_FILE_HAMSTER_PPM	"textures/hammy.ppm"

namespace scop {

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

/* ========================================================================== */

class App {
public:
	/* ========================================================================= */
	/*                                  TYPEDEF                                  */
	/* ========================================================================= */

	typedef	std::chrono::high_resolution_clock::time_point	time_point;

	/* ========================================================================= */
	/*                               HELPER OBJECTS                              */
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

	struct PushConstantData {
		scop::Vect2		offset;
		scop::Vect3		color;
	};

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	App(const std::string& model_file, const std::string& texture_file);
	~App();

	App() = delete;
	App(const App& x) = delete;
	App(App&& x) = delete;
	App& operator=(const App& rhs) = delete;

	/* ========================================================================= */

	void			run();
	static void		toggleTexture();

private:
	/* ========================================================================= */
	/*                               CONST MEMBERS                               */
	/* ========================================================================= */

	const std::vector<const char*>	validation_layers = {
		"VK_LAYER_KHRONOS_validation"
	};
	const std::vector<const char*>	device_extensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	static constexpr size_t			max_frames_in_flight = 2;
	static constexpr float			transition_duration = 300.0f;	// ms

	#ifndef NDEBUG
	static constexpr bool			enable_validation_layers = false;
	#else
	static constexpr bool			enable_validation_layers = true;
	#endif

	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	std::unique_ptr<scop::ImageLoader>	image_loader;

	scop::Window					window;
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
	std::vector<VkFramebuffer>		swap_chain_frame_buffers;

	VkRenderPass					render_pass;

	VkPipelineLayout				pipeline_layout;
	VkPipeline						graphics_pipeline;

	VkCommandPool					command_pool;
	std::vector<VkCommandBuffer>	command_buffers;

	std::vector<VkSemaphore>		image_available_semaphores;
	std::vector<VkSemaphore>		render_finished_semaphores;
	std::vector<VkFence>			in_flight_fences;

	std::vector<scop::Vertex>		vertices;
	std::vector<uint32_t>			indices;

	VkBuffer						vertex_buffer;
	VkDeviceMemory					vertex_buffer_memory;
	VkBuffer						index_buffer;
	VkDeviceMemory					index_buffer_memory;

	VkDescriptorSetLayout			descriptor_set_layout;
	VkDescriptorPool				descriptor_pool;
	std::vector<VkDescriptorSet>	descriptor_sets;

	std::vector<VkBuffer>			uniform_buffers;
	std::vector<VkDeviceMemory>		uniform_buffers_memory;
	std::vector<void*>				uniform_buffers_mapped;

	uint32_t						mip_levels;
	VkImage							texture_image;
	VkDeviceMemory					texture_image_memory;
	VkImageView						texture_image_view;
	VkSampler 						texture_sampler;

	VkImage							depth_image;
	VkDeviceMemory					depth_image_memory;
	VkImageView						depth_image_view;

	VkSampleCountFlagBits			msaa_samples;
	VkImage							color_image;
	VkDeviceMemory					color_image_memory;
	VkImageView						color_image_view;

	static
	bool							texture_enabled;
	static
	std::optional<time_point>		texture_enabled_start;

	uint32_t						current_frame = 0;

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	void							cleanupSwapChain();
	void							createInstance();
	void							setupDebugMessenger();
	void							populateDebugMessengerCreateInfo(
		VkDebugUtilsMessengerCreateInfoEXT& create_info
	);
	bool							checkValidationLayerSupport();
	void							pickPhysicalDevice();
	bool							isDeviceSuitable(
		const VkPhysicalDevice& device
	);
	bool							checkDeviceExtensionSupport(
		const VkPhysicalDevice& device
	);
	QueueFamilyIndices				findQueueFamilies(
		const VkPhysicalDevice& device
	);
	void							createLogicalDevice();
	void							createSurface();
	SwapChainSupportDetails			querySwapChainSupport(
		const VkPhysicalDevice& device
	) const;
	VkSurfaceFormatKHR				chooseSwapSurfaceFormat(
		const std::vector<VkSurfaceFormatKHR>& available_formats
	) const;
	VkPresentModeKHR				chooseSwapPresentMode(
		const std::vector<VkPresentModeKHR>& available_present_modes
	) const;
	VkExtent2D						chooseSwapExtent(
		const VkSurfaceCapabilitiesKHR& capabilities
	);
	void							createSwapChain();
	void							createImageViews();
	VkImageView						createImageView(
		VkImage image,
		VkFormat format,
		VkImageAspectFlags aspect_flags,
		uint32_t mip_level
	) const;
	void							createRenderPass();
	void							createGraphicsPipeline();
	VkShaderModule					createShaderModule(
		const std::vector<char>& code
	);
	void							createFrameBuffers();
	void							createCommandPool();
	void							createCommandBuffers();
	void							recordCommandBuffer(
		VkCommandBuffer command_buffer,
		uint32_t image_index
	);
	void							drawFrame();
	void							createSyncObjects();
	std::vector<const char*>		getRequiredExtensions();

	void							recreateSwapChain();
	void							createVertexBuffer();
	void							createIndexBuffer();
	void							createBuffer(
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkBuffer& buffer,
		VkDeviceMemory& buffer_memory
	) const;
	void							copyBuffer(
		VkBuffer src_buffer,
		VkBuffer dst_buffer,
		VkDeviceSize size
	) const;
	uint32_t						findMemoryType(
		uint32_t type_filter,
		VkMemoryPropertyFlags properties
	) const;
	void							createDescriptorSetLayout();
	void							createUniformBuffers();
	void							updateUniformBuffer(
		uint32_t current_image
	);
	void							createDescriptorPool();
	void							createDescriptorSets();
	void							createTextureImage();
	void							createImage(
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
	) const;
	VkCommandBuffer					beginSingleTimeCommands() const;
	void							endSingleTimeCommands(
		VkCommandBuffer command_buffer
	) const;
	void							transitionImageLayout(
		VkImage image,
		VkFormat format,
		VkImageLayout old_layout,
		VkImageLayout new_layout,
		uint32_t mip_level
	) const;
	void							copyBufferToImage(
		VkBuffer buffer,
		VkImage image,
		uint32_t width,
		uint32_t height
	) const;
	void							createTextureImageView();
	void							createTextureSampler();
	void							createDepthResources();
	VkFormat						findSupportedFormat(
		const std::vector<VkFormat>& candidates,
		VkImageTiling tiling,
		VkFormatFeatureFlags features
	) const;
	VkFormat						findDepthFormat() const;
	bool							hasStencilCompotent(
		VkFormat format
	) const;
	void							loadModel(
		const std::string& path
	);
	void							generateMipmaps(
		VkImage image,
		VkFormat image_format,
		int32_t tex_width,
		int32_t tex_height,
		uint32_t mip_level
	) const;
	VkSampleCountFlagBits			getMaxUsableSampleCount() const;
	void							createColorResources();
	void							updateVertexPart(
		UniformBufferObject& ubo,
		time_point current_time
	);
	void							updateFragmentPart(
		UniformBufferObject& ubo,
		time_point current_time
	);
	void							createTextureLoader(
		const std::string& path
	);

	static VKAPI_ATTR
	VkBool32 VKAPI_CALL				debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
		VkDebugUtilsMessageTypeFlagsEXT message_type,
		const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
		void* p_user_data
	);

}; // class App

} // namespace scop

#endif