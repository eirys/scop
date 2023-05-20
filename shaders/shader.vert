#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;
layout(location = 2) in vec2 in_tex_coord;

layout(location = 0) out vec3 frag_color;
layout(location = 1) out vec2 frag_tex_coord;

layout(binding = 0) uniform Camera {
	mat4 model;
	mat4 view;
	mat4 proj;
	mat4 zoom;
	vec3 translation;
} camera_ubo;

void	main() {
	gl_Position = (
		camera_ubo.proj
		* camera_ubo.view
		* camera_ubo.zoom
		* camera_ubo.model
		* vec4(in_position, 1.0)
	);
	frag_color = in_color;
	frag_tex_coord = in_tex_coord;
}