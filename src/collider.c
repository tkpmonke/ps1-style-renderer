#include "rigidbody.h"

uint8_t aabb_check_point(rigidbody_t* a, vec3 b) {
	return 
		(b[0] >= a->collider.cube.min[0] && b[0] <= a->collider.cube.max[0]) &&
		(b[1] >= a->collider.cube.min[1] && b[1] <= a->collider.cube.max[1]) &&
		(b[2] >= a->collider.cube.min[2] && b[2] <= a->collider.cube.max[2]);
}

uint8_t aabb_check_aabb(rigidbody_t* a, rigidbody_t* b) {
	return 
		(b->collider.cube.max[0] >= a->collider.cube.min[0] && b->collider.cube.min[0] <= a->collider.cube.max[0]) &&
		(b->collider.cube.max[1] >= a->collider.cube.min[1] && b->collider.cube.min[1] <= a->collider.cube.max[1]) &&
		(b->collider.cube.max[2] >= a->collider.cube.min[2] && b->collider.cube.min[2] <= a->collider.cube.max[2]);
}

uint8_t aabb_check_sphere(rigidbody_t* a, rigidbody_t* b) {
	float x = glm_max(a->collider.cube.min[0], glm_min(b->position[0], a->collider.cube.max[0]));
	float y = glm_max(a->collider.cube.min[1], glm_min(b->position[1], a->collider.cube.max[1]));
	float z = glm_max(a->collider.cube.min[2], glm_min(b->position[2], a->collider.cube.max[2]));

	float distance2 = (x-b->position[0]) * (x-b->position[0]) +
							(y-b->position[1]) * (y-b->position[1]) +
							(z-b->position[2]) * (z-b->position[2]);

	return sqrt(distance2) < b->collider.sphere.radius;
}

uint8_t sphere_check_point(rigidbody_t* a, vec3 b) {
	float distance2 =	pow((b[0]-a->position[0]),2) +
							pow((b[1]-a->position[1]),2) +
							pow((b[2]-a->position[2]),2);
	return distance2 < a->collider.sphere.radius*a->collider.sphere.radius;
}

uint8_t sphere_check_sphere(rigidbody_t* a, rigidbody_t* b) {
	float distance2 = pow((a->position[0]-b->position[0]),2) +
							pow((a->position[1]-b->position[1]),2) +
							pow((a->position[2]-b->position[2]),2);
	return sqrt(distance2) < a->collider.sphere.radius + b->collider.sphere.radius;
}
