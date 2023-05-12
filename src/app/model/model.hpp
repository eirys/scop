/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   model.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 19:23:53 by eli               #+#    #+#             */
/*   Updated: 2023/05/12 23:31:31 by etran            ###   ########.fr       */
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
namespace obj {

class Model {
public:
	/* ========================================================================= */
	/*                                HELPER CLASS                               */
	/* ========================================================================= */

	struct Index {
		int							vertex;
		int							texture;
		int							normal;

		int	operator[](size_t i) const {
			switch (i) {
				case 0: return vertex;
				case 1: return texture;
				case 2: return normal;
				default: throw std::out_of_range("Index out of range");
			}
		}
		int&	operator[](size_t i) {
			switch (i) {
				case 0: return vertex;
				case 1: return texture;
				case 2: return normal;
				default: throw std::out_of_range("Index out of range");
			}
		}
	};

	struct Triangle {
		std::array<Index, 3>		indices;
	};

	const std::vector<Vect2>		default_texture_coords = {
		{ 0.0f, 0.0f },
		{ 1.0f, 0.0f },
		{ 1.0f, 1.0f },
		{ 0.0f, 1.0f }
	};
	const std::vector<Vect3>		default_normal_coords = {};	// TODO

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	Model() = default;
	~Model() = default;
	Model(const Model& x) = default;
	Model(Model&& x) = default;
	Model&	operator=(const Model& x) = default;

	/* ========================================================================= */

	void								addVertex(const Vect3& vertex);
	void								addTexture(const Vect2& texture);
	void								addNormal(const Vect3& normal);
	void								addIndex(const Index& index);
	void								addTriangle(const Triangle& triangle);

	void								setDefaultTextureCoords();
	void								setDefaultNormalCoords();

	const std::vector<Vect3>&			getVertexCoords() const noexcept;
	const std::vector<Vect2>&			getTextureCoords() const noexcept;
	const std::vector<Vect3>&			getNormalCoords() const noexcept;
	const std::vector<Index>&			getIndices() const noexcept;
	const std::vector<Triangle>&		getTriangles() const noexcept;

private:
	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	std::vector<Vect3>					vertex_coords;
	std::vector<Vect2>					texture_coords;
	std::vector<Vect3>					normal_coords;
	std::vector<Index>					indices;
	std::vector<Triangle>				triangles;

}; // class Model

} // namespace obj
} // namespace scop

#endif