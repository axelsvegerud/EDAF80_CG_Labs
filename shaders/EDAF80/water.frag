#version 410

uniform sampler2D normal_map;
uniform samplerCube skybox;
uniform vec3 light_position;
uniform vec3 camera_position;
uniform mat4 normal_model_to_world;

in VS_OUT {
	vec3 vertex;
	vec3 normal_water;
	vec3 tangent_water;
	vec3 binormal_water;

	vec2 normalCoord0;
	vec2 normalCoord1;
	vec2 normalCoord2;
} fs_in;

out vec4 frag_color;

void main()
{
	vec3 V = normalize(camera_position - fs_in.vertex);
	vec3 L = normalize(light_position - fs_in.vertex);

	vec3 n = fs_in.normal_water;
	n = normalize(n);

	// Animated Normal mapping:
	mat3 TBN = mat3(normalize(fs_in.tangent_water), normalize(fs_in.binormal_water), normalize(fs_in.normal_water));

	vec3 n0	= texture(normal_map, fs_in.normalCoord0).rgb * 2 - 1; 
	vec3 n1	= texture(normal_map, fs_in.normalCoord1).rgb * 2 - 1; 
	vec3 n2	= texture(normal_map, fs_in.normalCoord2).rgb * 2 - 1;

	vec3 n_ripple = normalize(n0 + n1 + n2);

	n_ripple = TBN * n_ripple;

	n = normalize(vec3(normal_model_to_world * vec4(n_ripple, 0.0)));

	// Reflection:
	vec3 R = normalize(reflect(-V, n));
	vec4 reflection_color = texture(skybox, R);

	// Fresnel reflection: 
	float r0 = 0.02037;
	float fastFresnel = r0 + (1 - r0) * pow(1 - dot(V, n), 5);

	// Refraction:

	// Color:
	vec4 color_deep = vec4(0.0, 0.0, 0.1, 1.0);
	vec4 color_shallow = vec4(0.0, 0.5, 0.5, 1.0);

	float facing = 1 - max(dot(V, n), 0.0);
	vec4 water_color = mix(color_deep, color_shallow, facing);

	frag_color = water_color + reflection_color * fastFresnel;
}
