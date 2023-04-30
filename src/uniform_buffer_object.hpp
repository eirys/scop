/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uniform_buffer_object.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/29 20:12:41 by eli               #+#    #+#             */
/*   Updated: 2023/04/30 21:58:02 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UBO_HPP
# define UBO_HPP

// Graphics
# ifndef GLFW_INCLUDE_VULKAN
#  define GLFW_INCLUDE_VULKAN
# endif
# include <GLFW/glfw3.h>
# include <glm/glm.hpp>

struct UniformBufferObject {
	alignas(16) glm::mat4	model;
	alignas(16) glm::mat4	view;
	alignas(16) glm::mat4	proj;
};

#endif