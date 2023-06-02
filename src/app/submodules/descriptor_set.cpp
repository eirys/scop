/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   descriptor_set.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 20:56:05 by etran             #+#    #+#             */
/*   Updated: 2023/06/02 17:06:48 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "descriptor_set.hpp"
#include "uniform_buffer_object.hpp"
#include "app.hpp"
#include "math.hpp"

#include <array> // std::array
#include <stdexcept> // std::runtime_error
#include <chrono> // std::chrono
#include <optional> // std::optional

namespace scop {
namespace graphics {

/* ========================================================================== */
/*                                   PUBLIC                                   */
/* ========================================================================== */

void	DescriptorSet::initLayout(Device& device) {
	createDescriptorSetLayout(device);
}

void	DescriptorSet::initSets(
	Device& device,
	TextureSampler& texture_sampler,
	const UniformBufferObject::Light& light
) {
	uint32_t	frames_in_flight = static_cast<uint32_t>(
		Engine::max_frames_in_flight
	);
	createUniformBuffers(device);
	createDescriptorPool(device, frames_in_flight);
	createDescriptorSets(device, texture_sampler, frames_in_flight);

	initUniformBuffer(light);
}

void	DescriptorSet::destroy(
	Device& device
) {
	// Remove uniform buffers
	vkDestroyBuffer(device.logical_device, uniform_buffers, nullptr);
	vkFreeMemory(device.logical_device, uniform_buffers_memory, nullptr);

	// Remove descriptor pool
	vkDestroyDescriptorPool(device.logical_device, vk_descriptor_pool, nullptr);
	vkDestroyDescriptorSetLayout(
		device.logical_device,
		vk_descriptor_set_layout,
		nullptr
	);
}

/**
 * Update transformation of vertices
*/
void	DescriptorSet::updateUniformBuffer(VkExtent2D extent) {
	updateCamera(extent);
	updateTexture();
	updateLight();
}

/* ========================================================================== */
/*                                   PRIVATE                                  */
/* ========================================================================== */

/**
 * Descriptor set layout for uniform buffer and combined image sampler
*/
void	DescriptorSet::createDescriptorSetLayout(Device& device) {
	// Uniform buffer layout: used during vertex shading
	VkDescriptorSetLayoutBinding	camera_layout_binding{};
	camera_layout_binding.binding = 0;
	camera_layout_binding.descriptorCount = 1;
	camera_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	camera_layout_binding.pImmutableSamplers = nullptr;
	camera_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	// Sampler descriptor layout: used during fragment shading
	VkDescriptorSetLayoutBinding	sampler_layout_binding{};
	sampler_layout_binding.binding = 1;
	sampler_layout_binding.descriptorCount = 1;
	sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	sampler_layout_binding.pImmutableSamplers = nullptr;
	sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	// Uniform buffer layout: used during fragment shading
	VkDescriptorSetLayoutBinding	texture_layout_binding{};
	texture_layout_binding.binding = 2;
	texture_layout_binding.descriptorCount = 1;
	texture_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	texture_layout_binding.pImmutableSamplers = nullptr;
	texture_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	// Light layout
	VkDescriptorSetLayoutBinding	light_layout_binding{};
	light_layout_binding.binding = 3;
	light_layout_binding.descriptorCount = 1;
	light_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	light_layout_binding.pImmutableSamplers = nullptr;
	light_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 4>	bindings = {
		camera_layout_binding,
		sampler_layout_binding,
		texture_layout_binding,
		light_layout_binding
	};

	VkDescriptorSetLayoutCreateInfo	layout_info{};
	layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
	layout_info.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(device.logical_device, &layout_info, nullptr, &vk_descriptor_set_layout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout");
	}
}

/**
 * Handler for descriptor sets (like command pool)
*/
void	DescriptorSet::createDescriptorPool(Device& device, uint32_t frames_in_flight) {
	std::array<VkDescriptorPoolSize, 4>	pool_sizes{};
	pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pool_sizes[0].descriptorCount = frames_in_flight;
	pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	pool_sizes[1].descriptorCount = frames_in_flight;
	pool_sizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pool_sizes[2].descriptorCount = frames_in_flight;
	pool_sizes[3].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pool_sizes[3].descriptorCount = frames_in_flight;

	VkDescriptorPoolCreateInfo	pool_info{};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
	pool_info.pPoolSizes = pool_sizes.data();
	pool_info.maxSets = frames_in_flight;

	if (vkCreateDescriptorPool(device.logical_device, &pool_info, nullptr, &vk_descriptor_pool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool");
	}
}

void	DescriptorSet::createDescriptorSets(
	Device& device,
	TextureSampler& texture_sampler,
	uint32_t frames_in_flight
) {
	VkDescriptorSetAllocateInfo			alloc_info{};
	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.descriptorPool = vk_descriptor_pool;
	alloc_info.descriptorSetCount = frames_in_flight;
	alloc_info.pSetLayouts = &vk_descriptor_set_layout;

	if (vkAllocateDescriptorSets(device.logical_device, &alloc_info, &vk_descriptor_sets) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets");
	}

	// Ubo Camera
	VkDescriptorBufferInfo	ubo_info_camera{};
	ubo_info_camera.buffer = uniform_buffers;
	ubo_info_camera.offset = 0;
	ubo_info_camera.range = sizeof(UniformBufferObject::Camera);

	// Texture sampler
	VkDescriptorImageInfo	image_info{};
	image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	image_info.imageView = texture_sampler.vk_texture_image_view;
	image_info.sampler = texture_sampler.vk_texture_sampler;

	// Ubo Texture
	VkDescriptorBufferInfo	ubo_info_texture{};
	ubo_info_texture.buffer = uniform_buffers;
	ubo_info_texture.offset = offsetof(UniformBufferObject, texture);
	ubo_info_texture.range = sizeof(UniformBufferObject::Texture);

	// Ubo light
	VkDescriptorBufferInfo	ubo_info_light{};
	ubo_info_light.buffer = uniform_buffers;
	ubo_info_light.offset = offsetof(UniformBufferObject, light);
	ubo_info_light.range = sizeof(UniformBufferObject::Light);

	// Allow buffer udpate using descriptor write
	std::array<VkWriteDescriptorSet, 4>	descriptor_writes{};
	descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptor_writes[0].dstSet = vk_descriptor_sets;
	descriptor_writes[0].dstBinding = 0;
	descriptor_writes[0].dstArrayElement = 0;
	descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptor_writes[0].descriptorCount = 1;
	descriptor_writes[0].pBufferInfo = &ubo_info_camera;
	descriptor_writes[0].pImageInfo = nullptr;
	descriptor_writes[0].pTexelBufferView = nullptr;

	descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptor_writes[1].dstSet = vk_descriptor_sets;
	descriptor_writes[1].dstBinding = 1;
	descriptor_writes[1].dstArrayElement = 0;
	descriptor_writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptor_writes[1].descriptorCount = 1;
	descriptor_writes[1].pBufferInfo = nullptr;
	descriptor_writes[1].pImageInfo = &image_info;
	descriptor_writes[1].pTexelBufferView = nullptr;

	descriptor_writes[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptor_writes[2].dstSet = vk_descriptor_sets;
	descriptor_writes[2].dstBinding = 2;
	descriptor_writes[2].dstArrayElement = 0;
	descriptor_writes[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptor_writes[2].descriptorCount = 1;
	descriptor_writes[2].pBufferInfo = &ubo_info_texture;
	descriptor_writes[2].pImageInfo = nullptr;
	descriptor_writes[2].pTexelBufferView = nullptr;

	descriptor_writes[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptor_writes[3].dstSet = vk_descriptor_sets;
	descriptor_writes[3].dstBinding = 3;
	descriptor_writes[3].dstArrayElement = 0;
	descriptor_writes[3].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptor_writes[3].descriptorCount = 1;
	descriptor_writes[3].pBufferInfo = &ubo_info_light;
	descriptor_writes[3].pImageInfo = nullptr;
	descriptor_writes[3].pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(
		device.logical_device,
		static_cast<uint32_t>(descriptor_writes.size()),
		descriptor_writes.data(),
		0,
		nullptr
	);
}

void	DescriptorSet::createUniformBuffers(Device& device) {
	// Camera and texture are dynamically updated.
	VkDeviceSize	buffer_size = sizeof(UniformBufferObject);

	// Create the buffer and allocate memory
	device.createBuffer(
		buffer_size,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		uniform_buffers,
		uniform_buffers_memory
	);

	// Map it to allow CPU to write on it
	vkMapMemory(
		device.logical_device,
		uniform_buffers_memory,
		0,
		buffer_size,
		0,
		&uniform_buffers_mapped
	);
}

/**
 * @brief	Initiate uniform buffer.
*/
void	DescriptorSet::initUniformBuffer(
	const UniformBufferObject::Light& light
) noexcept {
	UniformBufferObject	ubo{};

	ubo.texture.state = static_cast<int32_t>(App::texture_state);
	ubo.texture.mix = -1.0f;
	ubo.light = light;

	memcpy(uniform_buffers_mapped, &ubo, sizeof(UniformBufferObject));
}

/**
 * Update the camera part of the uniform buffer.
 * 
 * TODO: Only update when needed.
*/
void	DescriptorSet::updateCamera(
	VkExtent2D extent
) {
	UniformBufferObject::Camera	camera{};

	static const std::array<scop::Vect3, 3>	axis = {
		scop::Vect3(1.0f, 0.0f, 0.0f),
		scop::Vect3(0.0f, 1.0f, 0.0f),
		scop::Vect3(0.0f, 0.0f, 1.0f)
	};

	// Add translation (object movement)
	App::position += App::movement;

	App::rotation_angles[RotationAxis::ROTATION_AXIS_X] +=
		App::rotating_input[RotationAxis::ROTATION_AXIS_X];

	App::rotation_angles[RotationAxis::ROTATION_AXIS_Y] +=
		App::rotating_input[RotationAxis::ROTATION_AXIS_Y];

	App::rotation_angles[RotationAxis::ROTATION_AXIS_Z] +=
		App::rotating_input[RotationAxis::ROTATION_AXIS_Z];

	// Define object transformation model
	camera.model = scop::rotate(
		scop::rotate(
			scop::rotate(
				// Translate object first
				scop::translate(
					scop::Mat4(1.0f),
					App::position
				),
				// Rotate around x
				scop::math::radians(App::rotation_angles[0]),
				axis[static_cast<int>(RotationAxis::ROTATION_AXIS_X)]
			),
			// Rotate around y
			scop::math::radians(App::rotation_angles[1]),
			axis[static_cast<int>(RotationAxis::ROTATION_AXIS_Y)]
		),
		// Rotate around z
		scop::math::radians(App::rotation_angles[2]),
		axis[static_cast<int>(RotationAxis::ROTATION_AXIS_Z)]
	);

	// Define camera transformation view
	camera.view = scop::lookAt(
		scop::App::eye_pos * scop::App::zoom_input,
		scop::Vect3(0.0f, 0.0f, 0.0f),
		axis[App::selected_up_axis]
	);

	// Define persp. projection transformation
	camera.proj = scop::perspective(
		scop::math::radians(45.0f),
		extent.width / static_cast<float>(extent.height),
		0.1f,
		100.0f
	);
	// Invert y axis (because y axis is inverted in Vulkan)
	camera.proj[5] *= -1;

	// Copy to uniform buffer
	memcpy(
		(char*)uniform_buffers_mapped,
		&camera,
		sizeof(UniformBufferObject::Camera)
	);
}

/**
 * Update the texture part of the uniform buffer.
*/
void	DescriptorSet::updateTexture() {
	// Only udpate if it was recently toggled
	if (!App::texture_transition_start.has_value()) {
		return;
	}
	UniformBufferObject::Texture	texture;
	time_point	current_time = std::chrono::high_resolution_clock::now();

	// Transition from 0 to 1 in /*transition_duration*/ ms	float
	float	time = std::chrono::duration<float, std::chrono::milliseconds::period>(
		current_time - App::texture_transition_start.value()
	).count() / App::transition_duration;

	texture.state = static_cast<int32_t>(App::texture_state);
	texture.mix = time;
	memcpy(
		(char*)uniform_buffers_mapped + offsetof(UniformBufferObject, texture),
		&texture,
		sizeof(UniformBufferObject::Texture)
	);

	// Reset texture_transition_start if time is up
	if (time >= 1.0f) {
		App::texture_transition_start.reset();
	}
}

/**
 * Update the light part of the uniform buffer.
*/
void	DescriptorSet::updateLight() {
	struct {
		alignas(__ALIGNMENT_VEC3) scop::Vect3	position;
		alignas(__ALIGNMENT_VEC3) scop::Vect3	color;
	} light_info = {
		App::light_positions[App::selected_light_pos],
		App::light_colors[App::selected_light_color]
	};
	
	memcpy(
		(char*)uniform_buffers_mapped + 
		offsetof(UniformBufferObject, light) +
		offsetof(UniformBufferObject::Light, light_pos),
		&light_info,
		sizeof(light_info)
	);
}

} // namespace graphics
} // namespace scop