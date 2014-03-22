#include "skane.h"


//--


static void game_init_food(food_t*);
static void game_init_snake(snake_t*);


void game_init(game_t* game)
{
	game->state = game_state_start;
	game->next_state = game_state_start;
	game->prev_state = game_state_start;
	game->score = 0;
	game->ticks = 1.0f;

	game_init_food(&game->food);
	game_init_snake(&game->snake);

}

static void game_init_food(food_t* food)
{
	food->coor.x = MAP_W - 1;
	food->coor.y = MAP_H / 2;
}

static void game_init_snake(snake_t* snake)
{
	int idx = 0;
	for (int y = MAP_H - 1; y >= 0; y--) {
		snake->body[idx] = (coor_t){.x = 1, .y = y};
	
		idx++;
		idx %= SNAKE_BODY_LEN;
	}
	
	for (int y = 0; y < MAP_H; y++) {
		snake->body[idx] = (coor_t){.x = 0, .y = y};
	
		idx++;
		idx %= SNAKE_BODY_LEN;
	}

	snake->head = idx;
	snake->length = MAP_H * 2; 
	snake->dir = dir_right;
}


//


static bool skane_step_game(const input_t*, game_t*);


bool skane_step(const input_t* input, skane_t* skane)
{
	skane->prev_state = skane->state;
	skane->state = skane->next_state;

	// Find next state.
	switch (skane->state) {
	case skane_state_menu: {
		menu_t* menu = &skane->menu;
		int move = menu_index_limit;
		switch (input->dir) {
		case dir_up: move--; break;
		case dir_down: move++; break;
		default: break;
		}
		menu->index += move;
		menu->index %= menu_index_limit;

		//

		if (input->a == button_pressed) {
			switch (menu->index) {
			case menu_index_play: {
				skane->next_state = skane_state_game;
				game_init(&skane->game);
				break;
			}
			case menu_index_high_score: skane->next_state = skane_state_high_score; break;
			case menu_index_exit: return true;
			default: assert(false); break;
			}
		}

		//

		return false;
	}
	case skane_state_game: {
		if (skane_step_game(input, &skane->game)) {
			skane->next_state = skane_state_menu;
		}
		return false;

		return false;
	}
	case skane_state_high_score: {
		return false;
	}
	default: break;
	}
	assert(false);
}


static bool skane_step_game(const input_t* input, game_t* game)
{
	game->prev_state = game->state;
	game->state = game->next_state;

	//

	snake_t* snake = &game->snake;
	//food_t* food = &game->food;

	switch (game->state) {
	case game_state_start: {
		if (game->ticks > 0.0f) {
			game->ticks -= SPF;
		} else {
			game->next_state = game_state_play;
			game->ticks = 0.0f;
		}
		break;
	}
	case game_state_play: {
		game->ticks += SPF;

		dir_t dir = snake->dir;
		int horz = dir == dir_left ? -1 : (dir == dir_right ? 1 : 0);
		int vert = dir == dir_up ? -1 : (dir == dir_down ? 1 : 0);
		
		coor_t next_coor = {
			.x = snake->body[snake->head].x + horz,
			.y = snake->body[snake->head].y + vert,
		};

		if (next_coor.x < 0 || next_coor.x >= MAP_W || next_coor.y < 0 || next_coor.y >= MAP_H) {
			return false;
		}

		snake->head += -1 + SNAKE_BODY_LEN;
		snake->head %= SNAKE_BODY_LEN;

		snake->body[snake->head] = next_coor;
		break;
	}
	case game_state_pause: {
		break;
	}
	case game_state_game_over: {
		break;
	}
	default: break;
	}

	return false;
}


