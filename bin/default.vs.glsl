#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_uv;
layout (location = 2) in vec2 in_normal;
uniform vec2 u_screen_res;
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
out vec2 uv;
out vec3 color;
void main() {
	gl_Position = u_projection*u_view*u_model*vec4(in_pos, 1);
	color = in_pos;
	uv = in_uv;
}
