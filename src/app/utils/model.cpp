/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   model.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 19:23:47 by eli               #+#    #+#             */
/*   Updated: 2023/05/11 17:34:08 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "model.hpp"

namespace scop {

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

/* ========================================================================== */

const std::vector<Vect3>&	Model::getVertexCoords() const {
	return vertex_coords;
}

const std::vector<Vect2>&	Model::getTextureCoords() const {
	return texture_coords;
}

const std::vector<Model::Triangle>&	Model::getTriangles() const {
	return triangles;
}

const std::vector<Vect3>&	Model::getNormalCoords() const {
	return normal_coords;
}

const std::vector<Model::Index>&	Model::getIndices() const {
	return indices;
}

} // namespace scop