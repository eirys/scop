/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/26 13:27:15 by etran             #+#    #+#             */
/*   Updated: 2023/05/27 14:14:41 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "vector.hpp"

namespace scop {
// class Image;

namespace mtl {

enum IlluminationModel {
	ILLUM_REGULAR = 0,
	ILLUM_LAMBERTIAN = 1,
	ILLUM_LAMBERT_PHONG = 2,
	// ILLUM_OREN_NAYAR = 3,
	// ILLUM_MINNAERT = 4,
};

/**
 * Contains .mtl file data.
*/
struct Material {
	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	std::string			name;				// newmtl
	std::string			texture_path;		// TODO remove
	scop::Vect3			ambient_color;		// Ka
	scop::Vect3			diffuse_color;		// Kd
	scop::Vect3			specular_color;		// Ks
	scop::Vect3			emissive_color;		// Ke
	float				opacity;			// d or Tr
	std::size_t			shininess;			// Ns
	IlluminationModel	illum;
	// TODO
	// std::unique_ptr<scop::Image>	ambient_texture;	// map_Ka

}; // class Material

} // namespace mtl
} // namespace scop