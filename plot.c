#include <SDL.h>
#include <SDL_ttf.h>

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
plot_text(const char *message, TTF_Font *font, SDL_Color color, int x, int y)
{
	SDL_Texture *texture = NULL;
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	SDL_Surface *surf = TTF_RenderUTF8_Blended(font, message, color);
	if (!surf) {
		printf("Error in TTF_RenderUTF8_Blended\n");
		return;
	}
	texture = SDL_CreateTextureFromSurface(renderer, surf);
	if (!texture){
		printf("Error in SDL_CreateTextureFromSurface\n");
	}
	SDL_FreeSurface(surf);

	SDL_QueryTexture(texture, NULL, NULL, &dst.w, &dst.h);
	SDL_RenderCopy(renderer, texture, NULL, &dst);

	SDL_DestroyTexture(texture);
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
