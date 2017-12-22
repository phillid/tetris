#include <SDL.h>

#include "colour.h"
#include "plot.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

void
plot_update(void)
{
	SDL_RenderPresent(renderer);
}

void
plot_rect(unsigned int x, unsigned int y, unsigned int width, unsigned int height, struct colour *c)
{
	SDL_Rect rect;

	SDL_SetRenderDrawColor(renderer, c->r, c->g, c->b, 255);

	rect.x = x;
	rect.y = y;
	rect.w = width;
	rect.h = height;

	SDL_RenderFillRect(renderer, &rect);
}

void
plot_cell(unsigned int x, unsigned int y, struct colour *c)
{
	plot_rect(
		x*CELL_SIZE+(x-1)*BORDER_THICKNESS,
		y*CELL_SIZE+(y-1)*BORDER_THICKNESS,
		CELL_SIZE,
		CELL_SIZE,
		c
	);
}

void
plot_clear(void)
{
	/* blank out the background with black */
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderFillRect(renderer, NULL);
}

int
plot_init(void)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Failed to init SDL video: %s\n", SDL_GetError());
		return 1;
	}

	window = SDL_CreateWindow(
		"Tetris Clone",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		WIDTH_PIXELS,
		HEIGHT_PIXELS,
		SDL_WINDOW_SHOWN
	);

	if (window == NULL) {
		fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
		return 1;
	}

	renderer = SDL_CreateRenderer(window, 0, 0);
	if (renderer == NULL) {
		fprintf(stderr, "Failed to get window renderer: %s\n", SDL_GetError());
		return 1;
	}

	plot_clear();

	return 0;
}
