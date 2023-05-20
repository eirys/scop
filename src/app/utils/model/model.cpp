/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   model.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 19:23:47 by eli               #+#    #+#             */
/*   Updated: 2023/05/19 00:56:25 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "model.hpp"
#include "image_handler.hpp"
#include "vector.hpp"
#include "vertex.hpp"
#include "utils.hpp"

#include <algorithm>
#include <set> // std::set

namespace scop {
namespace obj {

/* ========================================================================== */
/*                                   PUBLIC                                   */
/* ========================================================================== */

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

void	Model::setDefaultTextureCoords(const scop::Image& img) {
	texture_coords = {
		{ 0.0f, 0.0f },
		{ 1.0f, 0.0f },
		{ 1.0f, 1.0f },
		{ 0.0f, 1.0f }
	};

	size_t	i = 0;
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


	// Avoid texture stretching
	// texture_coords.reserve(vertex_coords.size());

	// float 	width = static_cast<float>(img.getWidth());
	// float 	height = static_cast<float>(img.getHeight());

	// Retrieve coplanar triangles.
	// We can't rely on the triangle normals.
	// We need to check the vertex values.
	// We can calculate the normal of a triangle using the cross product.

	// Sort by normals
	// std::map<scop::Vect3, std::vector<Triangle>>	coplanar_triangles;

	// for (const auto& triangle : triangles) {
	// 	const auto&	v1 = vertex_coords[triangle.indices[0].vertex];
	// 	const auto&	v2 = vertex_coords[triangle.indices[1].vertex];
	// 	const auto&	v3 = vertex_coords[triangle.indices[2].vertex];
	// 	scop::Vect3 normal = scop::normalize(scop::cross(v2 - v1, v3 - v1));
	// 	coplanar_triangles[normal].emplace_back(triangle);
	// }

	// For each normal (triangle set), generate texture coordinates,
	// where it's a continuous application of the texture.
	// The texture is applied on the triangle set as a whole.
	// for (const auto& normal: coplanar_triangles) {
	// 	// Get the triangle set
	// 	const auto&	triangles_vec = normal.second;
	// }
	(void)img;
}

void	Model::setDefaultNormalCoords() {
	// TODO
	return;
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

} // namespace obj
} // namespace scop