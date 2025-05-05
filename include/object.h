#pragma once
#include <cglm/cglm.h>

#include "model.h"
#include "shader.h"
#include "camera.h"
#include "rigidbody.h"

#define OBJECT_T_DEFAULT()	\
	(object_t) {	\
		.position = {0,0,0},	\
		.rotation = {0,0,0},	\
		.scale = {1,1,1},	\
		.model = NULL,	\
		.shader = NULL	\
	}

typedef struct object_t {
	vec3 position, rotation, scale;

	model_t* model;
	shader_t* shader;
	rigidbody_t* rigidbody;
} object_t;

void draw_object(object_t*, camera_t*);
