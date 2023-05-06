/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   model.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 19:23:53 by eli               #+#    #+#             */
/*   Updated: 2023/05/05 12:47:47 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MODEL_HPP
# define MODEL_HPP

// Std
# include <vector>

# include "vector.hpp"
# include "vertex.hpp"

class Model {
public:

	struct Face {
		std::vector<scop::Vect3>	vertex_indices;
	};

private:
	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	/**
	 * Vertices of the model
	*/
	std::vector<Vertex>		vertices;

	/**
	 * 
	*/
	std::vector<uint32_t>	indices;

};

#endif