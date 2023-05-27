/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/26 13:27:15 by etran             #+#    #+#             */
/*   Updated: 2023/05/28 01:51:20 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <memory> // std::unique_ptr

# include "vector.hpp"

namespace scop {
class Image;

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
	/*                                  TYPEDEFS                                 */
	/* ========================================================================= */

	typedef		std::unique_ptr<scop::Image>	ImagePtr;

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	Material(const Material& other):
		name(other.name),
		ambient_color(other.ambient_color),
		diffuse_color(other.diffuse_color),
		specular_color(other.specular_color),
		emissive_color(other.emissive_color),
		opacity(other.opacity),
		shininess(other.shininess),
		illum(other.illum),
		ambient_texture(std::move(other.ambient_texture).get()) {}

	Material() = default;
	Material(Material&& other) = default;
	~Material() = default;

	Material&	operator=(const Material& other) = delete;

	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	std::string			name;				// newmtl
	scop::Vect3			ambient_color;		// Ka
	scop::Vect3			diffuse_color;		// Kd
	scop::Vect3			specular_color;		// Ks
	scop::Vect3			emissive_color;		// Ke
	float				opacity;			// d or Tr
	std::size_t			shininess;			// Ns
	IlluminationModel	illum;
	ImagePtr			ambient_texture;	// map_Ka

}; // class Material

} // namespace mtl
} // namespace scop