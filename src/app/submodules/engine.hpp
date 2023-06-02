/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   engine.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 16:27:44 by etran             #+#    #+#             */
/*   Updated: 2023/06/02 17:06:48 by etran            ###   ########.fr       */
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

# include "debug_module.hpp"
# include "device.hpp"
# include "render_target.hpp"
# include "texture_sampler.hpp"
# include "descriptor_set.hpp"
# include "command_buffer.hpp"
# include "vertex_input.hpp"

namespace scop {
namespace graphics {

struct QueueFamilyIndices {
	std::optional<uint32_t>	graphics_family;
	std::optional<uint32_t>	present_family;

	bool	isComplete() {
		return graphics_family.has_value() && present_family.has_value();
	}
};

class Engine {
public:
	/* ========================================================================= */
	/*                               CONST MEMBERS                               */
	/* ========================================================================= */

	static const std::vector<const char*>	validation_layers;
	static constexpr std::size_t			max_frames_in_flight = 1;

	#ifndef NDEBUG
	static constexpr bool					enable_validation_layers = false;
	#else
	static constexpr bool					enable_validation_layers = true;
	#endif

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	Engine() = default;
	Engine(Engine&& other) = default;
	~Engine() = default;

	Engine(const Engine& other) = delete;
	Engine& operator=(const Engine& other) = delete;

	/* ========================================================================= */

	void						init(
		scop::Window& window,
		const scop::Image& image,
		const UniformBufferObject::Light& light,
		const std::vector<Vertex>& vertices,
		const std::vector<uint32_t>& indices
	);
	void						destroy();

	void						idle();
	void						render(
		scop::Window& window,
		std::size_t indices_size
	);

private:
	/* ========================================================================= */
	/*                               HELPER OBJECTS                              */
	/* ========================================================================= */

	struct PushConstantData {
		scop::Vect2		offset;
		scop::Vect3		color;
	};

	/* ========================================================================= */
	/*                               CONST MEMBERS                               */
	/* ========================================================================= */

	static constexpr const char*	vertex_shader_bin = "shaders/vert.spv";
	static constexpr const char*	fragment_shader_bin = "shaders/frag.spv";

	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	VkInstance						vk_instance;
	DebugModule						debug_module;

	Device							device;
	RenderTarget					render_target;
	TextureSampler					texture_sampler;
	DescriptorSet					descriptor_set;
	CommandBuffer					command_buffer;
	VertexInput						vertex_input;

	VkSemaphore						image_available_semaphores;
	VkSemaphore						render_finished_semaphores;
	VkFence							in_flight_fences;

	VkPipelineLayout				pipeline_layout;
	VkPipeline						engine;

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	/* INIT ==================================================================== */

	void							createInstance();
	void							createGraphicsPipeline();
	void							createSyncObjects();

	bool							checkValidationLayerSupport();
	std::vector<const char*>		getRequiredExtensions();
	VkShaderModule					createShaderModule(
		const std::vector<char>& code
	);
	void							recordCommandBuffer(
		std::size_t indices_size,
		VkCommandBuffer command_buffer,
		uint32_t image_index
	);

}; // class Engine

/* ========================================================================== */
/*                                    UTILS                                   */
/* ========================================================================== */

VkCommandBuffer	beginSingleTimeCommands(
	VkDevice device,
	VkCommandPool command_pool
);

void	endSingleTimeCommands(
	VkDevice device,
	VkQueue queue,
	VkCommandPool command_pool,
	VkCommandBuffer command_buffer
);

VkImageView	createImageView(
	VkDevice logical_device,
	VkImage image,
	VkFormat format,
	VkImageAspectFlags aspect_flags,
	uint32_t mip_level
);

void	copyBuffer(
	VkDevice device,
	VkQueue queue,
	VkCommandPool command_pool,
	VkBuffer src_buffer,
	VkBuffer dst_buffer,
	VkDeviceSize size
);

void	copyBufferToImage(
	VkDevice device,
	VkQueue queue,
	VkCommandPool command_pool,
	VkBuffer buffer,
	VkImage image,
	uint32_t width,
	uint32_t height
);

} // namespace graphics
} // namespace scop

