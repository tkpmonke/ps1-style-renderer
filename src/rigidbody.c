#include "rigidbody.h"
#include "scene.h"

void simulate_rigidbody(rigidbody_t* rb, void* vscene, float delta_time) {
	if (rb->is_static) return;

	scene_t* scene = (scene_t*)vscene;
	
	vec3 last_acceleration;
	glm_vec3_scale((float*)gravity_direction, 1/rb->mass, last_acceleration);

	vec3 delta_vel;
	glm_vec3_scale(rb->velocity, delta_time, delta_vel);

	vec3 scaled_last_acc;
	glm_vec3_scale(last_acceleration, delta_time*delta_time*0.5f, scaled_last_acc);
	
	vec3 tmp;
	glm_vec3_add(delta_vel, scaled_last_acc, tmp);

	vec3 predicted_position;
	glm_vec3_add(rb->position, tmp, predicted_position);

	for (size_t i = 0; i < scene->object_count; ++i) {
		rigidbody_t* other = scene->objects[i].rigidbody;

		if (other == NULL || other == rb) {
			continue;
		}

		vec3 original_position;
		glm_vec3_copy(rb->position, original_position);
		glm_vec3_copy(predicted_position, rb->position);

		uint8_t colliding = 0;
		if (other->collider_type == collider_type_cube) {
			if (rb->collider_type == collider_type_cube) {
				colliding = aabb_check_aabb(rb, other);
			} else if (rb->collider_type == collider_type_sphere) {
				colliding = aabb_check_sphere(other, rb);
			}
		} else if (other->collider_type == collider_type_sphere) {
			if (rb->collider_type == collider_type_cube) {
				colliding = aabb_check_sphere(rb, other);
			} else if (rb->collider_type == collider_type_sphere) {
				colliding = sphere_check_sphere(rb, other);
			}
		}

		if (colliding) {
			glm_vec3_copy(original_position, rb->position);
			glm_vec3_zero(rb->velocity);
			return;
		}
		glm_vec3_copy(original_position, rb->position);
	}

	glm_vec3_copy(predicted_position, rb->position);

	vec3 new_acceleration, avg_acceleration;
	glm_vec3_scale((float*)gravity_direction, 1/rb->mass, new_acceleration);
	glm_vec3_add(new_acceleration, last_acceleration, avg_acceleration);
	glm_vec3_scale(avg_acceleration, 0.5f, avg_acceleration);
	glm_vec3_scale(avg_acceleration, delta_time, tmp);
	glm_vec3_add(rb->velocity, tmp, rb->velocity);
}
