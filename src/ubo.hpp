/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ubo.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/29 20:12:41 by eli               #+#    #+#             */
/*   Updated: 2023/04/29 20:13:20 by eli              ###   ########.fr       */
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
	glm::mat4	model;
	glm::mat4	view;
	glm::mat4	proj;
};

#endif