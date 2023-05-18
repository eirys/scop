/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   matrix.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/03 14:11:57 by eli               #+#    #+#             */
/*   Updated: 2023/05/17 17:58:44 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MATRIX_HPP
# define MATRIX_HPP

// Std
# include <stdexcept>
# include <cstring>

# include "vector.hpp"

namespace scop {

struct Mat4 {
	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	float	mat[16];

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	Mat4() {
		memset(mat, 0.0f, 16 * sizeof(float));
	}

	Mat4(const Mat4& other) {
		for (size_t i = 0; i < 16; ++i) {
			mat[i] = other.mat[i];
		}
	}

	Mat4(float x) {
		for (size_t i = 0; i < 16; ++i) {
			if (i % 5 == 0)
				mat[i] = x;
			else
				mat[i] = 0;
		}
	}

	Mat4(std::initializer_list<float> list) {
		if (list.size() != 16) {
			throw std::invalid_argument("Mat4 initializer list must have 16 elements");
		}
		for (size_t i = 0; i < list.size(); ++i) {
			mat[i] = *(list.begin() + i);
		}
	}

	Mat4&	operator=(const Mat4& rhs) noexcept {
		for (size_t i = 0; i < 16; ++i) {
			mat[i] = rhs.mat[i];
		}
		return *this;
	}

	~Mat4() = default;

	/* ACCESSORS =============================================================== */

	float&	operator[](size_t index) {
		if (index >= 16) {
			throw std::out_of_range("Matrix index out of range");
		}
		return mat[index];
	}

	const float&	operator[](size_t index) const {
		if (index >= 16) {
			throw std::out_of_range("Matrix index out of range");
		}
		return mat[index];
	}

	/* OPERATORS =============================================================== */

	Mat4&	operator+=(const Mat4& rhs) noexcept {
		for (size_t i = 0; i < 16; i++) {
			mat[i] += rhs.mat[i];
		}
		return *this;
	}

	Mat4	operator+(const Mat4& rhs) const noexcept {
		Mat4	result(*this);
		return result.operator+=(rhs);
	}

	Mat4&	operator-=(const Mat4& rhs) noexcept {
		for (size_t i = 0; i < 16; i++) {
			mat[i] -= rhs.mat[i];
		}
		return *this;
	}

	Mat4	operator-(const Mat4& rhs) const noexcept {
		Mat4	result(*this);
		return result.operator-=(rhs);
	}

	Mat4&	operator*=(const Mat4& rhs) noexcept {
		Mat4	result;
		for (size_t i = 0; i < 4; i++) {
			for (size_t j = 0; j < 4; j++) {
				for (size_t k = 0; k < 4; k++) {
					result[i * 4 + j] = static_cast<float>(
						std::fma(mat[i * 4 + k], rhs.mat[k * 4 + j], result[i * 4 + j])
					);
				}
			}
		}
		*this = result;
		return *this;
	}

	Mat4	operator*(const Mat4& rhs) const noexcept {
		Mat4 result(*this);
		return result.operator*=(rhs);
	}

	/* ========================================================================= */

	Mat4&	operator*=(float rhs) noexcept {
		for (size_t i = 0; i < 16; i++) {
			mat[i] *= rhs;
		}
		return *this;
	}

	Mat4	operator*(float rhs) const noexcept {
		Mat4 result(*this);
		return result.operator*=(rhs);
	}

	Vect3	operator*(const Vect3& rhs) const noexcept {
		Vect3 result;
		// result.x = mat[0] * rhs.x + mat[1] * rhs.y + mat[2] * rhs.z + mat[3];
		// result.y = mat[4] * rhs.x + mat[5] * rhs.y + mat[6] * rhs.z + mat[7];
		// result.z = mat[8] * rhs.x + mat[9] * rhs.y + mat[10] * rhs.z + mat[11];
		// w component of Vect4 ignored (assumed to be 1)
		result.x = static_cast<float>(
			std::fma(
				mat[0],
				rhs.x,
				std::fma(mat[1], rhs.y, std::fma(mat[2], rhs.z, mat[3]))
			)
		);
		result.y = static_cast<float>(
			std::fma(
				mat[4],
				rhs.x,
				std::fma(mat[5], rhs.y, std::fma(mat[6], rhs.z, mat[7]))
			)
		);
		result.z = static_cast<float>(
			std::fma(
				mat[8],
				rhs.x,
				std::fma(mat[9], rhs.y, std::fma(mat[10], rhs.z, mat[11]))
			)
		);
		return result;
	}

}; // struct Mat4

/**
 * @brief Produces rotation matrix around the given axis
 *
 * @param angle:	angle in radians
 * @param axis:		axis of rotation
*/
inline Mat4	rotate(float angle, const Vect3& axis) noexcept {
	float	c = std::cos(angle);
	float	s = std::sin(angle);
	Vect3	u = axis.normalize();

	return Mat4{
		// Row 1
		// u.x * u.x * (1 - c) + c,
		static_cast<float>(std::fma(u.x, std::fma(u.x, 1 - c, 0), c)),
		// u.x * u.y * (1 - c) - u.z * s,
		static_cast<float>(std::fma(u.x, std::fma(u.y, 1 - c, 0), std::fma(-u.z, s, 0))),
		// u.x * u.z * (1 - c) + u.y * s,
		static_cast<float>(std::fma(u.x, std::fma(u.z, 1 - c, 0), std::fma(u.y, s, 0))),
		0,
		// Row 2
		// u.y * u.x * (1 - c) + u.z * s,
		static_cast<float>(std::fma(u.y, std::fma(u.x, 1 - c, 0), std::fma(u.z, s, 0))),
		// u.y * u.y * (1 - c) + c,
		static_cast<float>(std::fma(u.y, std::fma(u.y, 1 - c, 0), c)),
		// u.y * u.z * (1 - c) - u.x * s,
		static_cast<float>(std::fma(u.y, std::fma(u.z, 1 - c, 0), std::fma(-u.x, s, 0))),
		0,
		// Row 3
		// u.z * u.x * (1 - c) - u.y * s,
		static_cast<float>(std::fma(u.z, std::fma(u.x, 1 - c, 0), std::fma(-u.y, s, 0))),
		// u.z * u.y * (1 - c) + u.x * s,
		static_cast<float>(std::fma(u.z, std::fma(u.y, 1 - c, 0), std::fma(u.x, s, 0))),
		// u.z * u.z * (1 - c) + c,
		static_cast<float>(std::fma(u.z, std::fma(u.z, 1 - c, 0), c)),
		0,
		// Row 4
		0, 0, 0, 1
	};
}

/**
 * @brief Produces lookAt matrix
 *
 * @param eye:		position of the camera
 * @param center:	position of the object to look at
 * @param up:		up vector, usually (0, 0, 1). Used to determine
 * 					orientation of the camera. Should not be parallel
 * 					to the vector from eye to center.
*/
inline Mat4	lookAt(const Vect3& eye, const Vect3& center, const Vect3& up) noexcept {
	Vect3	f = (center - eye).normalize();
	Vect3	s = f.cross(up).normalize();
	Vect3	u = s.cross(f);

	return Mat4{
		// Row 1
		s.x, u.x, -f.x, 0,
		// Row 2
		s.y, u.y, -f.y, 0,
		// Row 3
		s.z, u.z, -f.z, 0,
		// Row 4
		-s.dot(eye), -u.dot(eye), f.dot(eye), 1
	};
}

/**
 * @brief Produces orthographic projection matrix
 *
 * @param fov			field of view in radians
 * @param aspect_ratio	aspect ratio of the screen
 * @param near			near clipping plane
 * @param far			far clipping plane
*/
inline Mat4	perspective(float fov, float aspect_ratio, float near, float far) noexcept {
	float	tanHalfFov = std::tan(fov / 2);
	float	range = far - near;

	return Mat4{
		// Row 1
		1 / (aspect_ratio * tanHalfFov), 0, 0, 0,
		// Row 2
		0, 1 / tanHalfFov, 0, 0,
		// Row 3
		0, 0, -(far + near) / range, -1,
		// Row 4
		0, 0, -2 * far * near / range, 0
	};
}

/**
 * @brief Scales the matrix by the given vector
 *
 * @param mat:		matrix to scale
 * @param scale:	vector to scale by
 *
 * @note			only scales the first 3 rows of the matrix
*/
inline Mat4	scale(const Mat4& mat, const Vect3& scale) noexcept {
	Mat4	result(mat);

	for (size_t j = 0; j < 3; ++j) {
		for (size_t i = 0; i < 3; ++i) {
			result[j * 4 + i] *= scale[j];
		}
	}
	return result;
}

}; // namespace scop

#endif