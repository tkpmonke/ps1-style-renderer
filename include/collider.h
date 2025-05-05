#pragma once

#include <stdint.h>
#include <cglm/cglm.h>

typedef union collider_ut {
	struct {
		vec3 min;
		vec3 max;
	} cube;

	struct {
		float radius;
	} sphere;
} collider_ut;

