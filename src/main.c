#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <memory.h>
#include <SDL2/SDL.h>

#define main_fatal(fmt, ...) (fprintf(stderr, "FATAL %s(%d) %s: " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__), exit(EXIT_FAILURE))
#define println(fmt, ...) printf("%s(%d) %s: " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define GFX_WIDTH  64
#define GFX_HEIGHT 32
typedef struct Chip8
{
	bool draw;
	uint16_t opcode;
	uint8_t  V[16];
	uint16_t I;
	uint16_t pc;
	uint8_t  mem[4096];
	uint8_t  gfx[GFX_WIDTH*GFX_HEIGHT];
	uint8_t  delay_timer;
	uint8_t  sound_timer;
	uint16_t stack[16];
	uint16_t sp;
	uint8_t  key[16];
} Chip8;

static int font_set[80];

void chip8_init(Chip8 *c8)
{
	c8->draw   = true;
	c8->pc     = 0x200;
	c8->opcode = 0;
	c8->I      = 0;
	c8->sp     = 0;

	memcpy(c8->mem, font_set, sizeof font_set);
}

void chip8_load(const char *filename)
{

}

void chip8_draw(Chip8 *c)
{

}

void chip8_set_keys(Chip8 *c)
{

}

void chip8_emulate_cycle(Chip8 *c)
{
	uint16_t opcode = c->mem[c->pc] << 8 | c->mem[c->pc+1];
	switch (opcode & 0xF000) {
	case 0x0000:
		switch (opcode & 0x000F) {
		case 0x0000:
			break;
		case 0x000E:
			break;
		default:
			println("Unknown opcode [0x0000] 0x%X", opcode);
			break;
		}
		break;
	case 0xA000:
		c->I = opcode & 0x0FFF;
		c->pc += sizeof(opcode);
		break;
	case 0x2000:
		c->stack[c->sp] = c->pc;
		c->sp++;
		c->pc = opcode & 0x0FFF;
		break;
	default:
		println("Unknown opcode 0x%X", opcode);
		break;
	}

	if (c->delay_timer > 0)
		c->delay_timer--;

	if (c->sound_timer > 0) {
		if (c->sound_timer == 1) {
			println("beep");
		}
		c->sound_timer--;
	}
}

uint32_t main_ticks(void)
{
	return SDL_GetTicks();
}

static bool main_run = true;
void main_quit(void)
{
	main_run = false;
}

static SDL_Window *g_window;
static SDL_Renderer *g_ren;
int window_width = 800;
int window_height = 600;
static Chip8 *chip8 = &(Chip8){
	.draw = true,
};
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

	chip8_init(chip8);
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

	chip8_emulate_cycle(chip8);
	chip8_draw(chip8);
	chip8_set_keys(chip8);


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
