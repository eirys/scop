/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   app.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/10 18:21:34 by eli               #+#    #+#             */
/*   Updated: 2023/05/16 16:01:51 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef APP_HPP
# define APP_HPP

// Graphics
# ifndef GLFW_INCLUDE_VULKAN
#  define GLFW_INCLUDE_VULKAN
# endif

// Window handler
# include <GLFW/glfw3.h>

// Std
# include <iostream>
# include <stdexcept>
# include <cstdlib>
# include <vector>
# include <optional>
# include <limits>
# include <algorithm>
# include <cassert>
# include <chrono>
# include <unordered_map>
# include <memory>

# include "window.hpp"
# include "utils.hpp"
# include "vertex.hpp"
# include "uniform_buffer_object.hpp"
# include "image_handler.hpp"
# include "ppm_loader.hpp"
# include "graphics_pipeline.hpp"

# define SCOP_TEXTURE_FILE_HAMSTER_PPM	"textures/hammy.ppm"

namespace scop {

enum RotationAxis {
	ROTATION_X,
	ROTATION_Y,
	ROTATION_Z,
	ROTATION_NONE
};

enum ZoomInput {
	ZOOM_IN,
	ZOOM_OUT,
	ZOOM_NONE
};

enum UpAxis {
	UP_X,
	UP_Y,
	UP_Z
};

/**
 * Core engine.
*/
class App {
private:
	/* ========================================================================= */
	/*                                  TYPEDEF                                  */
	/* ========================================================================= */

	typedef	std::chrono::high_resolution_clock::time_point	time_point;

public:

	friend class graphics::DescriptorSet;

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
	static void							toggleRotation(RotationAxis axis) noexcept;
	static void							toggleZoom(ZoomInput input) noexcept;
	static void							changeUpAxis(UpAxis axis) noexcept;

private:
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
	static std::optional<Vect3>			rotation_axis;
	static float						zoom_input;
	static Vect3						up_axis;

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	void							drawFrame();

	void							loadModel(
		const std::string& path
	);
	void							loadTexture(const std::string& path);


}; // class App

} // namespace scop

#endif