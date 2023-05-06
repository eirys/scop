/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/03 13:54:57 by eli               #+#    #+#             */
/*   Updated: 2023/05/06 12:24:53 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VECTOR_HPP
# define VECTOR_HPP

// Std
# include <cmath>
# include <stdexcept>

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

	Vect3(): x(0), y(0), z(0) {}
	Vect3(float new_x, float new_y, float new_z): x(new_x), y(new_y), z(new_z) {}

	/* ACCESSORS =============================================================== */

	const float&	operator[](size_t index) const {
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

	float&	operator[](size_t index) {
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
		return Vect3{ x + rhs.x, y + rhs.y, z + rhs.z };
	}

	Vect3	operator-(const Vect3& rhs) const {
		return Vect3{ x - rhs.x, y - rhs.y, z - rhs.z };
	}

	Vect3	operator*(float rhs) const {
		return Vect3{ x * rhs, y * rhs, z * rhs };
	}

	/**
	 * @brief Returns the dot product of the vector with another vector
	 */
	float	dot(const Vect3& rhs) const {
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	/**
	 * @brief Returns the norm of the vector
	*/
	float	norm() const {
		return std::sqrt(dot(*this));
	}

	/**
	 * @brief Returns a normalized vector
	*/
	Vect3	normalize() const {
		float	n = norm();
		return Vect3{ x / n, y / n, z / n };
	}

	/**
	 * @brief Returns the cross product of the vector with another vector
	*/
	Vect3	cross(const Vect3& rhs) const {
		return Vect3{
			y * rhs.z - z * rhs.y,
			z * rhs.x - x * rhs.z,
			x * rhs.y - y * rhs.x
		};
	}

	/* BOOLEAN COMPARISON ====================================================== */

	bool	operator==(const Vect3& rhs) const {
		return x == rhs.x && y == rhs.y && z == rhs.z;
	}
}; // struct Vect3

struct Vect2 {
	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	float	x;
	float	y;

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	Vect2(): x(0), y(0) {}
	Vect2(float x, float y): x(x), y(y) {}

	/* CONSTRUCTOR ============================================================= */

	float&	operator[](size_t index) {
		switch (index) {
			case 0:
				return x;
			case 1:
				return y;
			default:
				throw std::out_of_range("Matrix index out of range");
		}
	}

	/* OPERATORS =============================================================== */

	Vect2	operator+(const Vect2& rhs) const {
		return Vect2{x + rhs.x, y + rhs.y};
	}

	Vect2	operator-(const Vect2& rhs) const {
		return Vect2{x - rhs.x, y - rhs.y};
	}

	Vect2	operator*(float rhs) const {
		return Vect2{x * rhs, y * rhs};
	}

	/* BOOLEAN COMPARISON ====================================================== */

	bool	operator==(const Vect2& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
}; // struct Vect2

} // namespace scop

namespace std {

template<>
struct hash<scop::Vect3> {
	size_t	operator()(const scop::Vect3& vect) const {
		return (
			hash<float>()(vect.x) ^
			hash<float>()(vect.y) ^
			hash<float>()(vect.z)
		);
	}
};

template<>
struct hash<scop::Vect2> {
	size_t	operator()(const scop::Vect2& vect) const {
		return (
			hash<float>()(vect.x) ^
			hash<float>()(vect.y)
		);
	}
};

} // namespace std

#endif