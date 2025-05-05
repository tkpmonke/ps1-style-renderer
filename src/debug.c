#include "debug.h"
#include "camera.h"

/// boat load of defines for nuklear
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear_glfw_gl3.h"

struct nk_glfw glfw = {0};
struct nk_context *ctx;
struct nk_colorf bg;

void init_debug_menu(window_t* window) {
	ctx = nk_glfw3_init(&glfw, window->window, NK_GLFW3_INSTALL_CALLBACKS);

	struct nk_font_atlas* atlas;
	nk_glfw3_font_stash_begin(&glfw, &atlas);
	nk_glfw3_font_stash_end(&glfw);

	window->draw_debug_menu = 1;
}

void draw_debug_menu(camera_t* camera, window_t* window) {
	nk_glfw3_new_frame(&glfw);
	
	if (nk_begin(ctx, "debug_window", nk_rect(0, 0, 300, 400),
				NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_SCALABLE |
				NK_WINDOW_MOVABLE)) {

		nk_layout_row_dynamic(ctx, 30, 1);
		nk_label(ctx, "Camera Position", NK_TEXT_CENTERED);
		nk_layout_row_dynamic(ctx, 30, 3);
		nk_property_float(ctx, "#X", -1000.f, &camera->position[0], 1000.f, 1, 1);
		nk_property_float(ctx, "#Y", -1000.f, &camera->position[1], 1000.f, 1, 1);
		nk_property_float(ctx, "#Z", -1000.f, &camera->position[2], 1000.f, 1, 1);

		nk_layout_row_dynamic(ctx, 30, 1);
		nk_label(ctx, "Camera Rotation", NK_TEXT_CENTERED);
		nk_layout_row_dynamic(ctx, 30, 3);
		nk_property_float(ctx, "#X", -1000.f, &camera->rotation[0], 1000.f, 0.1f, 0.05f);
		nk_property_float(ctx, "#Y", -1000.f, &camera->rotation[1], 1000.f, 0.1f, 0.05f);
		nk_property_float(ctx, "#Z", -1000.f, &camera->rotation[2], 1000.f, 0.1f, 0.05f);

		nk_layout_row_dynamic(ctx, 30, 1);
		nk_label(ctx, "Window Scale", NK_TEXT_CENTERED);
		nk_layout_row_dynamic(ctx, 30, 2);
		nk_property_int(ctx, "#X", 0, &window->width, 10000, 1, 1);
		nk_property_int(ctx, "#Y", 0, &window->height, 10000, 1, 1);

		nk_spacer(ctx);

		nk_layout_row_dynamic(ctx, 30, 1);
		nk_property_float(ctx, "FOV", 15, &camera->fov, 140, 1, 1);
	}
	nk_end(ctx);

	nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, 512*1024, 128*1024);
}

void destroy_debug_window() {
	nk_glfw3_shutdown(&glfw);
}
