#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;
layout(location = 2) in vec2 in_tex_coord;
layout(location = 3) in vec3 in_normal;

layout(location = 0) out vec3 frag_color;
layout(location = 1) out vec2 frag_tex_coord;
layout(location = 2) out vec3 pos_world;
layout(location = 3) out vec3 normal_world;

layout(binding = 0) uniform Camera {
	mat4 model;
	mat4 view;
	mat4 proj;
} camera_ubo;

void	main() {
	// Transform to world space
	pos_world = vec3(camera_ubo.model * vec4(in_position, 1.0));

	// Apply camera view
	gl_Position = camera_ubo.proj * camera_ubo.view * vec4(pos_world, 1.0);
	frag_color = in_color;
	frag_tex_coord = in_tex_coord;

	// Note: no need to inverse transpose model matrix
	// 		 because object scaling (zoom) is uniform.
	normal_world = normalize(mat3(camera_ubo.model) * in_normal);
}