#pragma once
#include "cglm/cglm.h"

typedef struct vertex_t {
	vec3 position;
	vec3 normal;
	vec2 uv;
} vertex_t;

typedef struct model_t {
	vertex_t* vertices;
	unsigned int vertex_count;

	unsigned int* indices;
	unsigned int indice_count;

	unsigned int vao, vbo, ebo;
} model_t;

void create_model(model_t*);
uint8_t load_model(const char* path, model_t* model);
