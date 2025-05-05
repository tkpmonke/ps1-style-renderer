#version 330 core
in vec3 color;
noperspective in vec2 uv;
out vec4 out_color;
void main() {
	vec3 o = floor((vec3(uv, color.z)+1)/2*31)/31;
	out_color = vec4(o,1);
}
