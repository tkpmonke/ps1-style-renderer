#pragma once
#include <cglm/cglm.h>
#include <GL/glew.h>

#include "window.h"

#define CAMERA_T_DEFAULT()	\
(camera_t){	\
	.position = {0,0,0},	\
	.rotation = {0,0,0},	\
	.forward = {0,0,0},	\
	.right = {0,0,0},	\
	.up = {0,0,0},	\
	.fov = 75,	\
	.min = 0.1f,	\
	.max = 100,	\
	.framebuffer = {	\
		.width = 320,	\
		.height = 200,	\
		.fbo = 0,	\
		.tex = 0,	\
		.depth = 0	\
	},	\
	.matrices = {	\
		.view = GLM_MAT4_IDENTITY_INIT,	\
		.projection = GLM_MAT4_IDENTITY_INIT	\
	}	\
}

typedef struct camera_t {
	vec3 position, rotation;
	vec3 forward, right, up;

	float fov;
	float min,max;

	struct {
		unsigned int width, height;

		/// framebuffer object, color texture, depth/stencil texture
		unsigned int fbo, tex, depth;

		unsigned int vao, vbo, program;
	} framebuffer;

	struct {
		mat4 view;
		mat4 projection;
	} matrices;
} camera_t;

void init_camera(camera_t*);
void update_camera_projection_matrix(camera_t*);
void clear_camera(camera_t*);
void present_camera(camera_t*, window_t*);
void destroy_camera(camera_t*);

void move_camera(camera_t*, window_t*);
