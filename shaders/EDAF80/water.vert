#version 410

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 texcoord;

uniform mat4 vertex_model_to_world;
uniform mat4 normal_model_to_world;
uniform mat4 vertex_world_to_clip;

out VS_OUT {
	vec3 vertex;
	vec3 normal_water;
	vec3 tangent_water;
	vec3 binormal_water;

	vec2 normalCoord0;
	vec2 normalCoord1;
	vec2 normalCoord2;
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

float derivative(vec2 position, vec2 direction, float amplitude, float frequency, float phase, float sharpness, float time){
	return 0.5 * sharpness * frequency * amplitude * (pow(sin(dot(direction, position) * frequency + time * phase) * 0.5 + 0.5, sharpness - 1)) * cos(dot(direction, position) * frequency + time * phase);
}

void main() {

	vec3 displaced_vertex = vertex;

	displaced_vertex.y += wave(vertex.xz, direction[0], amplitude[0], frequency[0], phase[0], sharpness[0], ellapsed_time);
	displaced_vertex.y += wave(vertex.xz, direction[1], amplitude[1], frequency[1], phase[1], sharpness[1], ellapsed_time);

	float dHdx = 0.0f;
	float dHdz = 0.0f;

	dHdx = derivative(vertex.xz, direction[0], amplitude[0], frequency[0], phase[0], sharpness[0], ellapsed_time) * direction[0].x + 
		derivative(vertex.xz, direction[1], amplitude[1], frequency[1], phase[1], sharpness[1], ellapsed_time) * direction[1].x;
	
	dHdz = derivative(vertex.xz, direction[0], amplitude[0], frequency[0], phase[0], sharpness[0], ellapsed_time) * direction[0].y + 
		derivative(vertex.xz, direction[1], amplitude[1], frequency[1], phase[1], sharpness[1], ellapsed_time) * direction[1].y;

	vec2 texScale		= vec2(8,4);
	float normalTime	= mod(ellapsed_time, 100.0);
	vec2 normalSpeed	= vec2(-0.05, 0);
	

	vs_out.vertex = vec3(vertex_model_to_world * vec4(displaced_vertex, 1.0));
	vs_out.normal_water = vec3(-dHdx, 1.0f, -dHdz);
	vs_out.binormal_water = vec3(0.0, dHdz, 1.0);
	vs_out.tangent_water = vec3(1.0, dHdx, 0.0);

	vs_out.normalCoord0 = vec2(texcoord.xy*texScale + normalTime*normalSpeed);
	vs_out.normalCoord1 = vec2(texcoord.xy*texScale*2 + normalTime*normalSpeed*4);
	vs_out.normalCoord2 = vec2(texcoord.xy*texScale*4 + normalTime*normalSpeed*8);

	gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(displaced_vertex, 1.0);
}
