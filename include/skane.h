#ifndef SKANE_H
#define SKANE_H


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


//--


#define MAP_W	320
#define MAP_H	240


//--


typedef enum dir {
	dir_invalid = -1,
	dir_left,
	dir_right,
	dir_up,
	dir_down,
	dir_limit
} dir_t;


typedef enum game_state {
	game_state_invalid = -1,
	game_state_menu,
	game_state_play,
	game_state_pause,
	game_state_die,
	game_state_game_over,
	game_state_limit
} game_state_t;


//


typedef struct coor {
	int x;
	int y;
} coor_t;


typedef struct snake {
	int head;
	int length;
	coor_t body[MAP_W * MAP_H];
} snake_t;


typedef struct food {
	int ticks;
	coor_t coor;
} food_t;


typedef struct game {
	game_state_t game_state;
	food_t food;
	snake_t snake;
} game_t;


typedef struct input {
	dir_t dir;
} input_t;


//--

bool io_init();
void io_quit();
void io_sync_input(input_t*);
void io_draw_game(game_t*);

//

bool game_step(const input_t*, game_t*);


#endif // SKANE_H
