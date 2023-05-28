/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/26 13:27:15 by etran             #+#    #+#             */
/*   Updated: 2023/05/28 11:56:47 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <memory> // std::unique_ptr

# include "vector.hpp"
# include "image_handler.hpp"

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
	/*                                  TYPEDEFS                                 */
	/* ========================================================================= */

	typedef		std::unique_ptr<scop::Image>	ImagePtr;

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	Material(Material&& other):
		name(std::move(other.name)),
		ambient_color(std::move(other.ambient_color)),
		diffuse_color(std::move(other.diffuse_color)),
		specular_color(std::move(other.specular_color)),
		emissive_color(std::move(other.emissive_color)),
		opacity(std::move(other.opacity)),
		shininess(std::move(other.shininess)),
		illum(std::move(other.illum)),
		ambient_texture(std::move(other.ambient_texture)) {}

	Material() = default;
	Material&	operator=(Material&& other) = default;
	~Material() = default;

	Material(const Material& other) = delete;
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
	ImagePtr			ambient_texture{};	// map_Ka

}; // class Material

} // namespace mtl
} // namespace scop

inline std::ostream& operator<<(std::ostream& o, const scop::mtl::Material& mat) {
	o	<< "Material: " << mat.name << '\n'
		<< "  Ambient color: " << mat.ambient_color << '\n'
		<< "  Diffuse color: " << mat.diffuse_color << '\n'
		<< "  Specular color: " << mat.specular_color << '\n'
		<< "  Emissive color: " << mat.emissive_color << '\n'
		<< "  Opacity: " << mat.opacity << '\n'
		<< "  Shininess: " << mat.shininess << '\n'
		<< "  Illumination model: " << static_cast<int>(mat.illum) << '\n'
		<< "  Ambient texture: " << mat.ambient_texture.get() << '\n';
	return o;
}