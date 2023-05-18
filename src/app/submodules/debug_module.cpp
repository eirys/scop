/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_module.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/16 15:07:01 by etran             #+#    #+#             */
/*   Updated: 2023/05/16 16:22:36 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug_module.hpp"

#include <stdexcept> // std::runtime_error
#include <iostream> // std::cerr

#include "window.hpp"
#include "graphics_pipeline.hpp"
#include "device.hpp"
#include "render_target.hpp"
#include "texture_sampler.hpp"
#include "descriptor_set.hpp"
#include "command_buffer.hpp"
#include "vertex_input.hpp"

namespace scop {
namespace graphics {

/* ========================================================================== */
/*                                   PUBLIC                                   */
/* ========================================================================== */

/**
 * Create a debug messenger to handle Vulkan errors
*/
void	DebugModule::init(
	VkInstance vk_instance
) {
	if (!GraphicsPipeline::enable_validation_layers) return;

	VkDebugUtilsMessengerCreateInfoEXT	create_info{};
	populate(create_info);

	if (CreateDebugUtilsMessengerEXT(vk_instance, &create_info, nullptr, &debug_messenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to setup debug messenger");
	}
}

void	DebugModule::destroy(
	VkInstance vk_instance
) {
	if (!GraphicsPipeline::enable_validation_layers) return;
	DestroyDebugUtilsMessengerEXT(vk_instance, debug_messenger, nullptr);
}

/**
 * Explicit which debug messages are to be handled
*/
void	DebugModule::populate(
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

/**
 * Debug callback function, used by validation layers
*/
VKAPI_ATTR VkBool32 VKAPI_CALL	debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
	VkDebugUtilsMessageTypeFlagsEXT message_type,
	const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
	void* p_user_data
) {
	(void)message_severity;
	(void)message_type;
	(void)p_user_data;
	std::cerr << "[validation layer] " << p_callback_data->pMessage << std::endl;
	return VK_FALSE;
}

} // namespace graphics
} // namespace scop