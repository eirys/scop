/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   graphics_pipeline.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 16:34:03 by etran             #+#    #+#             */
/*   Updated: 2023/05/18 15:55:38 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "graphics_pipeline.hpp"
#include "window.hpp"
#include "utils.hpp"

#include <iostream> // std::cerr std::endl
#include <cstring> // std::strcmp
#include <set> // std::set

namespace scop {
namespace graphics {

const std::vector<const char*>	GraphicsPipeline::validation_layers = {
	"VK_LAYER_KHRONOS_validation"
};

/* ========================================================================== */
/*                                   PUBLIC                                   */
/* ========================================================================== */

void	GraphicsPipeline::init(
	scop::Window& window,
	const scop::Image& image,
	const std::vector<Vertex>& vertices,
	const std::vector<uint32_t>& indices
) {
	createInstance();
	LOG("Passed instance ");
	debug_module.init(vk_instance);
	LOG("Passed debug module ");
	device.init(window, vk_instance);
	LOG("Passed device ");
	render_target.init(device, window);
	LOG("Passed render target ");
	descriptor_set.initLayout(device);
	LOG("Passed descriptor layout");
	createGraphicsPipeline();
	LOG("Passed graphics pipeline ");
	command_buffer.initPool(device);
	LOG("Passed command pool ");
	texture_sampler.init(device, command_buffer.vk_command_pool, image);
	LOG("Passed texture sampler ");
	vertex_input.init(device, command_buffer.vk_command_pool, vertices, indices);
	LOG("Passed vertex input ");
	descriptor_set.initSets(device, texture_sampler);
	LOG("Passed descriptor set ");
	command_buffer.initBuffer(device);
	LOG("Passed command buffer ");
	createSyncObjects();
	LOG("Passed sync objects");
}

void	GraphicsPipeline::destroy() {
	render_target.destroy(device);

	texture_sampler.destroy(device);

	// Remove graphics pipeline
	vkDestroyPipeline(device.logical_device, graphics_pipeline, nullptr);
	vkDestroyPipelineLayout(device.logical_device, pipeline_layout, nullptr);

	descriptor_set.destroy(device);
	vertex_input.destroy(device);

	// Remove sync objects
	vkDestroySemaphore(device.logical_device, image_available_semaphores, nullptr);
	vkDestroySemaphore(device.logical_device, render_finished_semaphores, nullptr);
	vkDestroyFence(device.logical_device, in_flight_fences, nullptr);

	command_buffer.destroy(device);
	device.destroy(vk_instance);
	debug_module.destroy(vk_instance);

	// Remove instance
	vkDestroyInstance(vk_instance, nullptr);
}

/* ========================================================================== */

void	GraphicsPipeline::idle() {
	device.idle();
}

void	GraphicsPipeline::render(
	scop::Window& window,
	size_t indices_size
) {
	// Wait fence available, lock it
	vkWaitForFences(device.logical_device, 1, &in_flight_fences, VK_TRUE, UINT64_MAX);

	// Next available image from swap chain
	uint32_t	image_index;
	VkResult	result = vkAcquireNextImageKHR(
		device.logical_device,
		render_target.vk_swap_chain,
		UINT64_MAX,
		image_available_semaphores,
		VK_NULL_HANDLE,
		&image_index
	);
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		// Swap chain incompatible for rendering (resize?)
		// recreateSwapChain();
		render_target.updateSwapChain(device, window);
		return;
	} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image");
	}

	// Work is done, unlock fence
	vkResetFences(device.logical_device, 1, &in_flight_fences);

	// Record buffer
	vkResetCommandBuffer(command_buffer.command_buffers, 0);

	recordCommandBuffer(
		indices_size,
		command_buffer.command_buffers, 
		image_index
	);

	descriptor_set.updateUniformBuffer(render_target.swap_chain_extent);

	// Set synchronization objects
	VkSemaphore				wait_semaphore[] = {
		image_available_semaphores
	};
	VkSemaphore				signal_semaphores[] = {
		render_finished_semaphores
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
	submit_info.pCommandBuffers = &command_buffer.command_buffers;
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphores;

	// Submit command buffer to be processed by graphics queue
	if (vkQueueSubmit(device.graphics_queue, 1, &submit_info, in_flight_fences) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer");
	}

	// Set presentation for next swap chain image
	VkSwapchainKHR	swap_chains[] = { render_target.vk_swap_chain };
	VkPresentInfoKHR	present_info{};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = signal_semaphores;
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swap_chains;
	present_info.pImageIndices = &image_index;
	present_info.pResults = nullptr;

	// Submit to swap chain, check if swap chain is still compatible
	result = vkQueuePresentKHR(device.present_queue, &present_info);

	if (
		result == VK_ERROR_OUT_OF_DATE_KHR ||
		result == VK_SUBOPTIMAL_KHR ||
		window.resized()
	) {
		window.toggleFrameBufferResized(false);
		// recreateSwapChain();
		render_target.updateSwapChain(device, window);
	} else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swapchain image");
	}
}

/* ========================================================================== */
/*                                   PRIVATE                                  */
/* ========================================================================== */

/* INIT ===================================================================== */

/**
 * Create a Vulkan instance
*/
void	GraphicsPipeline::createInstance() {
	// Check if validation layers are available
	if (enable_validation_layers && !checkValidationLayerSupport())
		throw std::runtime_error("validation layers requested but not availalbe");

	// Provides information to driver
	VkApplicationInfo	app_info{};

	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "Ft_Vox";
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
		debug_module.populate(debug_create_info);
		create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;
	} else {
		create_info.enabledLayerCount = 0;
		create_info.pNext = nullptr;
	}

	// Create the instance
	if (vkCreateInstance(&create_info, nullptr, &vk_instance) != VK_SUCCESS)
		throw std::runtime_error("failed to create vk_instance");
}

void	GraphicsPipeline::createGraphicsPipeline() {
	std::vector<char>	vert_shader_code = scop::utils::readFile(vertex_shader_bin);
	std::vector<char>	frag_shader_code = scop::utils::readFile(fragment_shader_bin);

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
	auto	binding_description = scop::Vertex::getBindingDescription();
	auto	attribute_descriptions = scop::Vertex::getAttributeDescriptions();

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

	// Multisampling
	VkPipelineMultisampleStateCreateInfo	multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = device.msaa_samples;
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

	// Push constants setup
	// VkPushConstantRange	push_constant_range{};
	// push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	// push_constant_range.offset = 0;
	// push_constant_range.size = sizeof(PushConstantData);

	// Pipeline layout setups

	VkPipelineLayoutCreateInfo	pipeline_layout_info{};
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_info.setLayoutCount = 1;
	pipeline_layout_info.pSetLayouts = &descriptor_set.vk_descriptor_set_layout;
	pipeline_layout_info.pushConstantRangeCount = 0;
	pipeline_layout_info.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(device.logical_device, &pipeline_layout_info, nullptr, &pipeline_layout) != VK_SUCCESS) {
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
	pipeline_info.renderPass = render_target.vk_render_pass;
	pipeline_info.subpass = 0;
	pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
	pipeline_info.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(device.logical_device, VK_NULL_HANDLE, 1, &pipeline_info,
	nullptr, &graphics_pipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline");
	}

	vkDestroyShaderModule(device.logical_device, frag_shader_module, nullptr);
	vkDestroyShaderModule(device.logical_device, vert_shader_module, nullptr);
}

/**
 * Create semaphores and fences
*/
void	GraphicsPipeline::createSyncObjects() {
	VkSemaphoreCreateInfo	semaphore_info{};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo	fence_info{};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	if (vkCreateSemaphore(device.logical_device, &semaphore_info, nullptr, &image_available_semaphores) != VK_SUCCESS ||
		vkCreateSemaphore(device.logical_device, &semaphore_info, nullptr, &render_finished_semaphores) != VK_SUCCESS ||
		vkCreateFence(device.logical_device, &fence_info, nullptr, &in_flight_fences) != VK_SUCCESS) {
		throw std::runtime_error("failed to create semaphore");
	}
}

/* ========================================================================== */

/**
 * Check if all required extensions are available for validation layers
*/
bool	GraphicsPipeline::checkValidationLayerSupport() {
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
 *  Retrieve list of extensions if validation layers enabled
 */
std::vector<const char*>	GraphicsPipeline::getRequiredExtensions() {
	uint32_t		glfw_extension_count = 0;
	const char**	glfw_extensions;
	glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

	std::vector<const char*>	extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

	if (enable_validation_layers) {
		extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	return extensions;
}

/**
 * Create a shader module, from a GLSL shader file, that will be used in the pipeline
*/
VkShaderModule	GraphicsPipeline::createShaderModule(const std::vector<char>& code) {
	// Create a shader module from code
	VkShaderModuleCreateInfo	create_info{};

	create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.codeSize = code.size();
	create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule	shader_module;
	if (vkCreateShaderModule(device.logical_device, &create_info, nullptr, &shader_module) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module");
	}
	return shader_module;
}

/**
 *  Write commands to command buffer to be subimtted to queue.
 */
void	GraphicsPipeline::recordCommandBuffer(
	size_t indices_size,
	VkCommandBuffer command_buffer,
	uint32_t image_index
) {
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
	render_pass_info.renderPass = render_target.vk_render_pass;
	render_pass_info.framebuffer = render_target.swap_chain_frame_buffers[image_index];
	render_pass_info.renderArea.offset = { 0, 0 };
	render_pass_info.renderArea.extent = render_target.swap_chain_extent;
	render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
	render_pass_info.pClearValues = clear_values.data();

	// Begin rp and bind pipeline
	vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);

	// Set viewport and scissors
	VkViewport	viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(render_target.swap_chain_extent.width);
	viewport.height = static_cast<float>(render_target.swap_chain_extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(command_buffer, 0, 1, &viewport);

	VkRect2D	scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = render_target.swap_chain_extent;
	vkCmdSetScissor(command_buffer, 0, 1, &scissor);

	// Bind vertex buffer && index buffer
	VkBuffer		vertex_buffers[] = { vertex_input.vertex_buffer };
	VkDeviceSize	offsets[] = { 0 };
	vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);
	vkCmdBindIndexBuffer(command_buffer, vertex_input.index_buffer, 0, VK_INDEX_TYPE_UINT32);

	// Bind descriptor sets
	vkCmdBindDescriptorSets(
		command_buffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipeline_layout,
		0,
		1,
		&descriptor_set.vk_descriptor_sets,
		0,
		nullptr
	);

	// Issue draw command
	vkCmdDrawIndexed(command_buffer, static_cast<uint32_t>(indices_size), 1, 0, 0, 0);

	// Stop the render target work
	vkCmdEndRenderPass(command_buffer);

	if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer");
	}
}

/* ========================================================================== */
/*                                    OTHER                                   */
/* ========================================================================== */

VkCommandBuffer	beginSingleTimeCommands(
	VkDevice device,
	VkCommandPool command_pool
) {
	// Allocate temporary command buffer for memory transfer
	VkCommandBufferAllocateInfo	alloc_info{};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandPool = command_pool;
	alloc_info.commandBufferCount = 1;

	VkCommandBuffer	buffer;
	vkAllocateCommandBuffers(device, &alloc_info, &buffer);

	VkCommandBufferBeginInfo	begin_info{};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(buffer, &begin_info);

	return buffer;
}

void	endSingleTimeCommands(
	VkDevice device,
	VkQueue queue,
	VkCommandPool command_pool,
	VkCommandBuffer buffer
) {
	// Submit to graphics queue to execute transfer
	vkEndCommandBuffer(buffer);

	VkSubmitInfo	submit_info{};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &buffer;

	vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);

	// Deallocate temporary command buffer
	vkFreeCommandBuffers(
		device,
		command_pool,
		1,
		&buffer
	);
}

VkImageView	createImageView(
	VkDevice logical_device,
	VkImage image,
	VkFormat format,
	VkImageAspectFlags aspect_flags,
	uint32_t mip_level
) {
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

/**
 * Record commands to copy data from one buffer to another,
 * and submit them to the graphics queue.
*/
void	copyBuffer(
	VkDevice device,
	VkQueue queue,
	VkCommandPool command_pool,
	VkBuffer src_buffer,
	VkBuffer dst_buffer,
	VkDeviceSize size
) {
	VkCommandBuffer	command_buffer = beginSingleTimeCommands(
		device,
		command_pool
	);

	VkBufferCopy	copy_region{};
	copy_region.srcOffset = 0;
	copy_region.dstOffset = 0;
	copy_region.size = size;
	vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

	endSingleTimeCommands(device, queue, command_pool, command_buffer);
}

void	copyBufferToImage(
	VkDevice device,
	VkQueue queue,
	VkCommandPool command_pool,
	VkBuffer src_buffer,
	VkImage image,
	uint32_t width,
	uint32_t height
) {
	VkCommandBuffer	command_buffer = beginSingleTimeCommands(
		device,
		command_pool
	);

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
		src_buffer,
		image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);

	endSingleTimeCommands(device, queue, command_pool, command_buffer);
}

} // namespace graphics
} // namespace scop