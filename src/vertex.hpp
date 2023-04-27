/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vertex.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eli <eli@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/26 15:25:04 by etran             #+#    #+#             */
/*   Updated: 2023/04/27 13:58:38 by eli              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VERTEX_HPP
# define VERTEX_HPP

// Graphics
# include <glm/glm.hpp>

struct Vertex {
	glm::vec2	pos;
	glm::vec3	color;

	static VkVertexInputBindingDescription	getBindingDescription() {
		// Passing data in readable format for Vulkan
		VkVertexInputBindingDescription	binding_description{};

		binding_description.binding = 0;
		binding_description.stride = sizeof(Vertex);
		binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return binding_description;
	}
};


#endif