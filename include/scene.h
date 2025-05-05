#pragma once

#include "object.h"
#include "camera.h"

#define SCENE_T_DEFAULT()	\
(scene_t){	\
	.objects = NULL,	\
	.object_count = 0,	\
	.object_capacity = 0,	\
}

typedef struct scene_t {
	object_t* objects;
	size_t object_count;
	size_t object_capacity;
} scene_t;

void load_scene(scene_t*, camera_t*, const char* path);
object_t* create_scene_object(scene_t*);
void destroy_scene(scene_t*);
