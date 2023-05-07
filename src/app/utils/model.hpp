/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   model.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 19:23:53 by eli               #+#    #+#             */
/*   Updated: 2023/05/07 21:03:34 by eli              ###   ########.fr       */
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

	struct Triangle {
		std::array<uin32_t, 3>			vertex_indices;
		std::array<uin32_t, 3>			texture_indices;

		// std::array<uin32_t, 3>			normal_indices;
	};

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	Model() = default;
	~Model() = default;
	Model(const Model& x) = default;
	Model&	operator=(const Model& x) = default;

	/* ========================================================================= */

	const std::vector<Vect3>&			getVertexCoords() const;
	const std::vector<Vect2>&			getTextureCoords() const;
	const std::vector<Triangle>&		getTriangles() const;

	// const std::vector<Vect3>&			getNormalCoords() const;

private:
	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	std::vector<Vect3>					vertex_coords;
	std::vector<Vect2>					texture_coords;
	std::vector<Triangle>				triangles;

	// std::vector<Vect3>					normal_coords;

}; // class Model

} // namespace scop

#endif