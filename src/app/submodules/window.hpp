/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/06 12:28:36 by eli               #+#    #+#             */
/*   Updated: 2023/05/12 11:32:01 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WINDOW_HPP
# define WINDOW_HPP

// Window handler
# include <GLFW/glfw3.h>

// Std
# include <string>
# include <chrono>

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

	static constexpr uint32_t		width = 800;
	static constexpr uint32_t		height = 600;
	static constexpr char			title[] = "Etran's Scop: ";
	static constexpr std::chrono::milliseconds	spam_delay = std::chrono::milliseconds(500);

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	Window(const std::string& model_name);
	~Window();

	Window() = delete;
	Window(const Window& x) = delete;
	Window(Window&& x) = delete;
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