/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   model.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 19:23:53 by eli               #+#    #+#             */
/*   Updated: 2023/05/09 21:49:28 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MODEL_HPP
# define MODEL_HPP

// Std
# include <vector>
# include <array>
# include <map>

# include "vector.hpp"
# include "vertex.hpp"

namespace scop {

class Model {
public:
	/* ========================================================================= */
	/*                                HELPER CLASS                               */
	/* ========================================================================= */

	// struct Triangle {
	// 	std::array<uint32_t, 3>			vertex_indices;
	// 	std::array<uint32_t, 2>			texture_indices;
	// 	std::array<uint32_t, 3>			normal_indices;
	// };

	struct Index {
		uint32_t							vertex;
		uint32_t							texture;
		uint32_t							normal;

		uint32_t	operator[](size_t i) const {
			switch (i) {
				case 0: return vertex;
				case 1: return texture;
				case 2: return normal;
				default: throw std::out_of_range("Index out of range");
			}
		}
		uint32_t&	operator[](size_t i) {
			switch (i) {
				case 0: return vertex;
				case 1: return texture;
				case 2: return normal;
				default: throw std::out_of_range("Index out of range");
			}
		}
	};

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	Model() = default;
	~Model() = default;
	Model(const Model& x) = default;
	Model&	operator=(const Model& x) = default;

	/* ========================================================================= */

	void								addVertex(const Vect3& vertex);
	void								addTexture(const Vect2& texture);
	// void								addTriangle(const Triangle& triangle);
	void								addNormal(const Vect3& normal);
	void								addIndex(const Index& index);

	const std::vector<Vect3>&			getVertexCoords() const;
	const std::vector<Vect2>&			getTextureCoords() const;
	// const std::vector<Triangle>&		getTriangles() const;
	const std::vector<Vect3>&			getNormalCoords() const;
	const std::vector<Index>&			getIndices() const;

private:
	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	std::vector<Vect3>					vertex_coords;
	std::vector<Vect2>					texture_coords;
	// std::vector<Triangle>				triangles;
	std::vector<Vect3>					normal_coords;
	std::vector<Index>					indices;

}; // class Model

} // namespace scop

#endif