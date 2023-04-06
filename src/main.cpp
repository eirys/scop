/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 03:53:55 by eli               #+#    #+#             */
/*   Updated: 2023/04/06 23:09:01 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Graphics
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// Std
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <optional>
#include <set>

/**
 * Un/comment to toggle NDEBUG mode and enable validation layers.
*/
#define NDEBUG
#include <cassert>

#include "utils.hpp"

class App {
public:
	struct QueueFamilyIndices {
		std::optional<uint32_t>	graphics_family;
		std::optional<uint32_t>	present_family;

		bool	isComplete() {
			return graphics_family.has_value() && present_family.has_value();
		}
	};

	void	run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private:
	const uint32_t					width = 800;
	const uint32_t					height = 600;
	const std::vector<const char*>	validation_layers = {
		"VK_LAYER_KHRONOS_validation"
	};

	#ifdef NDEBUG
	const bool						enable_validation_layers = false;
	#else
	const bool						enable_validation_layers = true;
	#endif

	GLFWwindow*						window;
	VkInstance						vk_instance;
	VkPhysicalDevice				physical_device = VK_NULL_HANDLE;
	VkDevice						logical_device;
	VkSurfaceKHR					vk_surface;
	VkQueue							graphics_queue;
	VkQueue							present_queue;

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
		createSurface();
		pickPhysicalDevice();
		createLogicalDevice();
	}

	void	mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			// Poll events while not pressing x...
			glfwPollEvents();
		}
	}

	void	cleanup() {
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
		
		return indices.isComplete();
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

		// Queue creation infos
		VkPhysicalDeviceFeatures	device_features{};
		VkDeviceCreateInfo			create_info{};

		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		create_info.pQueueCreateInfos = queue_create_infos.data();
		create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
		create_info.pEnabledFeatures = &device_features;

		// Validation layers
		// Deprecated, but set for compatibility with older Vulkan implementations
		create_info.enabledExtensionCount = 0;

		if (enable_validation_layers) {
			create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
			create_info.ppEnabledLayerNames = validation_layers.data();
		} else {
			create_info.enabledLayerCount = 0;
		}

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

};	// class App


int main() {
	App		app;

	#ifdef NDEBUG
	std::cout << "In debug mode" << NL;
	#endif

	try {
		app.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << NL;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}