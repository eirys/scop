/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   app.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/10 18:21:34 by eli               #+#    #+#             */
/*   Updated: 2023/05/21 11:40:52 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// Graphics
# ifndef GLFW_INCLUDE_VULKAN
#  define GLFW_INCLUDE_VULKAN
# endif

// Window handler
# include <GLFW/glfw3.h>

// Std
# include <memory> // std::unique_ptr

# include "window.hpp"
# include "utils.hpp"
# include "matrix.hpp"
# include "vertex.hpp"
# include "image_handler.hpp"
# include "graphics_pipeline.hpp"

# define SCOP_TEXTURE_FILE_HAMSTER_PPM	"textures/hammy.ppm"
# define SCOP_MOVE_SPEED				0.005f

namespace scop {

enum RotationAxis {
	ROTATION_AXIS_X = 0,
	ROTATION_AXIS_Y = 1,
	ROTATION_AXIS_Z = 2
};

enum RotationInput {
	ROTATION_INPUT_ADD,
	ROTATION_INPUT_SUB
};

enum ObjectDirection {
	MOVE_FORWARD		= 1 << 0,
	MOVE_BACKWARD		= 1 << 1,
	MOVE_LEFT			= 1 << 2,
	MOVE_RIGHT			= 1 << 3,
	MOVE_UP				= 1 << 4,
	MOVE_DOWN			= 1 << 5
};

enum ZoomInput {
	ZOOM_IN,
	ZOOM_OUT,
	ZOOM_NONE
};

/**
 * Core engine.
*/
class App {
public:

	friend graphics::DescriptorSet;

	/* ========================================================================= */
	/*                               CONST MEMBERS                               */
	/* ========================================================================= */

	static constexpr float			transition_duration = 300.0f;	// ms

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	App(const std::string& model_file, const std::string& texture_file);
	~App();

	App() = delete;
	App(const App& x) = delete;
	App(App&& x) = delete;
	App& operator=(const App& rhs) = delete;

	/* ========================================================================= */

	void								run();
	static void							toggleTexture() noexcept;
	static void							resetModel() noexcept;
	static void							updateRotation(
		RotationAxis axis, 
		RotationInput value
	) noexcept;
	static void							toggleMove(
		ObjectDirection direction
	) noexcept;
	static void							untoggleMove(
		ObjectDirection direction
	) noexcept;
	static void							toggleZoom(ZoomInput input) noexcept;
	static void							changeUpAxis() noexcept;

private:
	/* ========================================================================= */
	/*                                  TYPEDEF                                  */
	/* ========================================================================= */

	typedef	std::chrono::high_resolution_clock::time_point	time_point;

	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	std::unique_ptr<scop::Image>		image;
	scop::Window						window;
	scop::graphics::GraphicsPipeline	graphics_pipeline;

	std::vector<scop::Vertex>			vertices;
	std::vector<uint32_t>				indices;

	/* ========================================================================= */
	/*                               STATIC MEMBERS                              */
	/* ========================================================================= */

	static bool							texture_enabled;
	static std::optional<time_point>	texture_enabled_start;
	static std::array<float, 3>			rotation_angles;
	static scop::Vect3					position;
	static scop::Vect3					movement;
	static float						zoom_input;
	static size_t						selected_up_axis;

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	void								drawFrame();
	void								loadModel(const std::string& path);
	void								loadTexture(const std::string& path);

}; // class App

} // namespace scop