#include <SDL.h>

#include "colour.h"
#include "plot.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

void plot_update()
{
	SDL_RenderPresent(renderer);
}

void plot_rect(unsigned int x, unsigned int y, unsigned int width, unsigned int height, struct colour *c)
{
	SDL_Rect rect;

	SDL_SetRenderDrawColor(renderer, c->r, c->g, c->b, 255);

	rect.x = x;
	rect.y = y;
	rect.w = width;
	rect.h = height;

	SDL_RenderFillRect(renderer, &rect);
}

void plot_cell(unsigned int x, unsigned int y, struct colour *c)
{
	plot_rect(
		x*CELL_SIZE+(x-1)*BORDER_THICKNESS,
		y*CELL_SIZE+(y-1)*BORDER_THICKNESS,
		CELL_SIZE,
		CELL_SIZE,
		c
	);
}

void plot_cell_borders()
{
	int i, j;

	struct colour col = {.r = 0, .g = 0, .b = 0};

	for (i = 1; i < WIDTH_CELLS; i++)
	{
		j = i*CELL_SIZE + (i-1)*BORDER_THICKNESS;
		plot_rect(
			j, 0,
			BORDER_THICKNESS, HEIGHT_PIXELS,
			&col);
	}

	for (i = 1; i < HEIGHT_CELLS; i++)
	{
		j = i*CELL_SIZE + (i-1)*BORDER_THICKNESS;
		plot_rect(
			0, j,
			WIDTH_PIXELS, BORDER_THICKNESS,
			&col);
	}

}


int plot_init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "Failed to init SDL video: %s\n", SDL_GetError());
		return 1;
	}

	window = SDL_CreateWindow(
		"Some title",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		WIDTH_PIXELS,
		HEIGHT_PIXELS,
		SDL_WINDOW_SHOWN
	);

	if (window == NULL)
	{
		fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
		return 1;
	}

	renderer = SDL_CreateRenderer(window, 0, 0);
	if (renderer == NULL)
	{
		fprintf(stderr, "Failed to get window renderer: %s\n", SDL_GetError());
		return 1;
	}

	/* blank out the background with gray */
	SDL_SetRenderDrawColor(renderer, 0x33, 0x33, 0x33, 0xFF);
	SDL_RenderFillRect(renderer, NULL);

	plot_cell_borders();

	return 0;
}
