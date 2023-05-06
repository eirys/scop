/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/06 12:28:36 by eli               #+#    #+#             */
/*   Updated: 2023/05/06 13:10:40 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WINDOW_HPP
# define WINDOW_HPP

// Graphics
# ifndef GLFW_INCLUDE_VULKAN
#  define GLFW_INCLUDE_VULKAN
# endif

// Window handler
# include <GLFW/glfw3.h>

namespace scop {


void								framebufferResizeCallback(
	GLFWwindow* window,
	int width,
	int height
);

/**
 * Window handler
*/
class Window {

public:
	/* ========================================================================= */
	/*                               CONST MEMBERS                               */
	/* ========================================================================= */

	static constexpr uint32_t		width = 800;
	static constexpr uint32_t		height = 600;
	static constexpr const char*	window_title = "Etran's cute lil app";

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	Window();
	~Window();

	void							retrieveSize(int& width, int& height) const;
	
	void							pause() const;
	void							await() const;
	bool							alive() const;

	GLFWwindow*						getWindow();
	GLFWwindow const*				getWindow() const;

private:
	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	GLFWwindow*						window;

}; // class Window

} // namespace scop

#endif