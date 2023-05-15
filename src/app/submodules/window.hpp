/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/06 12:28:36 by eli               #+#    #+#             */
/*   Updated: 2023/05/15 10:54:22 by etran            ###   ########.fr       */
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

/**
 * Window handler
*/
class Window {
public:
	/* ========================================================================= */
	/*                                  TYPEDEF                                  */
	/* ========================================================================= */

	typedef std::chrono::milliseconds		milliseconds;

	/* ========================================================================= */
	/*                               CONST MEMBERS                               */
	/* ========================================================================= */

	static constexpr const uint32_t		width = 800;
	static constexpr const uint32_t		height = 600;
	static constexpr const char			title[] = "Etran's Scop: ";
	static constexpr const milliseconds	spam_delay = std::chrono::milliseconds(500);

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	Window(const std::string& model_name);
	~Window();

	Window() = delete;
	Window(const Window& x) = delete;
	Window(Window&& x) = delete;
	Window& operator=(const Window& rhs) = delete;

	void							pause() const;
	void							await() const;
	bool							alive() const;
	bool							resized() const noexcept;

	void							retrieveSize(int& width, int& height) const;
	GLFWwindow*						getWindow() noexcept;
	GLFWwindow const*				getWindow() const noexcept;

	void							toggleFrameBufferResized(bool resized) noexcept;

private:
	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	GLFWwindow*						window = nullptr;
	bool							frame_buffer_resized = false;

}; // class Window

} // namespace scop

#endif