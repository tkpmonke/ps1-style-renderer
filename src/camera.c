#include "camera.h"
#include "debug.h"

#include <GLFW/glfw3.h>

const float quad_vertices[12] = {
	-1, 	 1,
	-1, 	-1,
	 1, 	-1,
	-1, 	 1,
	 1, 	-1,
	 1, 	 1
};

const char* const vertex_shader = 
"#version 330 core\n"
"layout (location = 0) in vec2 in_pos;\n"
"out vec2 uv;\n"
"uniform vec2 u_scale;\n"
"void main() {\n"
"	uv = (in_pos+1)/2;\n"
"	gl_Position = vec4(in_pos * u_scale, 0, 1);\n"
"}\n\0";
const char* const fragment_shader = 
"#version 330 core\n"
"uniform sampler2D u_screen;\n"
"in vec2 uv;\n"
"out vec4 out_color;\n"
"void main() {\n"
"	out_color = texture(u_screen, uv);\n"
"}\n\0";

void create_quad(camera_t* camera) {
	glGenVertexArrays(1, &camera->framebuffer.vao);
	glGenBuffers(1, &camera->framebuffer.vbo);
	glBindVertexArray(camera->framebuffer.vao);
	glBindBuffer(GL_ARRAY_BUFFER, camera->framebuffer.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	char info_log[512];
	int success;

	unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertex_shader, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, info_log);
		printf("Vertex Shader Failed To Compile > %s", info_log);
	}

	unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragment_shader, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, info_log);
		printf("Fragment Shader Failed To Compile > %s", info_log);
	}
	
	camera->framebuffer.program = glCreateProgram();
	glAttachShader(camera->framebuffer.program, vertex);
	glAttachShader(camera->framebuffer.program, fragment);
	glLinkProgram(camera->framebuffer.program);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void get_quad_scale(float* x, float* y, camera_t* camera, window_t* window) {
	float target_aspect = (float)camera->framebuffer.width/camera->framebuffer.height;
	float window_aspect = (float)window->width/window->height;
	if (window_aspect > target_aspect) {
		*x = target_aspect / window_aspect;
	} else {
		*y = window_aspect / target_aspect;
	}
}

void init_camera(camera_t* camera) {
	update_camera_projection_matrix(camera);

	glGenFramebuffers(1, &camera->framebuffer.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, camera->framebuffer.fbo);

	glGenTextures(1, &camera->framebuffer.tex);
	glBindTexture(GL_TEXTURE_2D, camera->framebuffer.tex);

	glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGB,
			camera->framebuffer.width,
			camera->framebuffer.height,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, camera->framebuffer.tex, 0);

	glGenRenderbuffers(1, &camera->framebuffer.depth);
	glBindRenderbuffer(GL_RENDERBUFFER, camera->framebuffer.depth);
	
	glRenderbufferStorage(
			GL_RENDERBUFFER,
			GL_DEPTH24_STENCIL8,
			camera->framebuffer.width,
			camera->framebuffer.height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, camera->framebuffer.depth);

	create_quad(camera);
}

void update_camera_projection_matrix(camera_t* camera) {
	glm_perspective(glm_rad(camera->fov), (float)camera->framebuffer.width/camera->framebuffer.height, 0.1f, 100.f, camera->matrices.projection);
	float pitch = camera->rotation[1];
   float yaw = camera->rotation[0];
	camera->forward[0] = -sin(yaw)*cos(pitch);
   camera->forward[1] = sin(pitch);
   camera->forward[2] = -cos(yaw)*cos(pitch);
   glm_vec3_cross(camera->forward, (vec3){0,1,0}, camera->right);
   glm_normalize(camera->right);
   glm_vec3_cross(camera->right, camera->forward, camera->up);
   glm_normalize(camera->up);

   vec3 center;
   glm_vec3_add(camera->position, camera->forward, center);
   glm_lookat(camera->position, center, camera->up, camera->matrices.view);
}

void clear_camera(camera_t* camera) {
	glBindFramebuffer(GL_FRAMEBUFFER, camera->framebuffer.fbo);
	glViewport(0, 0, camera->framebuffer.width, camera->framebuffer.height);

	glClearColor(0.2f, 0.3f, 0.7f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	update_camera_projection_matrix(camera);
}


void present_camera(camera_t* camera, window_t* window) {
	glDisable(GL_DEPTH_TEST);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, window->width, window->height);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	float scale_x = 1, scale_y = 1;
	get_quad_scale(&scale_x, &scale_y, camera, window);

	glUseProgram(camera->framebuffer.program);
	glUniform2f(glGetUniformLocation(camera->framebuffer.program, "u_scale"), scale_x, scale_y);
	glUniform1i(glGetUniformLocation(camera->framebuffer.program, "u_screen"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, camera->framebuffer.tex);

	glBindVertexArray(camera->framebuffer.vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	if (window->draw_debug_menu) {
		draw_debug_menu(camera, window);
	}

	glEnable(GL_DEPTH_TEST);
	present_window(window);


	/*
	camera->framebuffer.width = window->width;
	camera->framebuffer.height = window->height;

	glBindTexture(GL_TEXTURE_2D, camera->framebuffer.tex);
	glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGB,
			camera->framebuffer.width,
			camera->framebuffer.height,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			NULL);
	glBindRenderbuffer(GL_RENDERBUFFER, camera->framebuffer.depth);
	
	glRenderbufferStorage(
			GL_RENDERBUFFER,
			GL_DEPTH24_STENCIL8,
			camera->framebuffer.width,
			camera->framebuffer.height);
			*/
}

void destroy_camera(camera_t* camera) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &camera->framebuffer.fbo);
}

void move_camera(camera_t* camera, window_t* window) {
	const float move_speed = 3.f;
	const float look_speed = 50;
	static float prev_time;
	float time = glfwGetTime();
	float delta = time-prev_time;
	prev_time = time;

	vec3 forward, right, up;
	float speed = glfwGetKey(window->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? move_speed*2 : move_speed;
	glm_vec3_scale(camera->forward, speed*delta, forward);
	glm_vec3_scale(camera->right, speed*delta, right);
	glm_vec3_scale(camera->up, speed*delta, up);

	if (glfwGetKey(window->window, GLFW_KEY_W) == GLFW_PRESS) {
		glm_vec3_add(camera->position, forward, camera->position);
	} if (glfwGetKey(window->window, GLFW_KEY_S) == GLFW_PRESS) {
		glm_vec3_sub(camera->position, forward, camera->position);
	} if (glfwGetKey(window->window, GLFW_KEY_A) == GLFW_PRESS) {
		glm_vec3_sub(camera->position, right, camera->position);
	} if (glfwGetKey(window->window, GLFW_KEY_D) == GLFW_PRESS) {
		glm_vec3_add(camera->position, right, camera->position);
	} if (glfwGetKey(window->window, GLFW_KEY_E) == GLFW_PRESS) {
		glm_vec3_add(camera->position, up, camera->position);
	} if (glfwGetKey(window->window, GLFW_KEY_Q) == GLFW_PRESS) {
		glm_vec3_sub(camera->position, up, camera->position);
	} if (glfwGetKey(window->window, GLFW_KEY_UP) == GLFW_PRESS) {
		camera->rotation[1] += glm_rad(look_speed*delta);
	} if (glfwGetKey(window->window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		camera->rotation[1] -= glm_rad(look_speed*delta);
	} if (glfwGetKey(window->window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		camera->rotation[0] += glm_rad(look_speed*delta);
	} if (glfwGetKey(window->window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		camera->rotation[0] -= glm_rad(look_speed*delta);
	}
}
