/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_module.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/16 15:03:08 by etran             #+#    #+#             */
/*   Updated: 2023/05/16 17:19:51 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# ifndef GLFW_INCLUDE_VULKAN
#  define GLFW_INCLUDE_VULKAN
# endif

# include <GLFW/glfw3.h>

namespace scop {
namespace graphics {

class GraphicsPipeline;

class DebugModule {
public:

	friend GraphicsPipeline;

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	DebugModule() = default;
	DebugModule(DebugModule&& other) = default;
	~DebugModule() = default;

	DebugModule(const DebugModule& other) = delete;
	DebugModule& operator=(const DebugModule& other) = delete;

	/* ========================================================================= */
	
	void							init(VkInstance vk_instance);
	void							destroy(VkInstance vk_instance);
	void							populate(
		VkDebugUtilsMessengerCreateInfoEXT& create_info
	);

private:
	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	VkDebugUtilsMessengerEXT		debug_messenger;

}; // class DebugModule

/* ========================================================================== */
/*                                    OTHER                                   */
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

VKAPI_ATTR	VkBool32 VKAPI_CALL	debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
	VkDebugUtilsMessageTypeFlagsEXT message_type,
	const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
	void* p_user_data
);

} // namespace graphics
} // namespace scop