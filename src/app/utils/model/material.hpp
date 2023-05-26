/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/26 13:27:15 by etran             #+#    #+#             */
/*   Updated: 2023/05/27 01:24:32 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "vector.hpp"

namespace scop {
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

	std::string			name; // newmtl
	scop::Vect3			ambient_color;	// Ka
	scop::Vect3			diffuse_color;	// Kd
	scop::Vect3			specular_color;	// Ks
	float				transparency; // Tr
	std::size_t			shininess; // Ns
	IlluminationModel	illum;

}; // class Material

} // namespace mtl
} // namespace scop