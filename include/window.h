#pragma once

typedef struct window_t {
	int width;
	int height;
	char draw_debug_menu;
	void* window;
} window_t;

void create_window(window_t*, const char* name);
char poll_window_events(window_t*);
void present_window(window_t*);

void destroy_window(window_t*);
