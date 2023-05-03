/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/03 13:54:57 by eli               #+#    #+#             */
/*   Updated: 2023/05/03 14:17:17 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VECTOR_HPP
# define VECTOR_HPP

// Std
# include <cmath>

namespace scop {

struct Vect3 {
	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	float	x;
	float	y;
	float	z;

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	/* CONSTRUCTOR ============================================================= */

	Vect3() : x(0), y(0), z(0) {}

	Vect3(float x, float y, float z) : x(x), y(y), z(z) {}

	/* ACCESSORS =============================================================== */

	float& operator[](size_t index) {
		switch (index) {
			case 0:
				return x;
			case 1:
				return y;
			case 2:
				return z;
			default:
				throw std::out_of_range("Matrix index out of range");
		}
	}

	/* OPERATORS =============================================================== */

	Vect3	operator+(const Vect3& rhs) const {
		return Vect3{x + rhs.x, y + rhs.y, z + rhs.z};
	}

	Vect3	operator-(const Vect3& rhs) const {
		return Vect3{x - rhs.x, y - rhs.y, z - rhs.z};
	}

	Vect3	operator*(float rhs) const {
		return Vect3{x * rhs, y * rhs, z * rhs};
	}

	float	dot(const Vect3& rhs) const {
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	float	norm() const {
		return std::sqrt(x * x + y * y + z * z);
	}
};


struct Vect4 {
	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	float	x;
	float	y;
	float	z;
	float	w;

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	float&	operator[](size_t index) {
		switch (index) {
			case 0:
				return x;
			case 1:
				return y;
			case 2:
				return z;
			case 3:
				return w;
			default:
				throw std::out_of_range("Matrix index out of range");
		}
	}

	/* OPERATORS =============================================================== */

	Vect4	operator+(const Vect4& rhs) const {
		return Vect4{x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w};
	}

	Vect4	operator-(const Vect4& rhs) const {
		return Vect4{x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w};
	}

	Vect4	operator*(float rhs) const {
		return Vect4{x * rhs, y * rhs, z * rhs, w * rhs};
	}

};

} // namespace scop

#endif