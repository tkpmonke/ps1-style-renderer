#pragma once
#include "window.h"
#include "camera.h"

void init_debug_menu(window_t*);
void draw_debug_menu(camera_t*, window_t*);
void destroy_debug_window();
