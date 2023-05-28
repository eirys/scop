#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;
layout(location = 2) in vec2 in_tex_coord;
layout(location = 3) in vec3 in_normal;

layout(location = 0) out vec3 frag_color;
layout(location = 1) out vec2 frag_tex_coord;
layout(location = 2) out vec4 lighting;

layout(binding = 0) uniform Camera {
	mat4 model;
	mat4 view;
	mat4 proj;
} camera_ubo;

// Light properties
const float _ambient_component = 0.01;
const vec3 _light_point = vec3(1.0, 1.5, 2.0);
const vec4 _light_color = vec4(1.0, 1.0, 0.7, 1.0); // yellowish, w is intensity

void	main() {
	// Transform to world space
	vec3 pos_world = vec3(camera_ubo.model * vec4(in_position, 1.0));

	// Calculate normal in world space (model space -> world space)
	// Note: no need to inverse transpose model matrix
	// 		 because object scaling (zoom) is uniform.
	vec3 normal = normalize(mat3(camera_ubo.model) * in_normal);

	// Compute vertex to light vector
	vec3 light_vector = _light_point - pos_world;

	// Compute atenuation (using inverse square law)
	float light_attenuation = 1.0f / dot(light_vector, light_vector);

	// Compute diffuse light component
	vec3 diffuse_component = (
		vec3(_light_color.xyz * _light_color.w)
		* light_attenuation
		* max(dot(normal, normalize(light_vector)), 0.0)
	);

	gl_Position = camera_ubo.proj * camera_ubo.view * vec4(pos_world, 1.0);

	frag_color = in_color;
	frag_tex_coord = in_tex_coord;
	lighting = vec4(diffuse_component, _ambient_component);
}