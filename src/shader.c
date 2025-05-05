#include "shader.h"
#include "camera.h"

void compile_shader(shader_t* shader) {
	char info_log[512];
	int success;

	unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &shader->vertex, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, info_log);
		printf("Vertex Shader Failed To Compile > %s", info_log);
	}

	unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &shader->fragment, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, info_log);
		printf("Fragment Shader Failed To Compile > %s", info_log);
	}
	
	shader->program = glCreateProgram();
	glAttachShader(shader->program, vertex);
	glAttachShader(shader->program, fragment);
	glLinkProgram(shader->program);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}
