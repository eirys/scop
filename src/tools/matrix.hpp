/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   matrix.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/03 14:11:57 by eli               #+#    #+#             */
/*   Updated: 2023/05/12 23:31:31 by etran            ###   ########.fr       */
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
		bzero(mat, 16 * sizeof(float));
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

	Mat4&	operator=(const Mat4& rhs) {
		for (size_t i = 0; i < 16; ++i) {
			mat[i] = rhs.mat[i];
		}
		return *this;
	}

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

	Mat4	operator+(const Mat4& rhs) const {
		Mat4 result;
		for (size_t i = 0; i < 16; i++) {
			result[i] = mat[i] + rhs.mat[i];
		}
		return result;
	}

	Mat4	operator-(const Mat4& rhs) const {
		Mat4 result;
		for (size_t i = 0; i < 16; i++) {
			result[i] = mat[i] - rhs.mat[i];
		}
		return result;
	}

	Mat4	operator*(const Mat4& rhs) const {
		Mat4 result;
		for (size_t i = 0; i < 4; i++) {
			for (size_t j = 0; j < 4; j++) {
				for (size_t k = 0; k < 4; k++) {
					result[i * 4 + j] += mat[i * 4 + k] * rhs.mat[k * 4 + j];
				}
			}
		}
		return result;
	}

}; // struct Mat4

/**
 * @brief Produces rotation matrix around the given axis
 *
 * @param angle:	angle in radians
 * @param axis:		axis of rotation
*/
inline Mat4	rotate(float angle, const Vect3& axis) {
	float	c = std::cos(angle);
	float	s = std::sin(angle);
	Vect3	u = axis.normalize();

	return Mat4{
		// Row 1
		u.x * u.x * (1 - c) + c,
		u.x * u.y * (1 - c) - u.z * s,
		u.x * u.z * (1 - c) + u.y * s,
		0,
		// Row 2
		u.y * u.x * (1 - c) + u.z * s,
		u.y * u.y * (1 - c) + c,
		u.y * u.z * (1 - c) - u.x * s,
		0,
		// Row 3
		u.z * u.x * (1 - c) - u.y * s,
		u.z * u.y * (1 - c) + u.x * s,
		u.z * u.z * (1 - c) + c,
		0,
		// Row 4
		0, 0, 0, 1
	};
}

/**
 * @brief Produces lookAt matrix
 * 
 * @param eye:		position of the camera (eye)
 * @param center:	position of the object to look at
 * @param up:		up vector, usually (0, 0, 1)
*/
inline Mat4	lookAt(const Vect3& eye, const Vect3& center, const Vect3& up) {
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
*/
inline Mat4	perspective(float fov, float aspect_ratio, float near, float far) {
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

}; // namespace scop

#endif