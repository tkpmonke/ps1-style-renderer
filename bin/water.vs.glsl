#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_uv;
layout (location = 2) in vec2 in_normal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform float u_time;

out vec2 uv;
out vec3 color;

float rand(vec2 n) {
	return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

const float scale = 0.0075f;
void main() {
	vec3 pos = vec3(0, sin(rand(in_pos.xz)*u_time)*scale, 0) + in_pos;
	gl_Position = u_projection*u_view*u_model*vec4(pos, 1);
	color = pos;
	uv = in_uv;
}
