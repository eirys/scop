/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   app.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/28 11:12:12 by eli               #+#    #+#             */
/*   Updated: 2023/05/17 18:08:29 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app.hpp"
#include "matrix.hpp"
#include "model.hpp"
#include "parser.hpp"
#include "image_handler.hpp"
#include "math.hpp"

namespace scop {

bool							App::texture_enabled = true;
std::optional<App::time_point>	App::texture_enabled_start;
std::optional<Vect3>			App::rotation_axis;
float							App::zoom_input = 1.0f;
Vect3							App::up_axis = Vect3(0.0f, 1.0f, 0.0f);

/* ========================================================================== */
/*                                   PUBLIC                                   */
/* ========================================================================== */

App::App(
	const std::string& model_file,
	const std::string& texture_file
): window(model_file) {
	loadTexture(texture_file);
	loadModel(model_file);
	graphics_pipeline.init(window, *image, vertices, indices);
}

App::~App() {
	graphics_pipeline.destroy();
}

/* ========================================================================== */

void	App::run() {
	while (window.alive()) {
		window.await();
		drawFrame();
	}
	graphics_pipeline.idle();
}

/* ========================================================================== */

/**
 * On toggle, changes the texture of the model.
*/
void	App::toggleTexture() noexcept {
	texture_enabled = !texture_enabled;
	texture_enabled_start.emplace(
		std::chrono::high_resolution_clock::now()
	);
}

/**
 * On toggle, changes the rotation of the model.
*/
void	App::toggleRotation(RotationAxis axis) noexcept {
	if (axis == RotationAxis::ROTATION_X) {
		rotation_axis = Vect3(1.0f, 0.0f, 0.0f);
	} else if (axis == RotationAxis::ROTATION_Y) {
		rotation_axis = Vect3(0.0f, 1.0f, 0.0f);
	} else if (axis == RotationAxis::ROTATION_Z) {
		rotation_axis = Vect3(0.0f, 0.0f, 1.0f);
	} else {
		rotation_axis.reset();
	}
}

void	App::toggleZoom(ZoomInput zoom) noexcept {
	if (zoom == ZoomInput::ZOOM_NONE) {
		zoom_input = 1.0f;
		return;
	} else if (zoom == ZoomInput::ZOOM_IN && zoom_input < 2.0f) {
		zoom_input += 0.1f;
	} else if (zoom == ZoomInput::ZOOM_OUT && zoom_input > 0.2f) {
		zoom_input -= 0.1f;
	}
}

void	App::changeUpAxis(UpAxis axis) noexcept {
	if (axis == UpAxis::UP_X) {
		up_axis = Vect3(1.0f, 0.0f, 0.0f);
	} else if (axis == UpAxis::UP_Y) {
		up_axis = Vect3(0.0f, 1.0f, 0.0f);
	} else if (axis == UpAxis::UP_Z) {
		up_axis = Vect3(0.0f, 0.0f, 1.0f);
	}
}

/* ========================================================================== */
/*                                   PRIVATE                                  */
/* ========================================================================== */

/**
 * Fence awaited: the cpu waits until the frame is ready to be retrieved.
 *
 * Semaphores awaited: the gpu waits until the command buffer
 * is done executing, aka the image is available in the swap chain.
*/
void	App::drawFrame() {
	graphics_pipeline.render(window, indices.size());
}

void	App::loadModel(const std::string& path) {
	scop::obj::Parser	parser;
	scop::obj::Model	model = parser.parseFile(path.c_str());
	std::unordered_map<scop::Vertex, uint32_t>	unique_vertices{};

	const auto&	model_vertices = model.getVertexCoords();
	const auto& model_textures = model.getTextureCoords();
	// const auto& model_normals = model.getNormalCoords();
	const auto& model_triangles = model.getTriangles();

	// Retrieve unique vertices:
	for (const auto& triangle: model_triangles) {
		for (const auto& index: triangle.indices) {
			scop::Vertex	vertex{};

			vertex.pos = model_vertices[index.vertex];
			vertex.tex_coord = {
				model_textures[index.texture].x,
				1.0f - model_textures[index.texture].y
			};
			math::generateVibrantColor(vertex.color.x, vertex.color.y, vertex.color.z);
			// vertex.normal = model_normals[index.normal_index];

			if (unique_vertices.count(vertex) == 0) {
				unique_vertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.emplace_back(vertex);
			}
			indices.emplace_back(unique_vertices[vertex]);
		}
	}

	// Center model
	scop::Vect3	barycenter = utils::computeBarycenter(vertices);
	for (auto& vertex: vertices) {
		vertex.pos -= barycenter;
	}
}

/**
 * @brief	Creates texture loader object. If no path is provided, default
 * 			texture is loaded.
 *
 * @todo	Handle other image formats
*/
void	App::loadTexture(const std::string& path) {
	std::unique_ptr<scop::ImageLoader>	image_loader;
	std::string	file;

	// Only handle ppm files for now
	if (path.empty()) {
		file = SCOP_TEXTURE_FILE_HAMSTER_PPM;
	} else {
		size_t	extension_pos = path.rfind('.');
		if (extension_pos == std::string::npos) {
			throw std::invalid_argument(
				"No extention found for texture file (must be .ppm)"
			);
		} else if (path.find("ppm", extension_pos) == std::string::npos) {
			throw std::invalid_argument(
				"Texture file must be a ppm file (.ppm)"
			);
		}
		file = path;
	}
	image_loader.reset(new PpmLoader(file));
	image = std::make_unique<scop::Image>(image_loader->load());
}

} // namespace scop