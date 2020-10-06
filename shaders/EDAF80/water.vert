#version 410

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;

uniform mat4 vertex_model_to_world;
uniform mat4 normal_model_to_world;
uniform mat4 vertex_world_to_clip;

out VS_OUT {
	vec3 vertex;
	vec3 normal;
} vs_out;

uniform	float amplitude[2] = { 1.0, 0.5 };
uniform	float frequency[2] = { 0.2, 0.4 };
uniform	float phase[2] = { 0.5, 1.3 };
uniform	float sharpness[2] = { 2.0, 2.0 };
uniform	vec2 direction[2] = { vec2(-1.0, 0.0), vec2(-0.7, 0.7) };
uniform float ellapsed_time;

float wave(vec2 position, vec2 direction, float amplitude, float frequency, float phase, float sharpness, float time){
	return amplitude * pow(sin(dot(position, direction) * frequency + time * phase) * 0.5 + 0.5, sharpness);
}

void main()
{

	vec3 displaced_vertex = vertex;
	displaced_vertex.y += wave(vertex.xz, direction[0], amplitude[0], frequency[0], phase[0], sharpness[0], ellapsed_time);
	displaced_vertex.y += wave(vertex.xz, direction[1], amplitude[1], frequency[1], phase[1], sharpness[1], ellapsed_time);

	vs_out.vertex = vec3(vertex_model_to_world * vec4(displaced_vertex, 1.0));
	vs_out.normal = vec3(normal_model_to_world * vec4(normal, 0.0));

	gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(displaced_vertex, 1.0);
}
