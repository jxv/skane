#include "skane.h"


//--


float curtailf(float num)
{
	return num - floorf(num);
}


//--


void input_init(input_t* input)
{
	input->dir = dir_up;
	input->dir_state = button_untouched;
	input->start = button_untouched;
	input->a = button_untouched;
	input->b = button_untouched;
}


//


static void game_init_food(food_t*);
static void game_init_snake(snake_t*, float speed);


void game_init(game_t* game)
{
	game->state = game_state_start;
	game->next_state = game_state_start;
	game->prev_state = game_state_start;
	game->score = 0;
	game->ticks = 1.0f;

	game_init_food(&game->food);
	game_init_snake(&game->snake, 0.9f);

}

static void game_init_food(food_t* food)
{
	food->coor.x = MAP_W - 1;
	food->coor.y = MAP_H / 2;
}

static void game_init_snake(snake_t* snake, float speed)
{
	int idx = 0;
	for (int y = MAP_H - 1; y >= 0 && idx < SNAKE_BODY_LEN; y -= speed) {
		snake->body[idx] = (coor_t){.x = 1, .y = y};
		//
		idx++;
	}
	
	for (int y = 0; y < MAP_H && idx < SNAKE_BODY_LEN; y += speed) {
		snake->body[idx] = (coor_t){.x = 0, .y = y};
		//
		idx++;
	}

	snake->head = 0; 
	snake->length = SNAKE_BODY_LEN; 
	snake->dir = dir_right;

	assert(speed > 0.0f);
	assert(speed <= 1.0f);
	snake->speed = speed;
	snake->ai_state = ai_state_far;
}


//


void skane_init(skane_t* skane)
{
	skane->prev_state = skane_state_menu;
	skane->state = skane_state_menu;
	skane->next_state = skane_state_menu;
	skane->menu.index = menu_index_play;
}


static bool skane_step_game(const input_t*, game_t*);
static bool snake_body_covers(const snake_t*, const coor_t*);


bool skane_step(const input_t* input, skane_t* skane)
{
	skane->prev_state = skane->state;
	skane->state = skane->next_state;

	// Find next state.
	switch (skane->state) {
	case skane_state_menu: {
		menu_t* menu = &skane->menu;
		int move = menu_index_limit;

		if (input->dir_state == button_pressed) {
			switch (input->dir) {
			case dir_up: move--; break;
			case dir_down: move++; break;
			default: break;
			}
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

		//

		return false;
	}
	case skane_state_high_score: {
		if (input->b == button_pressed || input->a == button_pressed || input->start == button_pressed) {
			skane->next_state = skane_state_menu;
		}

		//

		return false;
	}
	default: assert(false); break;
	}
}


static void step_dir(dir_t dir, float* horz, float* vert)
{
	*horz = dir == dir_left ? -1 : (dir == dir_right ? 1 : 0);
	*vert = dir == dir_up ? -1 : (dir == dir_down ? 1 : 0);
}


static bool coor_collision(const coor_t* c, const coor_t* d)
{
	return (int)c->x == (int)d->x && (int)c->y == (int)d->y;
}

static bool snake_body_covers(const snake_t* snake, const coor_t* coor)
{
	for (int len = 0, idx = snake->head;
	     len < snake->length;
	     len++, idx += 1 + SNAKE_BODY_LEN, idx %= SNAKE_BODY_LEN) { 
		if (coor_collision(&snake->body[idx], coor)) {
			return true;
		}
	}

	return false;
}


static void step_snake_ai(const food_t* food, const snake_t* snake)
{
}


static bool skane_step_game(const input_t* input, game_t* game)
{
	game->prev_state = game->state;
	game->state = game->next_state;

	//

	snake_t* snake = &game->snake;
	food_t* food = &game->food;
	
	//

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

		//

		if (input->start == button_pressed) {
			game->next_state = game_state_pause;
		}

		//

		do {
			step_snake_ai(food, snake);
			
			float horz;
			float vert;
			step_dir(snake->dir, &horz, &vert);
		
			coor_t next_coor = {
				.x = snake->body[snake->head].x + horz * snake->speed,
				.y = snake->body[snake->head].y + vert * snake->speed,
			};

			if (next_coor.x < 0 || next_coor.x >= MAP_W || next_coor.y < 0 || next_coor.y >= MAP_H ||
			    coor_collision(&snake->body[snake->head], &food->coor) ||
			    (snake_body_covers(snake, &next_coor) &&
			     !coor_collision(&snake->body[snake->head], &next_coor))) {
				break;
			}

			snake->head += -1 + SNAKE_BODY_LEN;
			snake->head %= SNAKE_BODY_LEN;

			snake->body[snake->head] = next_coor;

		} while (false);

		//

		do {
			if (!(input->dir_state == button_held || input->dir_state == button_pressed)) {
				break;
			}

			float horz;
			float vert;
			step_dir(input->dir, &horz, &vert);
		
			coor_t next_coor = {
				.x = food->coor.x + horz,
				.y = food->coor.y + vert,
			};

			if (next_coor.x < 0 || next_coor.x >= MAP_W || next_coor.y < 0 || next_coor.y >= MAP_H ||
			    snake_body_covers(snake, &next_coor)) {
				break;
			}
			
			food->coor = next_coor;
		} while (false);
		
		break;
	}
	case game_state_pause: {
		if (input->start == button_pressed) {
			game->next_state = game_state_play;
		}
		break;
	}
	case game_state_game_over: {
		if (input->start == button_pressed) {
			return true;
		}
		break;
	}
	default: break;
	}

	return false;
}


