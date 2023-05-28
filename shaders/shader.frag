#version 450

layout(location = 0) in vec3 frag_color;
layout(location = 1) in vec2 frag_tex_coord;
layout(location = 2) in vec3 lighting;

layout(location = 0) out vec4 out_color;

// Reference to combined image sampler descriptor
layout(binding = 1) uniform sampler2D tex_sampler;
layout(binding = 2) uniform Texture {
	int		state;
	float	mix;
} texture_ubo;

const vec3 _gray_scale = vec3(0.7, 0.7, 0.7);

void main() {
	vec4	light_pos = vec4(0.0, 10.0, 10.0, 1.0);
	vec4	input_color; // Current color
	vec4	output_color; // Next color
	
	if (texture_ubo.state == 1) { // From texture to color
		input_color = texture(tex_sampler, frag_tex_coord);
		output_color = vec4(frag_color, 1.0);
	} else if (texture_ubo.state == 2) { // From color to grayscale
		input_color = vec4(frag_color, 1.0);
		output_color = vec4(_gray_scale, 1.0);
	} else { // From grayscale to texture
		input_color = vec4(_gray_scale, 1.0);
		output_color = texture(tex_sampler, frag_tex_coord);
	}

	if (texture_ubo.mix != -1.0 && texture_ubo.mix <= 1.0) {
		out_color = mix(input_color, output_color, texture_ubo.mix);
	} else {
		out_color = output_color;
	}
	out_color *= vec4(lighting, 1.0);
}