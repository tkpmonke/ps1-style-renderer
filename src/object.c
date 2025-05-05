#include "object.h"
#include "camera.h"
#include <GLFW/glfw3.h>
void get_object_model_matrix(object_t* object, mat4 out) {
	vec4 quat;
	vec3 rot;
	rot[0] = glm_rad(object->rotation[0]);
	rot[1] = glm_rad(object->rotation[1]);
	rot[2] = glm_rad(object->rotation[2]);
	glm_euler_zyx_quat(rot, quat);

	glm_mat4_identity(out);
	glm_translate(out, object->position);
	glm_quat_rotate(out, quat, out);
	glm_scale(out, object->scale);
}

void draw_object(object_t* object, camera_t* camera) {
	mat4 model_matrix;
	get_object_model_matrix(object, model_matrix);

	glUseProgram(object->shader->program);
	glUniform2f(glGetUniformLocation(object->shader->program, "u_screen_res"), camera->framebuffer.width, camera->framebuffer.height);
	glUniform1f(glGetUniformLocation(object->shader->program, "u_time"), glfwGetTime());

	glUniformMatrix4fv(glGetUniformLocation(object->shader->program, "u_model"),
			1, GL_FALSE, (float*)model_matrix);
	glUniformMatrix4fv(glGetUniformLocation(object->shader->program, "u_view"), 
			1, GL_FALSE, (float*)camera->matrices.view);
	glUniformMatrix4fv(glGetUniformLocation(object->shader->program, "u_projection"),
			1, GL_FALSE, (float*)camera->matrices.projection);

	glBindVertexArray(object->model->vao);
	glDrawElements(GL_TRIANGLES, object->model->indice_count, GL_UNSIGNED_INT, 0);
}
