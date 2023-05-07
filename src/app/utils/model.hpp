/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   model.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 19:23:53 by eli               #+#    #+#             */
/*   Updated: 2023/05/07 10:54:29 by eli              ###   ########.fr       */
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
	// 	std::array<uin32_t, 3>			vertex_indices;
	// 	std::array<uin32_t, 3>			texture_indices;
	// 	std::array<uin32_t, 3>			normal_indices;
	// };

	struct Material {

	};

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	Model() = default;
	~Model() = default;
	Model(const Model& x) = default;
	Model&	operator=(const Model& x) = default;

private:
	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	std::vector<Vertex>						vertices;

	// std::vector<Vect3>					vectors;
	// std::vector<Triangle>				triangles;
	// std::map<std::string, std::string>	textures;

}; // class Model

} // namespace scop

#endif