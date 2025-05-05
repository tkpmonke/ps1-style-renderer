#pragma once

typedef struct shader_t {
	const char* vertex;
	const char* fragment;
	unsigned int program;
} shader_t;

void compile_shader(shader_t* shader);
