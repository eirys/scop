/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/06 12:28:42 by eli               #+#    #+#             */
/*   Updated: 2023/05/06 12:34:12 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "window.hpp"

/* ========================================================================== */
/*                                   PUBLIC                                   */
/* ========================================================================== */

/* ========================================================================== */
/*                                   PRIVATE                                  */
/* ========================================================================== */

void	Window::initWindow() {
	// initialize glfw
	glfwInit();

	// disable OpenGL context creation
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// create a window pointer
	window = glfwCreateWindow(width, height, "Etran's cute lil app", nullptr, nullptr);

	// set pointer to window to `this` instance pointer
	glfwSetWindowUserPointer(window, this);

	// handle resizing
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}
