#ifndef SKANE_H
#define SKANE_H


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>


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


typedef enum button {
	button_invalid = -1,
	button_untouched,
	button_pressed,
	button_held,
	button_released,
	button_limit
} button_t;


typedef enum skane_state {
	skane_state_invalid = -1,
	skane_state_menu,
	skane_state_game,
	skane_state_high_score,
	skane_state_limit
} skane_state_t;


typedef enum menu_index {
	menu_index_invalid = -1,
	menu_index_play,
	menu_index_high_score,
	menu_index_exit,
	menu_index_limit
} menu_index_t;


typedef enum game_state {
	game_state_invalid = -1,
	game_state_start,
	game_state_play,
	game_state_pause,
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
	float ticks;
	coor_t coor;
} food_t;


typedef struct game {
	game_state_t game_state;
	food_t food;
	snake_t snake;
	float die_ticks;
} game_t;


typedef struct menu {
	menu_index_t menu_index;
} menu_t;


typedef struct skane {
	skane_state_t skane_state;
	menu_t menu;
	game_t game;
} skane_t;


typedef struct input {
	dir_t dir;
	button_t start;
	button_t a;
	button_t b;
} input_t;


//--


bool io_init();
void io_quit();
bool io_sync_input(input_t*);
void io_draw_skane(skane_t*);


//


bool skane_step(const input_t*, skane_t*);


#endif // SKANE_H
