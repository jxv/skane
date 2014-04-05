#include "skane.h"


//--


float curtailf(float num)
{
	return num - floorf(num);
}


//--


void input_init(input_t* input)
{
	input->dir = DIR_UP;
	input->dir_state = BUTTON_UNTOUCHED;
	input->start = BUTTON_UNTOUCHED;
	input->a = BUTTON_UNTOUCHED;
	input->b = BUTTON_UNTOUCHED;
}


//


static void game_init_food(food_t*);
static void game_init_snake(snake_t*, float speed);


void game_init(game_t* game)
{
	game->state = GAME_STATE_START;
	game->next_state = GAME_STATE_START;
	game->prev_state = GAME_STATE_START;
	game->score = 0;
	game->ticks = 1.0f;

	game_init_food(&game->food);
	game_init_snake(&game->snake, 0.5f);

}


static void game_init_food(food_t* food)
{
	food->coor = (coor_t) {
		.x = MAP_W - 1,
		.y = MAP_H / 2,
	};
	food->prev_coor = food->coor;
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
	snake->length = SNAKE_BODY_LEN / 2; 
	snake->dir = DIR_RIGHT;

	assert(speed > 0.0f);
	assert(speed <= 1.0f);
	snake->speed = speed;
}


//


void skane_init(skane_t* skane)
{
	skane->prev_state = SKANE_STATE_MENU;
	skane->state = SKANE_STATE_MENU;
	skane->next_state = SKANE_STATE_MENU;
	skane->menu.index = MENU_INDEX_PLAY; 
}


static bool skane_step_game(const input_t*, game_t*);
static bool snake_body_covers(const snake_t*, const coor_t*);


bool skane_step(const input_t* input, skane_t* skane)
{
	skane->prev_state = skane->state;
	skane->state = skane->next_state;

	// Find next state.
	switch (skane->state) {
	case SKANE_STATE_MENU: {
		menu_t* menu = &skane->menu;
		int move = MENU_INDEX_LIMIT;

		if (input->dir_state == BUTTON_PRESSED) {
			switch (input->dir) {
			case DIR_UP: move--; break;
			case DIR_DOWN: move++; break;
			default: break;
			}
		}
		menu->index += move;
		menu->index %= MENU_INDEX_LIMIT;

		//

		if (input->a == BUTTON_PRESSED) {
			switch (menu->index) {
			case MENU_INDEX_PLAY: {
				skane->next_state = SKANE_STATE_GAME;
				game_init(&skane->game);
				break;
			}
			case MENU_INDEX_HIGH_SCORE: skane->next_state = SKANE_STATE_HIGH_SCORE; break;
			case MENU_INDEX_EXIT: return true;
			default: assert(false); break;
			}
		}

		//

		return false;
	}
	case SKANE_STATE_GAME: {
		if (skane_step_game(input, &skane->game)) {
			skane->next_state = SKANE_STATE_MENU;
		}

		//

		return false;
	}
	case SKANE_STATE_HIGH_SCORE: {
		if (input->b == BUTTON_PRESSED || input->a == BUTTON_PRESSED || input->start == BUTTON_PRESSED) {
			skane->next_state = SKANE_STATE_MENU;
		}

		//

		return false;
	}
	default: assert(false); break;
	}
}


static void step_dir(dir_t dir, float* horz, float* vert)
{
	*horz = dir == DIR_LEFT ? -1 : (dir == DIR_RIGHT ? 1 : 0);
	*vert = dir == DIR_UP ? -1 : (dir == DIR_DOWN ? 1 : 0);
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


static dir_t step_snake_ai(const food_t* food, const snake_t* snake)
{
	const coor_t* head = &snake->body[snake->head];
	
	// Decide state.
	ai_state_t state = AI_STATE_OPEN;
	do {
		if ((int) food->coor.x == (int) head->x || (int) food->coor.y == (int) head->y) {
			state = AI_STATE_ON_TARGET;
			break;
		}

	} while (false);

	// Decide movement direction.
	switch (state) {
	case AI_STATE_ON_TARGET: {
		if ((int) food->coor.x == (int) head->x) {
		  if ((int) food->coor.y > (int) head->y) {
		    return DIR_DOWN;
		  }
		  return DIR_UP;
		}

		if ((int) food->coor.x > (int) head->x) {
		  return DIR_RIGHT;
		}
		return DIR_LEFT;
	}
	case AI_STATE_OPEN: {
		coor_t food_v = {
			.x = food->coor.x - food->prev_coor.x,
			.y = food->coor.y - food->prev_coor.y,
		};

		// The mahattan distance between the food and snake's head.
		// Use the distance to create a weight from the food's predicted location and current location.
		float dist = abs(food->coor.x - head->x) + abs(food->coor.y - head->y);
		float inv_speed = 1.0f / snake->speed;
		float rel_dist = dist * inv_speed;
		
		coor_t target = {
			.x = food->coor.x + food_v.x * rel_dist,
			.y = food->coor.y + food_v.y * rel_dist,
		};

		float food_dist = abs(food->coor.x - target.x) + abs(food->coor.y - target.y);
		float head_dist = abs(head->x - target.x) + abs(head->y - target.y);

		// Magic
		float weight = dist / (dist + head_dist + food_dist + 0.0001f);

		coor_t goal = {
		    .x = food->coor.x * (1.0f - weight) + target.x * weight,
		    .y = food->coor.y * (1.0f - weight) + target.y * weight,
		};

		// Given a goal position, choose the best direction.
		if (abs(goal.x - head->x) > abs(goal.y - head->y)) {
			if (goal.x > head->x) {
				return DIR_RIGHT;
			}
			return DIR_LEFT;
		}
		if (goal.y > head->y) {
			return DIR_DOWN;
		}
		return DIR_UP;
		
		
	}
	default: break;
	}
	return DIR_DOWN;
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
	case GAME_STATE_START: {
		if (game->ticks > 0.0f) {
			game->ticks -= SPF;
		} else {
			game->next_state = GAME_STATE_PLAY;
			game->ticks = 0.0f;
		}
		break;
	}
	case GAME_STATE_PLAY: {
		game->ticks += SPF;

		//

		if (input->start == BUTTON_PRESSED) {
			game->next_state = GAME_STATE_PAUSE;
		}

		//

		do {
			const float GAME_TIME = 60.0f;

			if (game->ticks > GAME_TIME) {
				game->next_state = GAME_STATE_GAME_OVER;
				return false;
			}
			snake->length = ((GAME_TIME - game->ticks) * SNAKE_BODY_LEN) / GAME_TIME;
			snake->speed = 0.7f + 0.3f * (game->ticks / GAME_TIME);
			
			// 
			
			snake->dir = step_snake_ai(food, snake);
			
			float horz;
			float vert;
			step_dir(snake->dir, &horz, &vert);
		
			coor_t next_coor = {
				.x = snake->body[snake->head].x + horz * snake->speed,
				.y = snake->body[snake->head].y + vert * snake->speed,
			};

			if (coor_collision(&snake->body[snake->head], &food->coor)) {
				game->next_state = GAME_STATE_GAME_OVER;
				break;
			}

			if (next_coor.x < 0 || next_coor.x >= MAP_W || next_coor.y < 0 || next_coor.y >= MAP_H ||
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
			if (!(input->dir_state == BUTTON_HELD || input->dir_state == BUTTON_PRESSED)) {
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
			
			food->prev_coor = food->prev_coor;
			food->coor = next_coor;
		} while (false);
		
		break;
	}
	case GAME_STATE_PAUSE: {
		if (input->start == BUTTON_PRESSED) {
			game->next_state = GAME_STATE_PLAY;
		}
		break;
	}
	case GAME_STATE_GAME_OVER: {
		if (input->start == BUTTON_PRESSED) {
			return true;
		}
		break;
	}
	default: break;
	}

	return false;
}


