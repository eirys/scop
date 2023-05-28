/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/03 13:54:57 by eli               #+#    #+#             */
/*   Updated: 2023/05/28 21:34:48 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// Std
# include <cmath> // std::fma, std::sqrt
# include <stdexcept> // std::out_of_range
# include <ostream> // std::ostream

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

	const float&	operator[](std::size_t index) const {
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

	float&	operator[](std::size_t index) {
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

	bool	operator!() const noexcept {
		return !x && !y && !z;
	}

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

	/* BOOLEAN COMPARISON ====================================================== */

	bool	operator==(const Vect3& rhs) const noexcept {
		return x == rhs.x && y == rhs.y && z == rhs.z;
	}

}; // struct Vect3

/**
 * @brief Returns the dot product of the vector with another vector
 */
inline float	dot(const Vect3& lhs, const Vect3& rhs) noexcept {
	return static_cast<float>(
		std::fma(lhs.x, rhs.x, std::fma(lhs.y, rhs.y, std::fma(lhs.z, rhs.z, 0)))
	);
}

/**
 * @brief Returns the norm of the vector
*/
inline float	norm(const Vect3& vec) {
	return std::sqrt(dot(vec, vec));
}

/**
 * @brief Returns a normalized vector
*/
inline Vect3	normalize(const Vect3& vec) {
	float	n = norm(vec);
	return Vect3{ vec.x / n, vec.y / n, vec.z / n };
}

/**
 * @brief Returns the cross product of the vector with another vector
*/
inline Vect3	cross(const Vect3& lhs, const Vect3& rhs) noexcept {
	return Vect3{
		static_cast<float>(std::fma(lhs.y, rhs.z, std::fma(-lhs.z, rhs.y, 0))),
		static_cast<float>(std::fma(lhs.z, rhs.x, std::fma(-lhs.x, rhs.z, 0))),
		static_cast<float>(std::fma(lhs.x, rhs.y, std::fma(-lhs.y, rhs.x, 0)))
	};
}

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

	float&	operator[](std::size_t index) {
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

	bool	operator!() const noexcept {
		return !x && !y;
	}

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

/* ========================================================================== */
/*                                    OTHER                                   */
/* ========================================================================== */

/* HASH FUNCTIONS =========================================================== */

template<>
struct std::hash<scop::Vect3> {
	inline std::size_t	operator()(const scop::Vect3& vect) const {
		return (
			std::hash<float>()(vect.x) ^
			std::hash<float>()(vect.y) ^
			std::hash<float>()(vect.z)
		);
	}
};

template<>
struct std::hash<scop::Vect2> {
	inline std::size_t	operator()(const scop::Vect2& vect) const {
		return (
			std::hash<float>()(vect.x) ^
			std::hash<float>()(vect.y)
		);
	}
};

/* IOSTREAM ================================================================= */

inline std::ostream& operator<<(std::ostream& os, const scop::Vect3& vect) {
	os << "Vect3(" << vect.x << ", " << vect.y << ", " << vect.z << ")";
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const scop::Vect2& vect) {
	os << "Vect2(" << vect.x << ", " << vect.y << ")";
	return os;
}