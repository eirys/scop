/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 03:53:55 by eli               #+#    #+#             */
/*   Updated: 2023/04/06 16:17:14 by eli              ###   ########.fr       */
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

#include "utils.hpp"

class App {
public:
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

	GLFWwindow*						window;
	VkInstance						vk_instance;

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
	}

	void	mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			// Poll events while not pressing x...
			glfwPollEvents();
		}
	}

	void	cleanup() {
		// Remove vk instance
		vkDestroyInstance(vk_instance, nullptr);

		// Remove window instance
		glfwDestroyWindow(window);

		glfwTerminate();
	}

	void	createInstance() {
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
};	// class App


int main() {
	App		app;

	try {
		app.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}