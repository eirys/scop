/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   model.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 19:23:53 by eli               #+#    #+#             */
/*   Updated: 2023/05/06 22:48:58 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MODEL_HPP
# define MODEL_HPP

// Std
# include <vector>

# include "vector.hpp"

namespace scop {

class Model {
public:
	/* ========================================================================= */
	/*                                HELPER CLASS                               */
	/* ========================================================================= */

	struct Triangle {
		scop::Vect3	vertex_indices;
		scop::Vect3	normal_indices;
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

	std::vector<Triangle>	triangles;

}; // class Model

} // namespace scop

#endif