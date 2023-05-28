#version 450

layout(location = 0) in vec3 frag_color;
layout(location = 1) in vec2 frag_tex_coord;
layout(location = 2) in vec3 pos_world;
layout(location = 3) in vec3 normal_world;

layout(location = 0) out vec4 out_color;

layout(binding = 1) uniform sampler2D tex_sampler;
layout(binding = 2) uniform Texture {
	int		state;
	float	parameter;
} texture_ubo;
layout(binding = 3) uniform Light {
	vec3 ambient_color;
	vec3 spot_position;
	vec3 spot_color;
	vec3 diffuse_color;
	vec3 eye_position;
	vec3 specular_color;
	int shininess;
} light_ubo;

// const vec3 _light_color = vec3(1.0, 1.0, 0.8);
const vec4 _gray_scale = vec4(0.7, 0.7, 0.7, 1.0);

void main() {
	vec4	input_color; // Current color
	vec4	output_color; // Next color

	if (texture_ubo.state == 1) {
		// From texture to color
		input_color = texture(tex_sampler, frag_tex_coord);
		output_color = vec4(frag_color, 1.0);
	} else if (texture_ubo.state == 2) {
		// From color to grayscale
		input_color = vec4(frag_color, 1.0);
		output_color = _gray_scale;
	} else {
		// From grayscale to texture
		input_color = _gray_scale;
		output_color = texture(tex_sampler, frag_tex_coord);
	}

	if (texture_ubo.parameter != -1.0 && texture_ubo.parameter <= 1.0) {
		// In transition
		output_color = mix(
			input_color,
			output_color,
			texture_ubo.parameter
		);
	}

	// Apply lighting
 	out_color = output_color * vec4(light_ubo.ambient_color, 1.0);

	// Vertex to light vector
	vec3 light_vector = light_ubo.spot_position - pos_world;
	// Atenuation due to distance
	float light_attenuation = 1.0f / dot(light_vector, light_vector);
	light_vector = light_vector * sqrt(light_attenuation);
	// Vector to eye
	vec3 eye_vector = normalize(light_ubo.eye_position - pos_world);
	vec3 perfect_reflection = reflect(-light_vector, normal_world);

	// Amount of diffuse light
	float diffuse_component = max(dot(normal_world, light_vector), 0.0);
	vec3 diffuse_lighting =
		light_ubo.spot_color
		* light_ubo.diffuse_color
		* light_attenuation
		* diffuse_component;

	// Amount of specular light
	float specular_component = max(
		pow(dot(eye_vector, perfect_reflection), light_ubo.shininess),
		0.0
	);
	vec3 specular_lighting =
		light_ubo.spot_color
		* light_ubo.specular_color
		* light_attenuation
		* specular_component;

	out_color +=
		output_color
		* vec4(
			clamp(diffuse_lighting + specular_lighting, vec3(0.0), vec3(1.0)),
			1.0
		);
}