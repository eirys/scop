/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   app.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/28 11:12:12 by eli               #+#    #+#             */
/*   Updated: 2023/05/19 01:26:48 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "app.hpp"
#include "model.hpp"
#include "parser.hpp"
#include "ppm_loader.hpp"
#include "math.hpp"

namespace scop {

bool							App::texture_enabled = true;
std::optional<App::time_point>	App::texture_enabled_start;

std::array<float, 3>			App::rotation_angles = { 0.0f, 0.0f, 0.0f };
std::array<scop::Mat4, 3>		App::rotation_matrices = {
	scop::Mat4(1.0f),
	scop::Mat4(1.0f),
	scop::Mat4(1.0f)
};
scop::Vect3						App::movement = scop::Vect3(0.0f, 0.0f, 0.0f);
scop::Vect3						App::position = scop::Vect3(0.0f, 0.0f, 0.0f);

float							App::zoom_input = 1.0f;

size_t							App::selected_up_axis = 1;

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

void	App::resetModel() noexcept {
	rotation_matrices[0] = scop::Mat4(1.0f);
	rotation_matrices[1] = scop::Mat4(1.0f);
	rotation_matrices[2] = scop::Mat4(1.0f);

	rotation_angles[0] = 0.0f;
	rotation_angles[1] = 0.0f;
	rotation_angles[2] = 0.0f;

	position = scop::Vect3(0.0f, 0.0f, 0.0f);
}

/**
 * On toggle, changes the rotation of the model.
*/
void	App::updateRotation(RotationAxis dir, RotationInput value) noexcept {
	static const std::array<scop::Vect3, 3>	axis = {
		scop::Vect3(1.0f, 0.0f, 0.0f),
		scop::Vect3(0.0f, 1.0f, 0.0f),
		scop::Vect3(0.0f, 0.0f, 1.0f)
	};

	for (size_t i = 0; i < 4; ++i) {
		if (i == static_cast<size_t>(dir)) {
			rotation_angles[i] += (
				value == RotationInput::ROTATION_INPUT_ADD ? +10 :-10
			);
			rotation_matrices[i] = scop::rotate(
				scop::math::radians(rotation_angles[i]),
				axis[i]
			);
		}
	}
}

/**
 * On toggle, the model is moved in the given direction.
*/
void	App::toggleMove(ObjectDirection dir) noexcept {
	switch (dir) {
		case ObjectDirection::MOVE_FORWARD:
			if (movement.z == 0.0f)
				movement.z = -SCOP_MOVE_SPEED;
			break;
		case ObjectDirection::MOVE_BACKWARD:
			if (movement.z == 0.0f)
				movement.z = SCOP_MOVE_SPEED;
			break;
		case ObjectDirection::MOVE_RIGHT:
			if (movement.x == 0.0f)
				movement.x = SCOP_MOVE_SPEED;
			break;
		case ObjectDirection::MOVE_LEFT:
			if (movement.x == 0.0f)
				movement.x = -SCOP_MOVE_SPEED;
			break;
		case ObjectDirection::MOVE_UP:
			if (movement.y == 0.0f)
				movement.y = SCOP_MOVE_SPEED;
			break;
		case ObjectDirection::MOVE_DOWN:
			if (movement.y == 0.0f)
				movement.y = -SCOP_MOVE_SPEED;
			break;
		default:
			return;
	}
}

/**
 * On untoggle, the model stops moving in the given direction.
*/
void	App::untoggleMove(ObjectDirection dir) noexcept {
	if (
		dir == ObjectDirection::MOVE_FORWARD ||
		dir == ObjectDirection::MOVE_BACKWARD
	) {
		movement.z = 0.0f;
	} else if (
		dir == ObjectDirection::MOVE_RIGHT ||
		dir == ObjectDirection::MOVE_LEFT
	) {
		movement.x = 0.0f;
	} else if (
		dir == ObjectDirection::MOVE_UP ||
		dir == ObjectDirection::MOVE_DOWN
	) {
		movement.y = 0.0f;
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

void	App::changeUpAxis() noexcept {
	selected_up_axis = (selected_up_axis + 1) % 3;
}

/* ========================================================================== */
/*                                   PRIVATE                                  */
/* ========================================================================== */

void	App::drawFrame() {
	graphics_pipeline.render(window, indices.size());
}

void	App::loadModel(const std::string& path) {
	scop::obj::Parser	parser;
	scop::obj::Model	model = parser.parseFile(path.c_str(), *image);
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
			math::generateVibrantColor(
				vertex.color.x,
				vertex.color.y,
				vertex.color.z
			);
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