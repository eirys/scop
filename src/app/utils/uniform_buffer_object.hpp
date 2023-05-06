/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uniform_buffer_object.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/29 20:12:41 by eli               #+#    #+#             */
/*   Updated: 2023/05/06 21:15:04 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UBO_HPP
# define UBO_HPP

# include "matrix.hpp"

struct UniformBufferObject {
	alignas(16) scop::Mat4	model;
	alignas(16) scop::Mat4	view;
	alignas(16) scop::Mat4	proj;
	bool					is_textured;
};

#endif