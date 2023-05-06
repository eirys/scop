#version 450

layout(location = 0) in vec3 frag_color;
layout(location = 1) in vec2 frag_tex_coord;

layout(location = 0) out vec4 out_color;

// Reference to combined image sampler descriptor
layout(binding = 1) uniform sampler2D tex_sampler;
layout(binding = 0) uniform UniformBufferObject {
	mat4 model;
	mat4 view;
	mat4 proj;
	bool is_textured;
} ubo;

void main() {
	out_color = ubo.is_textured ?
		texture(tex_sampler, frag_tex_coord) :
		vec4(frag_color, 1.0);
	// out_color = texture(tex_sampler, frag_tex_coord);
}