/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   matrix.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/19 23:18:11 by etran             #+#    #+#             */
/*   Updated: 2023/05/21 11:51:33 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "matrix.hpp"

#include <cstring> // memset

namespace scop {

/* ========================================================================== */
/*                                 4X4 MATRIX                                 */
/* ========================================================================== */

Mat4::Mat4() {
	memset(mat, 0.0f, 16 * sizeof(float));
}

Mat4::Mat4(const Mat4& other) {
	for (size_t i = 0; i < 16; ++i) {
		mat[i] = other.mat[i];
	}
}

Mat4::Mat4(float x) {
	for (size_t i = 0; i < 16; ++i) {
		if (i % 5 == 0)
			mat[i] = x;
		else
			mat[i] = 0;
	}
}

Mat4::Mat4(std::initializer_list<float> list) {
	if (list.size() != 16) {
		throw std::invalid_argument("Mat4 initializer list must have 16 elements");
	}
	for (size_t i = 0; i < list.size(); ++i) {
		mat[i] = *(list.begin() + i);
	}
}

Mat4&	Mat4::operator=(const Mat4& rhs) noexcept {
	for (size_t i = 0; i < 16; ++i) {
		mat[i] = rhs.mat[i];
	}
	return *this;
}

/* ACCESSORS =============================================================== */

float&	Mat4::operator[](size_t index) {
	if (index >= 16) {
		throw std::out_of_range("Matrix index out of range");
	}
	return mat[index];
}

const float&	Mat4::operator[](size_t index) const {
	if (index >= 16) {
		throw std::out_of_range("Matrix index out of range");
	}
	return mat[index];
}

/* OPERATORS =============================================================== */

Mat4&	Mat4::operator+=(const Mat4& rhs) noexcept {
	for (size_t i = 0; i < 16; i++) {
		mat[i] += rhs.mat[i];
	}
	return *this;
}

Mat4	Mat4::operator+(const Mat4& rhs) const noexcept {
	Mat4	result(*this);
	return result.operator+=(rhs);
}

Mat4&	Mat4::operator-=(const Mat4& rhs) noexcept {
	for (size_t i = 0; i < 16; i++) {
		mat[i] -= rhs.mat[i];
	}
	return *this;
}

Mat4	Mat4::operator-(const Mat4& rhs) const noexcept {
	Mat4	result(*this);
	return result.operator-=(rhs);
}

Mat4&	Mat4::operator*=(const Mat4& rhs) noexcept {
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

Mat4	Mat4::operator*(const Mat4& rhs) const noexcept {
	Mat4 result(*this);
	return result.operator*=(rhs);
}

/* ========================================================================= */

Mat4&	Mat4::operator*=(float rhs) noexcept {
	for (size_t i = 0; i < 16; i++) {
		mat[i] *= rhs;
	}
	return *this;
}

Mat4	Mat4::operator*(float rhs) const noexcept {
	Mat4 result(*this);
	return result.operator*=(rhs);
}

Vect3	Mat4::operator*(const Vect3& rhs) const noexcept {
	Vect3 result;

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

/**
 * @brief Compute the minor [row, column] of the matrix.
 *
 * @param row		The line to remove.
 * @param column	The column to remove.
*/
Mat3	Mat4::minor(size_t row, size_t column) const {
	Mat3	submatrix{};
	size_t	x = 0;
	size_t	y = 0;

	for (size_t line = 0; line < 4; ++line) {
		if (line != row) {
			for (size_t col = 0; col < 4; ++col) {
				if (col != column) {
					submatrix[y * 3 + x] = mat[line * 3 + col];
					++x;
				}
			}
			++y;
		}
		x = 0;
	}
	return submatrix;
}

/**
 * @brief Compute the determinant of a 4x4 matrix.
*/
float	Mat4::det() const {
	float	sum{};

	for (size_t j = 0; j < 4; ++j) {
		sum = std::fma(
			std::pow(-1.0f, j),
			std::fma(
				minor(0, j).det(),
				mat[j],
				0.0f
			),
			sum
		);
	}
	return sum;
}

Mat4	Mat4::adjugate() const {
	Mat4	cofactor{};

	for (size_t i = 0; i < 4; ++i) {
		for (size_t j = 0; j < 4; ++j) {
			cofactor[4 * i + j] = std::fma(
				std::pow(-1.0, i + j),
				minor(i, j).det(),
				0.0f
			);
		}
	}
	return cofactor.transpose();
}

Mat4	Mat4::transpose() const {
	Mat4	result{};

	for (size_t i = 0; i < 4; ++i) {
		for (size_t j = 0; j < 4; ++j) {
			result[4 * i + j] = mat[4 * j + i];
		}
	}
	return result;
}

/* ========================================================================== */
/*                                 MATRIX 3X3                                 */
/* ========================================================================== */

Mat3::Mat3() {
	memset(mat, 0, 9 * sizeof(float));
}

/* ACCESSORS ================================================================ */

float&	Mat3::operator[](size_t index) {
	if (index >= 9) {
		throw std::out_of_range("Mat3 index out of range");
	}
	return mat[index];
}

const float&	Mat3::operator[](size_t index) const {
	if (index >= 9) {
		throw std::out_of_range("Mat3 index out of range");
	}
	return mat[index];
}

/* ========================================================================== */

Mat2	Mat3::minor(size_t row, size_t column) const {
	Mat2	submatrix{};
	size_t	x = 0;
	size_t	y = 0;

	for (size_t line = 0; line < 3; ++line) {
		if (line != row) {
			for (size_t col = 0; col < 3; ++col) {
				if (col != column) {
					submatrix[2 * y + x] = mat[2 * line + col];
					++x;
				}
			}
			++y;
		}
		x = 0;
	}
	return submatrix;
}

float	Mat3::det() const {
	float	sum{};

	for (size_t j = 0; j < 3; ++j) {
		sum = std::fma(
			std::pow(-1.0f, j),
			std::fma(
				minor(0, j).det(),
				mat[j],
				0.0f
			),
			sum
		);
	}
	return sum;
}

/* ========================================================================== */
/*                                 MATRIX 2X2                                 */
/* ========================================================================== */

Mat2::Mat2() {
	memset(mat, 0, 4 * sizeof(float));
}

/* ACCESSORS ================================================================ */

float& Mat2::operator[](size_t index) {
	if (index >= 4) {
		throw std::out_of_range("Mat2 index out of range");
	}
	return mat[index];
}

const float& Mat2::operator[](size_t index) const {
	if (index >= 4) {
		throw std::out_of_range("Mat2 index out of range");
	}
	return mat[index];
}

/* ========================================================================== */

float	Mat2::det() const {
	return std::fma(mat[0], mat[3], -std::fma(mat[1], mat[2], 0.0f));
}

/* ========================================================================== */
/*                                    OTHER                                   */
/* ========================================================================== */

/**
 * @brief Produces rotation matrix around the given axis
 *
 * @param mat:		matrix to rotate
 * @param angle:	angle in radians
 * @param axis:		axis of rotation
*/
Mat4	rotate(const Mat4& mat, float angle, const Vect3& axis) noexcept {
	const float	c = std::cos(angle);
	const float	s = std::sin(angle);
	const Vect3	u = scop::normalize(axis);

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
	} * mat;
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
Mat4	lookAt(const Vect3& eye, const Vect3& center, const Vect3& up) noexcept {
	const Vect3	f = scop::normalize(center - eye);
	const Vect3	s = scop::normalize(scop::cross(f, up));
	const Vect3	u = scop::cross(s, f);

	return Mat4{
		// Row 1
		s.x, u.x, -f.x, 0,
		// Row 2
		s.y, u.y, -f.y, 0,
		// Row 3
		s.z, u.z, -f.z, 0,
		// Row 4
		scop::dot(-s, eye), scop::dot(-u, eye), scop::dot(f, eye), 1
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
Mat4	perspective(float fov, float aspect_ratio, float near, float far) noexcept {
	const float	tanHalfFov = std::tan(fov / 2);
	const float	range = far - near;

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
 * @note Only scales the first 3 rows of the matrix
*/
Mat4	scale(const Mat4& mat, const Vect3& scale) noexcept {
	Mat4	result(mat);

	for (size_t j = 0; j < 3; ++j) {
		for (size_t i = 0; i < 3; ++i) {
			result[j * 4 + i] *= scale[j];
		}
	}
	return result;
}

/**
 * @brief Produces a translation matrix from the given vector
 * 
 * @param mat	matrix to translate
 * @param dir	vector to translate by
*/
Mat4	translate(const Mat4& mat, const Vect3& dir) noexcept {
	Mat4	result(mat);

	result[3] = dir.x;
	result[7] = dir.y;
	result[11] = dir.z;
	return result;
}

/**
 * @brief Compute the inverse of a 4x4 matrix.
*/
Mat4	inverse(const Mat4& mat) {
	const float determinant = mat.det();
	if (!determinant) {
		// The matrix is singular.
		throw std::invalid_argument("Attempt to inverse a singular matrix.");
	}

	// Calculate the adjugate matrix.
	Mat4	adjugate = mat.adjugate();
	return adjugate * (1 / determinant);
}

} // namespace scop