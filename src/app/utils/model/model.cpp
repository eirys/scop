/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   model.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 19:23:47 by eli               #+#    #+#             */
/*   Updated: 2023/05/28 02:20:42 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "model.hpp"
#include "image_handler.hpp"
#include "vector.hpp"
#include "vertex.hpp"
#include "utils.hpp"
#include "material.hpp"
#include "ppm_loader.hpp"

namespace scop {
namespace obj {

/* ========================================================================== */
/*                                   PUBLIC                                   */
/* ========================================================================== */

Model::Model(Model&& x):
vertex_coords(std::move(x.vertex_coords)),
texture_coords(std::move(x.texture_coords)),
normal_coords(std::move(x.normal_coords)),
indices(std::move(x.indices)),
triangles(std::move(x.triangles)),
smooth_shading(x.smooth_shading) {
	if (x.material.has_value()) {
		material.emplace(std::move(x.material.value()));
	}
}

Model::Model(const Model& x):
vertex_coords(x.vertex_coords),
texture_coords(x.texture_coords),
normal_coords(x.normal_coords),
indices(x.indices),
triangles(x.triangles),
smooth_shading(x.smooth_shading) {
	if (x.material.has_value()) {
		material.emplace(std::move(x.material.value()));
	}
}

void	Model::addVertex(const Vect3& vertex) {
	vertex_coords.emplace_back(vertex);
}

void	Model::addTexture(const Vect2& texture) {
	texture_coords.emplace_back(texture);
}

void	Model::addTriangle(const Triangle& triangle) {
	triangles.emplace_back(triangle);
}

void 	Model::addNormal(const Vect3& normal) {
	normal_coords.emplace_back(normal);
}

void	Model::addIndex(const Index& index) {
	indices.emplace_back(index);
}

void	Model::setDefaultTextureCoords() {
	texture_coords = {
		{ 0.0f, 0.0f },
		{ 1.0f, 0.0f },
		{ 1.0f, 1.0f },
		{ 0.0f, 1.0f }
	};

	std::size_t	i = 0;
	for (auto& triangle: triangles) {
		if (i++ % 2) {
			triangle.indices[0].texture = 0;
			triangle.indices[1].texture = 1;
			triangle.indices[2].texture = 2;
		} else {
			triangle.indices[0].texture = 0;
			triangle.indices[1].texture = 2;
			triangle.indices[2].texture = 3;
		}
	}
}

void	Model::setDefaultNormalCoords() {
	// Reserve as many as the triangles.
	normal_coords.reserve(triangles.size());

	// Calculate normal coordinates for each triangles.
	for (Triangle& triangle: triangles) {
		// Calculate 2 coplanar vectors of the triangle
		const scop::Vect3	v1 =
			vertex_coords[triangle.indices[1].vertex] -
			vertex_coords[triangle.indices[0].vertex];
		const scop::Vect3	v2 =
			vertex_coords[triangle.indices[2].vertex] -
			vertex_coords[triangle.indices[0].vertex];

		// Save the normalized normal coordinates
		normal_coords.emplace_back(scop::normalize(scop::cross(v1, v2)));

		// Set the normal coordinates for each triangle index
		for (auto& index: triangle.indices) {
			index.normal = normal_coords.size() - 1;
		}
	}
	return;
}

void	Model::setMaterial(scop::mtl::Material&& mtl) {
	material.emplace(std::move(mtl));
	if (material->ambient_texture != nullptr) {
		return;
	} else {
		scop::PpmLoader ppm_loader(SCOP_TEXTURE_FILE_DEFAULT);
		material->ambient_texture.reset(new scop::Image(ppm_loader.load()));
	}
}

void	Model::toggleSmoothShading() noexcept {
	smooth_shading = true;
}

/* ========================================================================== */

const std::vector<Vect3>&	Model::getVertexCoords() const noexcept {
	return vertex_coords;
}

const std::vector<Vect2>&	Model::getTextureCoords() const noexcept {
	return texture_coords;
}

const std::vector<Model::Triangle>&	Model::getTriangles() const noexcept {
	return triangles;
}

const std::vector<Vect3>&	Model::getNormalCoords() const noexcept {
	return normal_coords;
}

const std::vector<Model::Index>&	Model::getIndices() const noexcept {
	return indices;
}

const std::optional<mtl::Material>&	Model::getMaterial() const noexcept {
	return material;
}

} // namespace obj
} // namespace scop