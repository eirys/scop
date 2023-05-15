/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/06 12:28:42 by eli               #+#    #+#             */
/*   Updated: 2023/05/15 10:54:39 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "window.hpp"
#include "app.hpp"

namespace scop {

/* ========================================================================== */
/*                              STATIC FUNCTIONS                              */
/* ========================================================================== */

void	framebufferResizeCallback(
	GLFWwindow* window,
	int width,
	int height
);

void	keyCallback(
	GLFWwindow* window,
	int key,
	int scancode,
	int action,
	int mods
);

void	mouseButtonCallback(
	GLFWwindow* window,
	int button,
	int action,
	int mods
);

void	scrollCallback(
	GLFWwindow* window,
	double xoffset,
	double yoffset
);

void	toggleTextureCallback() noexcept;

/* ========================================================================== */
/*                                   PUBLIC                                   */
/* ========================================================================== */

Window::Window(const std::string& model_name) {
	// initialize glfw
	glfwInit();

	// disable OpenGL context creation
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// create a window pointer
	const std::string	window_title = title + model_name;

	window = glfwCreateWindow(width, height, window_title.c_str(), nullptr, nullptr);

	// set pointer to window to `this` instance pointer
	// so we can access it from the callback functions
	glfwSetWindowUserPointer(window, this);

	// Setup event callbacks
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetScrollCallback(window, scrollCallback);
}

Window::~Window() {
	if (window != nullptr) {
		// Remove window instance
		glfwDestroyWindow(window);
	}

	// Remove glfw instance
	glfwTerminate();
}

/* ========================================================================== */

void	Window::retrieveSize(int& width, int& height) const {
	glfwGetFramebufferSize(window, &width, &height);
}

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

bool	Window::resized() const noexcept {
	return frame_buffer_resized;
}

GLFWwindow*	Window::getWindow() noexcept {
	return window;
}

GLFWwindow const*	Window::getWindow() const noexcept {
	return window;
}

/* ========================================================================== */

void	Window::toggleFrameBufferResized(bool is_resized) noexcept {
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
	(void)scancode;
	(void)mods;

	if (action != GLFW_PRESS) {
		return;
	}
	// Ignore everything but those keys
	switch (key) {
		case GLFW_KEY_1:
			return App::toggleRotation(RotationAxis::ROTATION_X);
		case GLFW_KEY_2:
			return App::toggleRotation(RotationAxis::ROTATION_Y);
		case GLFW_KEY_3:
			return App::toggleRotation(RotationAxis::ROTATION_Z);
		case GLFW_KEY_4:
			return App::toggleRotation(RotationAxis::ROTATION_NONE);
		case GLFW_KEY_SPACE:
			return toggleTextureCallback();
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		default:
			break;
	}
}

/**
 * Toggles texture on/off
*/
void	toggleTextureCallback() noexcept {
	using std::chrono::steady_clock;

	// TODO: try disabling delay

	static steady_clock::time_point	key_pressed{};

	steady_clock::time_point	now = steady_clock::now();
	steady_clock::duration	duration =
		std::chrono::duration_cast<Window::milliseconds>(
			now - key_pressed
		);

	// Avoid key spamming
	if (duration < Window::spam_delay) {
		return;
	}
	scop::App::toggleTexture();

	// Update last key press
	key_pressed = now;
}

/**
 * Function callback for mouse button press (unused)
*/
void	mouseButtonCallback(
	GLFWwindow* window,
	int button,
	int action,
	int mods
) {
	(void)window;
	(void)button;
	(void)action;
	(void)mods;

	if (action != GLFW_PRESS) {
		return;
	}

	// Reset zoom on scroll click
	if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
		App::toggleZoom(ZoomInput::ZOOM_NONE);
	}
}

void	scrollCallback(
	GLFWwindow* window,
	double xoffset,
	double yoffset
) {
	(void)window;
	(void)xoffset;

	if (yoffset > 0) {
		App::toggleZoom(ZoomInput::ZOOM_IN);
	} else {
		App::toggleZoom(ZoomInput::ZOOM_OUT);
	}
}

} // namespace scop