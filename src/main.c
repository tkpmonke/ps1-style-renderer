#include "camera.h"
#include "object.h"
#include "scene.h"

#include "debug.h"

#include <GLFW/glfw3.h>

float get_delta_time() {
	static float last_time = 0;
	float time = glfwGetTime();
	float delta = time-last_time;
	last_time = time;
	return delta;
}

int main() {
	window_t window = {
		.width = 1280, 
		.height = 720,
		.window = NULL
	};
	create_window(&window, "cool window");

	camera_t camera = CAMERA_T_DEFAULT();

	scene_t scene = SCENE_T_DEFAULT();
	load_scene(&scene, &camera, "main.scene");

	init_camera(&camera);

	init_debug_menu(&window);

	while (poll_window_events(&window)) {
		move_camera(&camera, &window);
		clear_camera(&camera);

		//draw_object(&water, &camera);
		//draw_object(&beach, &camera);
		
		float delta = get_delta_time();
		for (unsigned int i = 0; i < scene.object_count; ++i) {
			if (scene.objects[i].rigidbody != NULL) {
				glm_vec3_copy(scene.objects[i].position, scene.objects[i].rigidbody->position);
				simulate_rigidbody(scene.objects[i].rigidbody, &scene, delta);
				glm_vec3_copy(scene.objects[i].rigidbody->position, scene.objects[i].position);
			}

			draw_object(&scene.objects[i], &camera);
		}

		present_camera(&camera, &window);
	}

	destroy_debug_window();
	destroy_scene(&scene);
	destroy_camera(&camera);
	destroy_window(&window);
}
