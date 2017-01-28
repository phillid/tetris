#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>
#include <time.h>

#include "colour.h"
#include "tetromino.h"
#include "plot.h"

#define VERSION "1.0"
#define TETROMINO_WIDTH  4
#define TETROMINO_HEIGHT 4
#define TETROMINO_AREA   (TETROMINO_HEIGHT*TETROMINO_WIDTH)
#define MIN(a,b)         (a<b? a : b)

struct colour palette[] = {
	{.r=0x00,.g=0x00,.b=0x00},
	{.r=0x00,.g=0x00,.b=0xFF},
	{.r=0x00,.g=0xFF,.b=0x00},
	{.r=0x00,.g=0xFF,.b=0xFF},
	{.r=0xFF,.g=0x00,.b=0x00},
	{.r=0xFF,.g=0x00,.b=0xFF},
	{.r=0xFF,.g=0xFF,.b=0x00},
	{.r=0xFF,.g=0xFF,.b=0xFF}
};

void
draw_board(struct colour* (*board)[WIDTH_CELLS][HEIGHT_CELLS])
{
	unsigned int x,y;
	plot_clear();
	for (y = 0; y < HEIGHT_CELLS; y++) {
		for (x = 0; x < WIDTH_CELLS; x++) {
			plot_cell(x,y, (*board)[x][y]);
		}
	}
}

void
draw_piece(int x, int y, struct colour *c, char (*piece)[4][4])
{
	int px, py, ipx, ipy, end_x, end_y, wx, wy;
	ipx = (x < 0)? -x : 0;
	ipy = (y < 0)? -y : 0;
	end_y = MIN(HEIGHT_CELLS, y + 4);
	end_x = MIN(WIDTH_CELLS, x + 4);

	for (wy = (y < 0)? 0 : y, py = ipy; wy < end_y; wy++, py++)
		for (wx = (x < 0)? 0 : x, px = ipx; wx < end_x; wx++, px++)
			if ((*piece)[px][py])
				plot_cell(wx, wy, c);
}

void
drop_piece(int x, int y, struct piece *piece, struct colour* (*board)[WIDTH_CELLS][HEIGHT_CELLS])
{
	int px, py, ipx, ipy, end_x, end_y, wx, wy;
	ipx = (x < 0)? -x : 0;
	ipy = (y < 0)? -y : 0;
	end_y = MIN(HEIGHT_CELLS, y + 4);
	end_x = MIN(WIDTH_CELLS, x + 4);

	for (wy = (y < 0)? 0 : y, py = ipy; wy < end_y; wy++, py++)
		for (wx = (x < 0)? 0 : x, px = ipx; wx < end_x; wx++, px++)
			if ((*piece->bitmap)[px][py])
				(*board)[wx][wy] = piece->colour;
}

Uint32
gravity_callback(Uint32 interval, void *param)
{
	(void)param; /* solves unused parameter warn+error */
	SDL_Event e;
	SDL_UserEvent ue;
	ue.type = SDL_USEREVENT;
	ue.code = 0;
	ue.data1 = 0;
	ue.data2 = 0;

	e.type = SDL_USEREVENT;
	e.user = ue;

	SDL_PushEvent(&e);
	return interval;
}

int
hit_floor(int x, int y, struct piece *held, struct colour* (*board)[WIDTH_CELLS][HEIGHT_CELLS])
{
	int px,py;
	for (px = 0; px < 4; px++) {
		/* seek to first cell of column */
		py = 0;
		while (py < 4 && (*held->bitmap)[px][py] == 0)
			py++;

		/* column has no cells? no collision possible. NEXT! */
		if (py == 4)
			continue;

		while (py < 4 && (*held->bitmap)[px][py])
			py++;

		if (   y + py >= HEIGHT_CELLS
			|| (*board)[x + px][y + py] != &(palette[0]))
			return 1;
	}
	return 0;
}

int
hit_side(int x, int y, struct piece *held, struct colour* (*board)[WIDTH_CELLS][HEIGHT_CELLS])
{
	int px,py;
	for (py = 0; py < 4; py++) {
		for (px = 0; px < 4; px++) {
			if ((*held->bitmap)[px][py]
			    &&
			    (  x + px >= WIDTH_CELLS
			    || x + px < 0
			    || (*board)[x + px][y + py] != &(palette[0])
			    ))
				return 1;
		}
	}
	return 0;
}

void
update_bitmap(struct piece *held)
{
	held->bitmap = &(tetrominoes[0][held->type][held->rotation]);
}

void
new_piece(struct piece *held)
{
	held->colour = &palette[rand() % 7 + 1];
	held->type = rand()%7;
	held->rotation= rand()%4;
	update_bitmap(held);
}

void
rotate(struct piece *held, int direction)
{
	held->rotation += direction;
	/* FIXME need to handle direction not in [-4, 4] */
	if (held->rotation >= 4)
		held->rotation -= 4;

	if (held->rotation < 0)
		held->rotation += 4;

	update_bitmap(held);
}

void
clear_rows(struct colour* (*board)[WIDTH_CELLS][HEIGHT_CELLS])
{
	char row;
	int x, y, x1, y1;
	for (y = 0; y < HEIGHT_CELLS; y++) {
		row = 1;
		for (x = 0; x < WIDTH_CELLS; x++) {
			if ((*board)[x][y] == &(palette[0])) {
				row = 0;
				break;
			}
		}
		if (!row)
			continue;

		for (y1 = y; y1 > 0; y1--)
			for (x1 = 0; x1 < WIDTH_CELLS; x1++)
				(*board)[x1][y1] = (*board)[x1][y1-1];

		for (x1 = 0; x1 < WIDTH_CELLS; x1++)
			(*board)[x1][y1] = &(palette[0]);
	}
}

void
main_loop()
{
	struct colour *board[WIDTH_CELLS][HEIGHT_CELLS];
	SDL_Event e = {0};
	bool running = false;
	int i = 0;
	int x = 0;
	int y = 0;
	int last_x = 0;
	int last_y = 1;
	struct piece held;
	new_piece(&held);

	for (y = 0; y < HEIGHT_CELLS; y++)
		for (x = 0; x < WIDTH_CELLS; x++)
			board[x][y] = &(palette[0]);

	running = true;
	last_x = last_y = x = y = 0;
	SDL_AddTimer(500, &gravity_callback, NULL);
	char lockout;

	while (running) {
		lockout = 0;
		clear_rows(&board);

		if (hit_side(x, y, &held, &board))
			x = last_x;

		if (hit_floor(x, y, &held, &board))
			lockout = 1;

		draw_board(&board);
		draw_piece(x, y, held.colour, held.bitmap);
		plot_update();
		SDL_WaitEvent(&e);
		switch (e.type) {
		case SDL_USEREVENT:
			if (lockout) {
				drop_piece(x, y, &held, &board);
				last_x = last_y = x = y = 0;
				new_piece(&held);
				lockout = 0;
			} else {
				last_y = y++; /* gravity */
				last_x = x;
			}
			break;
		case SDL_QUIT:
			fprintf(stderr, "quit\n");
			running = false;
			break;
		case SDL_KEYDOWN:
			switch (e.key.keysym.sym)
			{
			case SDLK_a: last_x = x--; break;
			case SDLK_d: last_x = x++; break;
			case SDLK_w:
				i = 0;
				do {
					rotate(&held, 1);
				} while(hit_side(x, y, &held, &board) && i++ < 4);
				break;
			case SDLK_q:
				running = false;
				break;
			}
			break;
		default:
			break;
		}
	}
}

int
main(int argc, char **argv)
{
	char *argument = NULL;

	if (argc >= 2) {
		argument = argv[1];
		if (strcmp(argument, "-v") == 0) {
			fprintf(stderr, "tetris "VERSION"\n");
		} else {
			fprintf(stderr, "%s [-v]\n", argv[0]);
		}
		return 0;
	}
	srand(time(NULL));
	plot_init();
	main_loop();
}
