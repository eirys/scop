/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/06 12:28:42 by eli               #+#    #+#             */
/*   Updated: 2023/05/23 01:47:54 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "window.hpp"
#include "app.hpp"

namespace scop {

/* ========================================================================== */
/*                                    OTHER                                   */
/* ========================================================================== */

/**
 * Function callback for window resize
*/
static void	framebufferResizeCallback(
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
 * Toggles texture on/off
*/
static void	toggleTextureCallback() noexcept {
	using std::chrono::steady_clock;

	static steady_clock::time_point	key_pressed{};

	steady_clock::time_point	now = steady_clock::now();
	steady_clock::duration	duration =
		std::chrono::duration_cast<Window::milliseconds>(
			now - key_pressed
		);

	// Waits for the color transition to finish
	if (duration < Window::spam_delay) {
		return;
	}
	scop::App::toggleTexture();

	// Update last key press
	key_pressed = now;
}

/**
 * Function callback for key press
*/
static void	keyCallback(
	GLFWwindow* window,
	int key,
	int scancode,
	int action,
	int mods
) {
	(void)scancode;
	(void)mods;

	if (action == GLFW_PRESS) {
		switch (key) {
			case GLFW_KEY_ESCAPE:
				return glfwSetWindowShouldClose(window, GLFW_TRUE);

			// Texture toggle
			case GLFW_KEY_T:
				return toggleTextureCallback();

			// Camera orientation
			case GLFW_KEY_TAB:
				return App::changeUpAxis();

			// Rotation
			case GLFW_KEY_1:
				return App::toggleRotation(RotationInput::ROTATION_SUB_X);
			case GLFW_KEY_7:
				return App::toggleRotation(RotationInput::ROTATION_ADD_X);
			case GLFW_KEY_2:
				return App::toggleRotation(RotationInput::ROTATION_SUB_Y);
			case GLFW_KEY_8:
				return App::toggleRotation(RotationInput::ROTATION_ADD_Y);
			case GLFW_KEY_3:
				return App::toggleRotation(RotationInput::ROTATION_SUB_Z);
			case GLFW_KEY_9:
				return App::toggleRotation(RotationInput::ROTATION_ADD_Z);

			// Translation
			case GLFW_KEY_W:
				return App::toggleMove(ObjectDirection::MOVE_FORWARD);
			case GLFW_KEY_S:
				return App::toggleMove(ObjectDirection::MOVE_BACKWARD);
			case GLFW_KEY_A:
				return App::toggleMove(ObjectDirection::MOVE_LEFT);
			case GLFW_KEY_D:
				return App::toggleMove(ObjectDirection::MOVE_RIGHT);
			case GLFW_KEY_SPACE:
				return App::toggleMove(ObjectDirection::MOVE_UP);
			case GLFW_KEY_LEFT_CONTROL:
				return App::toggleMove(ObjectDirection::MOVE_DOWN);

			// Reset
			case GLFW_KEY_R:
				return App::resetModel();

			default:
				break;
		}
	} else if (action == GLFW_RELEASE) {
		switch (key) {
			// Rotation
			case GLFW_KEY_1:
				return App::untoggleRotation(RotationInput::ROTATION_SUB_X);
			case GLFW_KEY_7:
				return App::untoggleRotation(RotationInput::ROTATION_ADD_X);
			case GLFW_KEY_2:
				return App::untoggleRotation(RotationInput::ROTATION_SUB_Y);
			case GLFW_KEY_8:
				return App::untoggleRotation(RotationInput::ROTATION_ADD_Y);
			case GLFW_KEY_3:
				return App::untoggleRotation(RotationInput::ROTATION_SUB_Z);
			case GLFW_KEY_9:
				return App::untoggleRotation(RotationInput::ROTATION_ADD_Z);

			// Translation
			case GLFW_KEY_W:
				return App::untoggleMove(ObjectDirection::MOVE_FORWARD);
			case GLFW_KEY_S:
				return App::untoggleMove(ObjectDirection::MOVE_BACKWARD);
			case GLFW_KEY_A:
				return App::untoggleMove(ObjectDirection::MOVE_LEFT);
			case GLFW_KEY_D:
				return App::untoggleMove(ObjectDirection::MOVE_RIGHT);
			case GLFW_KEY_SPACE:
				return App::untoggleMove(ObjectDirection::MOVE_UP);
			case GLFW_KEY_LEFT_CONTROL:
				return App::untoggleMove(ObjectDirection::MOVE_DOWN);
			default:
				break;
		}
	}
}

/**
 * Function callback for mouse button press (unused)
*/
static void	mouseButtonCallback(
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

static void	scrollCallback(
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

	// int count;
	// GLFWmonitor** monitors = glfwGetMonitors(&count);

	window = glfwCreateWindow(
		width,
		height,
		window_title.c_str(),
		nullptr,
		nullptr
	);

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

} // namespace scop