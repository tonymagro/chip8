#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <SDL2/SDL.h>

#define main_fatal(fmt, ...) (fprintf(stderr, "FATAL %s(%d) %s: " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__), exit(EXIT_FAILURE))
#define println(fmt, ...) printf("%s(%d) %s: " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

uint32_t main_ticks(void)
{
	return SDL_GetTicks();
}

static SDL_Window *g_window;
static SDL_Renderer *g_ren;
static bool main_run = true;
int window_width = 800;
int window_height = 600;
void main_quit(void)
{
	main_run = false;
}
void main_init(void)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		main_fatal("Failed to initialize video: %s", SDL_GetError());
	}

	SDL_CreateWindowAndRenderer(window_width, window_height, 0, &g_window, &g_ren);

	if (!g_window) {
		main_fatal("Failed to create window %dx%d: %s", window_width, window_height, SDL_GetError());
	}
	if (!g_ren) {
		main_fatal("Failed to create renderer %dx%d: %s", window_width, window_height, SDL_GetError());
	}

}

static void handle_window_event(const SDL_WindowEvent *window_event)
{
	assert(window_event != NULL);
	switch (window_event->event) {
	case SDL_WINDOWEVENT_RESIZED:
		println("Window resized %dx%d", window_event->data1, window_event->data2);
		break;
	default:
		break;
	}
}

void main_quit(void);
static void handle_key_pressed_event(const SDL_KeyboardEvent *key_event)
{
	assert(key_event != NULL);

	switch (key_event->keysym.sym) {
	case SDLK_ESCAPE:
		main_quit();
		break;
	default:
		break;
	}
}

static void handle_keyboard_event(const SDL_KeyboardEvent *key_event)
{
	assert(key_event != NULL);
	uint8_t state = key_event->state;
	switch (key_event->keysym.sym) {
		default:
			if (state == SDL_PRESSED)
				handle_key_pressed_event(key_event);
			break;
	}
}

void main_update(float dt)
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			main_quit();
			return;
		case SDL_WINDOWEVENT:
			handle_window_event(&event.window);
			break;
		case SDL_KEYUP:
		case SDL_KEYDOWN:
			handle_keyboard_event(&event.key);
			break;
		default:
			break;
		}
	}

	SDL_SetRenderDrawColor(g_ren, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(g_ren);
	/*
	SDL_RenderFillRect(g_ren, &(SDL_Rect){
		.x = 0, .y = 0,
		.w = window_width,
		.h = window_height,
	});
	*/

	SDL_SetRenderDrawColor(g_ren, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderDrawPoint(g_ren, 50, 50);

	SDL_RenderPresent(g_ren);
}

void main_shutdown()
{
	if (g_ren)
		SDL_DestroyRenderer(g_ren);
	if (g_window)
		SDL_DestroyWindow(g_window);
	SDL_Quit();
}

#if !defined(UNIT_TESTS)
int main(int argc, char** argv)
{
	main_init();

	float last_time = main_ticks();
	while (main_run) {
		float new_time = main_ticks() / 1000.f;
		main_update(new_time - last_time);
		last_time = new_time;
	}

	main_shutdown();

	return EXIT_SUCCESS;
}
#endif // #if !defined(UNIT_TESTS)
