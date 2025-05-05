#pragma once

#include "collider.h"

typedef struct rigidbody_t {
	float mass;
	vec3 velocity;
	vec3 position;

	uint8_t is_static;

	enum {
		collider_type_cube,
		collider_type_sphere
	} collider_type;

	collider_ut collider;
} rigidbody_t;

const static float gravity_force = 9.8f;
const static vec3 gravity_direction = {0, -gravity_force, 0};

uint8_t aabb_check_point(rigidbody_t* a, vec3 b);
uint8_t aabb_check_aabb(rigidbody_t* a, rigidbody_t* b);
uint8_t aabb_check_sphere(rigidbody_t* a, rigidbody_t* b);

uint8_t sphere_check_point(rigidbody_t* a, vec3 b);
uint8_t sphere_check_sphere(rigidbody_t* a, rigidbody_t* b);

void simulate_rigidbody(rigidbody_t* rb, void* scene, float delta_time);
void apply_force(rigidbody_t* rb, vec3 force);
