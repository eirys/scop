/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   model.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 19:23:47 by eli               #+#    #+#             */
/*   Updated: 2023/05/14 17:59:27 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "model.hpp"

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

void	Model::setDefaultTextureCoords() {
	texture_coords = default_texture_coords;

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
}

void	Model::setDefaultNormalCoords() {
	normal_coords = default_normal_coords;
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