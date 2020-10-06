#version 410

uniform samplerCube skybox;
uniform vec3 light_position;
uniform vec3 camera_position;

in VS_OUT {
	vec3 vertex;
	vec3 normal_water;
	vec3 tangent_water;
	vec3 binormal_water;
} fs_in;

out vec4 frag_color;

void main()
{
	vec3 V = normalize(camera_position - fs_in.vertex);
	vec3 L = normalize(light_position - fs_in.vertex);

	// Color:
	vec3 n = fs_in.normal_water;
	n = normalize(n);

	vec4 color_deep = vec4(0.0, 0.0, 0.1, 1.0);
	vec4 color_shallow = vec4(0.0, 0.5, 0.5, 1.0);

	float facing = 1 - max(dot(V, n), 0.0);
	vec4 water_color = mix(color_deep, color_shallow, facing);

	// Reflection:
	vec3 R = normalize(reflect(-V, n));
	vec4 reflection_color = texture(skybox, R);

	// Animated Normal mapping:


	// Fresnel reflection:


	// Refraction:

	frag_color = water_color + reflection_color;
}
