#ifndef SKANE_H
#define SKANE_H


#include <stdio.h>
#include <stdbool.h>


//--


#define WORLD_WIDTH	320
#define WORLD_HEIGHT	240


//--


typedef enum dir {
	dir_invalid = -1,
	dir_left,
	dir_right,
	dir_up,
	dir_down,
	dir_limit
} dir_t;

//

typedef struct coor {
	int x;
	int y;
} coor_t;


typedef struct snake {
	int head;
	int length;
	coor_t body[WORLD_WIDTH * WORLD_HEIGHT];
} snake_t;


typedef struct food {
	int ticks;
	coor_t coor;
} food_t;


typedef struct game {
	food_t food;
	snake_t snake;
} game_t;


typedef struct input {
	dir_t dir;
} input_t;


//--

void io_init();
void io_quit();
void io_sync_input(input_t*);
void io_draw_game(game_t*);

//

void game_init(game_t*);
bool game_step(const input_t*, game_t*);


#endif // SKANE_H
