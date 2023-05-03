/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   matrix.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/03 14:11:57 by eli               #+#    #+#             */
/*   Updated: 2023/05/03 14:17:32 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MATRIX_HPP
# define MATRIX_HPP

namespace scop {

struct Mat4 {

	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	float	mat[16];

	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	/* CONSTRUCTOR ============================================================= */

	Mat4() {
		for (size_t i = 0; i < 16; i++) {
			mat[i] = 0;
		}
	}

	Mat4(float x[16]) {
		for (size_t i = 0; i < 16; i++) {
			mat[i] = x[i];
		}
	}

	Mat4(float x) {
		for (size_t i = 0; i < 16; i++) {
			mat[i] = x;
		}
	}

	/* ACCESSORS =============================================================== */

	float& operator[](size_t index) {
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

};

}; // namespace scop

#endif