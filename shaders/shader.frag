#version 450

layout(location = 0) in vec3 frag_color;
layout(location = 1) in vec2 frag_tex_coord;

layout(location = 0) out vec4 out_color;

// Reference to combined image sampler descriptor
layout(binding = 1) uniform sampler2D tex_sampler;
layout(binding = 2) uniform UniformBufferObject {
	bool	texture_enabled;
	float	texture_mix;
} ubo;

void main() {
	vec4	tex_color = texture(tex_sampler, frag_tex_coord);
	vec4	input_color = vec4(frag_color, 1.0);

	if (ubo.texture_mix != -1.0 || ubo.texture_mix > 1.0) {
		out_color = mix(input_color, tex_color, ubo.texture_mix);
	} else if (ubo.texture_enabled) {
		out_color = tex_color;
	} else {
		out_color = input_color;
	}
}