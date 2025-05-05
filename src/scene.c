#define _POSIX_C_SOURCE 200112L
#include "scene.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>

char buf[512];
#if defined(__unix__)

#include <unistd.h>
#include <limits.h>

char* duplicate_string(char* str) {
	size_t len = strlen(str);
	char* out = malloc(len+1);
	memcpy(str, out, len+1);
	return out;
}

char* exe_path() {
	ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf)-1);
	
	if (len == -1) return NULL;
	buf[len] = '\0';
	
	char* d = duplicate_string(buf);
	strcpy(buf, dirname(d));
	free(d);

	return buf;
}

#elif defined(_WIN32)
#include <windows.h>
#include <shlwapi.h>
char* exe_path() {
	GetModuleFileName(NULL, buf, 512);
	PathRemoveFileSpec(buf);
}
#endif

char* dump_file(char* path, uint8_t is_local) {
	char* content = NULL;
	int size = 0;

	char* p;

	if (is_local) {
		p = strcat(strcat(exe_path(), "/"), path);
	} else {
		p = path;
	}

	FILE* fp = fopen(p, "r");
	if (fp) {
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		rewind(fp);

		content = (char*)malloc(size+1);
		fread(content, 1, size, fp);
		fclose(fp);
	} else {
		return NULL;
	}

	content[size] = '\0';
	return content;
}

char* skip_whitespace(char* s) {
	while (*s && (*s == ' ' || *s == '\n' || *s == '\t' || *s == '\r')) s++;
	return s;
}

void read_vec3(char* s, vec3 out) {
	sscanf(s, "%f,%f,%f", &out[0], &out[1], &out[2]);
}

void set_object_property(scene_t* scene, char* variable, char* value) {
	vec3 tmp;
	if (strcmp(variable, "position") == 0) {
		read_vec3(value, tmp);
		glm_vec3_copy(tmp, scene->objects[scene->object_count-1].position);
	} else if (strcmp(variable, "rotation") == 0) {
		read_vec3(value, tmp);
		glm_vec3_copy(tmp, scene->objects[scene->object_count-1].rotation);
	} else if (strcmp(variable, "scale") == 0) {
		read_vec3(value, tmp);
		glm_vec3_copy(tmp, scene->objects[scene->object_count-1].scale);
	} else if (strcmp(variable, "model") == 0) {
		load_model(value, scene->objects[scene->object_count-1].model);
	} else if (strcmp(variable, "vertex") == 0) {
		scene->objects[scene->object_count-1].shader->vertex = dump_file(value, 1);
	} else if (strcmp(variable, "fragment") == 0) {
		scene->objects[scene->object_count-1].shader->fragment = dump_file(value, 1);
	} else if (strcmp(variable, "compile_shader") == 0) {
		compile_shader(scene->objects[scene->object_count-1].shader);
	}
}

void set_camera_property(camera_t* camera, char* variable, char* value) {
	vec3 tmp;
	if (strcmp(variable, "position") == 0) {
		read_vec3(value, tmp);
		glm_vec3_copy(tmp, camera->position);
	} else if (strcmp(variable, "rotation") == 0) {
		read_vec3(value, tmp);
		glm_vec3_copy(tmp, camera->rotation);
	} else if (strcmp(variable, "fov") == 0) {
		camera->fov = atof(value);
	} else if (strcmp(variable, "min") == 0) {
		camera->min = atof(value);
	} else if (strcmp(variable, "max") == 0) {
		camera->max = atof(value);
	} else if (strcmp(variable, "width") == 0) {
		camera->framebuffer.width = atoi(value);
	} else if (strcmp(variable, "height") == 0) {
		camera->framebuffer.height = atoi(value);
	}
}

void set_rigidbody_properties(scene_t* scene, char* variable, char* value) {
	if (strcmp(variable, "mass") == 0) {
		scene->objects[scene->object_count-1].rigidbody->mass = atof(value);
	} else if (strcmp(variable, "static") == 0) {
		scene->objects[scene->object_count-1].rigidbody->is_static = atoi(value);
	}
}

void set_collider_properties(scene_t* scene, char* variable, char* value) {
	if (scene->objects[scene->object_count-1].rigidbody == NULL) {
		fprintf(stderr, "Rigidbody must be defined before using Collider\n");
		return;
	}

	if (strcmp(variable, "type") == 0) {
		if (strcmp(value, "cube") == 0) {
			scene->objects[scene->object_count-1].rigidbody->collider_type = collider_type_cube;
		} else if (strcmp(value, "sphere") == 0) {
			scene->objects[scene->object_count-1].rigidbody->collider_type = collider_type_sphere;
		}
	} else if (strcmp(variable, "radius") == 0) {
		scene->objects[scene->object_count-1].rigidbody->collider.sphere.radius = atof(value);
	} else if (strcmp(variable, "min") == 0) {
		read_vec3(value, scene->objects[scene->object_count-1].rigidbody->collider.cube.min);
	} else if (strcmp(variable, "max") == 0) {
		read_vec3(value, scene->objects[scene->object_count-1].rigidbody->collider.cube.max);
	}

}

void load_scene(scene_t* scene, camera_t* camera, const char* path) {
	char* current_section = NULL;

	char* file_contents = dump_file((char*)path, 1);
	char* token = strtok(file_contents, "\n");

	/// basically while new line
	while (token != NULL) {
		if (token[0] == '[') {
			int end_brace_pos = 0;

			while (token[end_brace_pos] != ']') {
				++end_brace_pos;
			}

			current_section = realloc(current_section, end_brace_pos);

			for (int i = 1; i < end_brace_pos; ++i) {
				current_section[i-1] = token[i];
			}
			current_section[end_brace_pos-1] = '\0';

			if (strcmp("Object", current_section) == 0) {
				create_scene_object(scene);
			} else if (strcmp("Rigidbody", current_section) == 0) {
				scene->objects[scene->object_count-1].rigidbody = malloc(sizeof(rigidbody_t));
			}
		} else {
			skip_whitespace(token);

			if (token == NULL) {
				goto break_loop;
			}

			int equal_position = 0;
			while (token[equal_position] != '=') {
				++equal_position;
			}

			char* variable_name = malloc(equal_position);
			for (int i = 0; i < equal_position-1; ++i) {
				variable_name[i] = token[i];
			}
			variable_name[equal_position-1] = '\0';

			int value_size = strlen(token)-equal_position-2;
			char* variable_value = malloc(value_size);
			for (int i = 0; i < value_size; ++i) {
				variable_value[i] = token[i+equal_position+2];
			}
			variable_value[value_size] = '\0';

			if (strcmp("Object", current_section) == 0) {
				set_object_property(scene, variable_name, variable_value);
			} else if (strcmp("Rigidbody", current_section) == 0) {
				set_rigidbody_properties(scene, variable_name, variable_value);
			} else if (strcmp("Collider", current_section) == 0) {
				set_collider_properties(scene, variable_name, variable_value);
			} else if (strcmp("Camera", current_section) == 0) {
				set_camera_property(camera, variable_name, variable_value);
			}

			free(variable_name);
			free(variable_value);
		}

break_loop:
		token = strtok(NULL, "\n");
	}

	free(current_section);
}

object_t* create_scene_object(scene_t* scene) {
	if (scene->object_count+1 > scene->object_capacity) {
		scene->object_capacity = ceil(1.5f*scene->object_capacity);
		if (scene->object_capacity == 0) {
			scene->object_capacity = 1;
			scene->objects = malloc(sizeof(object_t));
		} else {
			scene->objects = realloc(scene->objects, sizeof(object_t)*scene->object_capacity);
		}
	}

	scene->objects[scene->object_count] = OBJECT_T_DEFAULT();
	scene->objects[scene->object_count].shader = malloc(sizeof(shader_t));
	scene->objects[scene->object_count].model = malloc(sizeof(model_t));
	scene->object_count++;

	return &scene->objects[scene->object_count-1];
}

void destroy_scene(scene_t* scene) {
	for (unsigned int i = 0; i < scene->object_count; ++i) {
		if (scene->objects[i].model != NULL) {
			free(scene->objects[i].model);
		} if (scene->objects[i].shader != NULL) {
			free(scene->objects[i].shader);
		} if (scene->objects[i].rigidbody != NULL) {
			free(scene->objects[i].rigidbody);
		}
	}

	if (scene->object_capacity > 0) {
		free(scene->objects);
	}
	scene->object_count = 0;
	scene->object_capacity = 0;
}
