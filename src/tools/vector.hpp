/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/03 13:54:57 by eli               #+#    #+#             */
/*   Updated: 2023/05/17 17:56:42 by etran            ###   ########.fr       */
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

	Vect3(): x(0), y(0), z(0) {}
	Vect3(float new_x, float new_y, float new_z): x(new_x), y(new_y), z(new_z) {}

	Vect3(const Vect3& other) = default;
	Vect3(Vect3&& other) = default;
	Vect3& operator=(const Vect3& rhs) = default;
	~Vect3() = default;

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

	Vect3	operator-() const noexcept {
		return Vect3(-x, -y, -z);
	}

	Vect3&	operator+=(const Vect3& rhs) noexcept {
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	Vect3	operator+(const Vect3& rhs) const noexcept {
		Vect3	res(*this);
		return res.operator+=(rhs);
	}

	Vect3&	operator-=(const Vect3& rhs) noexcept {
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	Vect3	operator-(const Vect3& rhs) const noexcept {
		Vect3	res(*this);
		return res.operator-=(rhs);
	}

	Vect3&	operator*=(float rhs) noexcept {
		x *= rhs;
		y *= rhs;
		z *= rhs;
		return *this;
	}

	Vect3	operator*(float rhs) const noexcept {
		Vect3	res(*this);
		return res.operator*=(rhs);
	}

	Vect3&	operator/=(float rhs) noexcept {
		x /= rhs;
		y /= rhs;
		z /= rhs;
		return *this;
	}

	Vect3	operator/(float rhs) const noexcept {
		Vect3	res(*this);
		return res.operator/=(rhs);
	}

	/* ========================================================================= */

	/**
	 * @brief Returns the dot product of the vector with another vector
	 */
	float	dot(const Vect3& rhs) const noexcept {
		return static_cast<float>(
			std::fma(x, rhs.x, std::fma(y, rhs.y, std::fma(z, rhs.z, 0)))
		);
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
	Vect3	cross(const Vect3& rhs) const noexcept {
		return Vect3{
			static_cast<float>(std::fma(y, rhs.z, std::fma(-z, rhs.y, 0))),
			static_cast<float>(std::fma(z, rhs.x, std::fma(-x, rhs.z, 0))),
			static_cast<float>(std::fma(x, rhs.y, std::fma(-y, rhs.x, 0)))
		};
	}

	/* BOOLEAN COMPARISON ====================================================== */

	bool	operator==(const Vect3& rhs) const noexcept {
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

	Vect2(const Vect2& other) = default;
	Vect2& operator=(const Vect2& rhs) = default;
	~Vect2() = default;

	/* ACCESSORS =============================================================== */

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

	Vect2	operator-() const noexcept {
		return Vect2{ -x, -y };
	}

	Vect2&	operator+=(const Vect2& rhs) noexcept {
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	Vect2	operator+(const Vect2& rhs) const noexcept {
		Vect2	res(*this);
		return res.operator+=(rhs);
	}

	Vect2&	operator-=(const Vect2& rhs) noexcept {
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	Vect2	operator-(const Vect2& rhs) const noexcept {
		Vect2	res(*this);
		return res.operator-=(rhs);
	}

	Vect2&	operator*=(float rhs) noexcept {
		x *= rhs;
		y *= rhs;
		return *this;
	}

	Vect2	operator*(float rhs) const noexcept {
		Vect2	res(*this);
		return res.operator*=(rhs);
	}

	Vect2&	operator/=(float rhs) noexcept {
		x /= rhs;
		y /= rhs;
		return *this;
	}

	Vect2	operator/(float rhs) const noexcept {
		Vect2	res(*this);
		return res.operator/=(rhs);
	}

	/* BOOLEAN COMPARISON ====================================================== */

	bool	operator==(const Vect2& rhs) const noexcept {
		return x == rhs.x && y == rhs.y;
	}

}; // struct Vect2

} // namespace scop

namespace std {

template<>
struct hash<scop::Vect3> {
	inline size_t	operator()(const scop::Vect3& vect) const {
		return (
			hash<float>()(vect.x) ^
			hash<float>()(vect.y) ^
			hash<float>()(vect.z)
		);
	}
};

template<>
struct hash<scop::Vect2> {
	inline size_t	operator()(const scop::Vect2& vect) const {
		return (
			hash<float>()(vect.x) ^
			hash<float>()(vect.y)
		);
	}
};

} // namespace std

#endif