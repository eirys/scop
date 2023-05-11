/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/06 12:28:36 by eli               #+#    #+#             */
/*   Updated: 2023/05/11 10:57:50 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WINDOW_HPP
# define WINDOW_HPP

// Window handler
# include <GLFW/glfw3.h>

namespace scop {

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

/**
 * Window handler
*/
class Window {
public:
	/* ========================================================================= */
	/*                               CONST MEMBERS                               */
	/* ========================================================================= */

	const uint32_t		width = 800;
	const uint32_t		height = 600;
	const char*			window_title = "Etran's cute lil app";

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	Window();
	~Window();

	Window(const Window& x) = delete;
	Window& operator=(const Window& rhs) = delete;

	void							retrieveSize(int& width, int& height) const;
	
	void							pause() const;
	void							await() const;
	bool							alive() const;
	bool							resized() const;

	GLFWwindow*						getWindow();
	GLFWwindow const*				getWindow() const;
	
	void							toggleFrameBufferResized(bool resized);

private:
	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	GLFWwindow*						window;
	bool							frame_buffer_resized = false;

}; // class Window

} // namespace scop

#endif