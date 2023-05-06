/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/06 12:28:42 by eli               #+#    #+#             */
/*   Updated: 2023/05/06 18:26:13 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "window.hpp"
#include "app.hpp"

namespace scop {

/* ========================================================================== */
/*                                   PUBLIC                                   */
/* ========================================================================== */

Window::Window() {
	// initialize glfw
	glfwInit();

	// disable OpenGL context creation
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// create a window pointer
	window = glfwCreateWindow(width, height, window_title, nullptr, nullptr);

	// set pointer to window to `this` instance pointer
	glfwSetWindowUserPointer(window, this);

	// handle resizing
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	glfwSetKeyCallback(window, keyCallback);
}

Window::~Window() {
	// Remove window instance
	glfwDestroyWindow(window);

	// Remove glfw instance
	glfwTerminate();
}

/* ========================================================================== */

void	Window::retrieveSize(int& width, int& height) const {
	glfwGetFramebufferSize(window, &width, &height);
}

/* ========================================================================== */

void	Window::pause() const {
	int	current_width, current_height;
	retrieveSize(current_width, current_height);

	while (current_width == 0 || current_height == 0) {
		retrieveSize(current_width, current_height);
		glfwWaitEvents();
	}
}

void	Window::await() const {
	glfwPollEvents();
}

bool	Window::alive() const {
	return !glfwWindowShouldClose(window);
}

bool	Window::resized() const {
	return frame_buffer_resized;
}

GLFWwindow*	Window::getWindow() {
	return window;
}

GLFWwindow const*	Window::getWindow() const {
	return window;
}

/* ========================================================================== */

void	Window::toggleFrameBufferResized(bool is_resized) {
	frame_buffer_resized = is_resized;
}

/* ========================================================================== */
/*                                    OTHER                                   */
/* ========================================================================== */

/**
 * Function callback for window resize
*/
void	framebufferResizeCallback(
	GLFWwindow* window,
	int width,
	int height
) {
	(void)width;
	(void)height;
	auto	handler = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	handler->toggleFrameBufferResized(true);
}

/**
 * Function callback for key press
*/
void	keyCallback(
	GLFWwindow* window,
	int key,
	int scancode,
	int action,
	int mods
) {
	(void)window;
	(void)scancode;
	(void)mods;

	if (key != GLFW_KEY_SPACE) {
		return;
	}
	if (action == GLFW_PRESS) {
		scop::App::toggleColorShift();
	}
}

} // namespace scop