#ifndef SKANE_H
#define SKANE_H


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>


//--


#define MAP_W	320
#define MAP_H	240
#define SNAKE_BODY_LEN (MAP_H*2)

#define FPS 60
#define SPF (1.0f / (float)FPS)


//--


typedef enum dir
{
	DIR_INVALID = -1,
	DIR_LEFT,
	DIR_RIGHT,
	DIR_UP,
	DIR_DOWN,
	DIR_LIMIT	
} dir_t;


typedef enum button
{
	BUTTON_INVALID = -1,
	BUTTON_UNTOUCHED,
	BUTTON_PRESSED,
	BUTTON_HELD,
	BUTTON_RELEASED,
	BUTTON_LIMIT,
} button_t;


typedef enum skane_state
{
	SKANE_STATE_INVALID = -1,
	SKANE_STATE_MENU,
	SKANE_STATE_GAME,
	SKANE_STATE_HIGH_SCORE,
	SKANE_STATE_LIMIT,
} skane_state_t;


typedef enum menu_index
{
	MENU_INDEX_INVALID = -1,
	MENU_INDEX_PLAY,
	MENU_INDEX_HIGH_SCORE,
	MENU_INDEX_EXIT,
	MENU_INDEX_LIMIT,
} menu_index_t;


typedef enum game_state
{
	GAME_STATE_INVALID = -1,
	GAME_STATE_START,
	GAME_STATE_PLAY,
	GAME_STATE_PAUSE,
	GAME_STATE_GAME_OVER,
	GAME_STATE_LIMIT,
} game_state_t;


typedef enum ai_state
{
	AI_STATE_INVALID = -1,
	AI_STATE_ON_TARGET,
	AI_STATE_OPEN,
	AI_STATE_NEAR,
	AI_STATE_PARTIAL_TRAP,
	AI_STATE_AJAR_TRAP,
	AI_STATE_FULL_TRAP,
	AI_STATE_LIMIT
} ai_state_t;


//


typedef struct coor
{
	float x;
	float y;
} coor_t;


typedef struct snake
{
	dir_t dir;
	int head;
	int length;
	float speed;
	coor_t body[SNAKE_BODY_LEN];
} snake_t;


typedef struct food
{
	coor_t prev_coor;
	coor_t coor;
} food_t;


typedef struct game
{
	game_state_t state;
	game_state_t next_state;
	game_state_t prev_state;
	food_t food;
	snake_t snake;
	int score;
	float ticks;
} game_t;


typedef struct menu
{
	menu_index_t index;
} menu_t;


typedef struct skane
{
	skane_state_t state;
	skane_state_t prev_state; 
	skane_state_t next_state; 
	menu_t menu;
	game_t game;
	int high_score;
} skane_t;


typedef struct input
{
	dir_t dir;
	button_t dir_state;
	button_t start;
	button_t a;
	button_t b;
} input_t;


//--


bool io_init();
void io_quit();
bool io_sync_input(input_t*);
void io_draw_skane(const skane_t*);

//

void input_init(input_t*);
void skane_init(skane_t*);
bool skane_step(const input_t*, skane_t*);


#endif // SKANE_H
