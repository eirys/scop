/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/26 13:27:15 by etran             #+#    #+#             */
/*   Updated: 2023/05/26 13:39:56 by etran            ###   ########.fr       */
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
};

class Material {
public:
	Material();
private:
	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	scop::Vect3	ambient_color;	// Ka
	scop::Vect3	diffuse_color;	// Kd
	scop::Vect3	specular_color;	// Ks
	float		alpha; // d (transparency: 1.0 = opaque, 0.0 = fully transparent)
	float		shininess; // Ns
	

}; // class Material

} // namespace mtl
} // namespace scop