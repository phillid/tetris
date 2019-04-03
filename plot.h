void plot_rect(unsigned int x, unsigned int y, unsigned int width, unsigned int height, struct colour *c);
void plot_cell(unsigned int x, unsigned int y, struct colour *c);
void plot_text(const char *message, TTF_Font *font, SDL_Color color, int x, int y);
void plot_clear(void);
int plot_init(void);
void plot_update(void);

#define CELL_SIZE          25
#define BORDER_THICKNESS   2
#define WIDTH_CELLS        10
#define HEIGHT_CELLS       20

#define WIDTH_PIXELS       (WIDTH_CELLS * CELL_SIZE + BORDER_THICKNESS * (WIDTH_CELLS-1))
#define HEIGHT_PIXELS      (HEIGHT_CELLS * CELL_SIZE + BORDER_THICKNESS * (HEIGHT_CELLS-1))

