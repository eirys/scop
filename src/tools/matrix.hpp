/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   matrix.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/03 14:11:57 by eli               #+#    #+#             */
/*   Updated: 2023/05/27 01:18:04 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// Std
# include <stdexcept>
# include <cstring>

# include "vector.hpp"

namespace scop {
struct Mat2;	// Forward declaration
struct Mat3;

struct Mat4 {
	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	float	mat[16];

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	Mat4();
	Mat4(const Mat4& other);
	Mat4(float x);
	Mat4(std::initializer_list<float> list);
	Mat4&	operator=(const Mat4& rhs) noexcept;

	~Mat4() = default;

	/* ACCESSORS =============================================================== */

	float&	operator[](std::size_t index);
	const float&	operator[](std::size_t index) const;

	/* OPERATORS =============================================================== */

	Mat4&	operator+=(const Mat4& rhs) noexcept;
	Mat4	operator+(const Mat4& rhs) const noexcept;
	Mat4&	operator-=(const Mat4& rhs) noexcept;
	Mat4	operator-(const Mat4& rhs) const noexcept;
	Mat4&	operator*=(const Mat4& rhs) noexcept;
	Mat4	operator*(const Mat4& rhs) const noexcept;

	/* ========================================================================= */

	Mat4&	operator*=(float rhs) noexcept;
	Mat4	operator*(float rhs) const noexcept;
	Vect3	operator*(const Vect3& rhs) const noexcept;

	Mat3	minor(std::size_t row, std::size_t col) const;
	float	det() const;
	Mat4	adjugate() const;
	Mat4	transpose() const;

}; // struct Mat4

Mat4	lookAt(const Vect3& eye, const Vect3& center, const Vect3& up) noexcept;
Mat4	perspective(float fov, float aspect_ratio, float near, float far) noexcept;
Mat4	rotate(const Mat4& mat, float angle, const Vect3& axis) noexcept;
Mat4	scale(const Mat4& mat, const Vect3& scale) noexcept;
Mat4	translate(const Mat4& mat, const Vect3& dir) noexcept;
Mat4	inverse(const Mat4& mat);

/* ========================================================================== */
/*                                 UTILITARIES                                */
/* ========================================================================== */

struct Mat3 {

	float	mat[9];

	Mat3();

	Mat3(const Mat3& other) = default;
	Mat3(Mat3&& other) = default;
	~Mat3() = default;

	/* ACCESSORS =============================================================== */

	float&	operator[](std::size_t index);
	const float&	operator[](std::size_t index) const;

	/* ========================================================================= */

	Mat2	minor(std::size_t row, std::size_t col) const;
	float	det() const;

}; // struct Mat3

struct Mat2 {

	float	mat[4];

	Mat2();

	Mat2(const Mat2& other) = default;
	Mat2(Mat2&& other) = default;
	~Mat2() = default;

	/* ACCESSORS =============================================================== */

	float&	operator[](std::size_t index);
	const float&	operator[](std::size_t index) const;

	/* ========================================================================= */

	float	det() const;

}; // struct Mat2

}; // namespace scop