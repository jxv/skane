#include <SDL.h>
#include <SDL_image.h>

//

#include "skane.h"


//--


#define RES_PATH "res/"


//--


static const Uint8 snake_color_r = 0xff;
static const Uint8 snake_color_g = 0xff;
static const Uint8 snake_color_b = 0xff;
static Uint32 snake_color;

//

static const Uint8 food_color_r = 0xff;
static const Uint8 food_color_g = 0xff;
static const Uint8 food_color_b = 0xff;
static Uint32 food_color;

//

static SDL_Surface* screen;

static SDL_Surface* menu_img;
static SDL_Surface* menu_index_img[MENU_INDEX_LIMIT][2];
static SDL_Surface* high_score_img;
static SDL_Surface* pause_img;
static SDL_Surface* game_over_img;
static SDL_Surface* ready_img;
static SDL_Surface* go_img;

//--


static bool io_load_img(const char* path, SDL_Surface** surface)
{
	*surface = IMG_Load(path);
	if (*surface != NULL) {
		return true;
	}
	
	fprintf(stderr, "\"%s\" failed to io_load_img\n", path);
	return false;
}


bool io_init()
{
	{
		if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
			fprintf(stderr, "Unable to initalize SDL.\n");
		return false;
		}
	
		//

		screen = SDL_SetVideoMode(320, 240, 32, SDL_SWSURFACE | SDL_DOUBLEBUF);
		if (!screen) {
			fprintf(stderr, "Unable to initalize screen.\n");
			return false;
		}

		//

		SDL_ShowCursor(SDL_DISABLE);
		SDL_WM_SetCaption("skane", NULL);
	}
	
	//

	{
		snake_color = SDL_MapRGB(screen->format, snake_color_r, snake_color_g, snake_color_b);
		food_color = SDL_MapRGB(screen->format, snake_color_r, snake_color_g, snake_color_b);
	}

	//

	{
		bool loaded = true;
		loaded &= io_load_img(RES_PATH "menu.png", &menu_img);

		//

		loaded &= io_load_img(RES_PATH "menu_play_false.png", &menu_index_img[MENU_INDEX_PLAY][0]);
		loaded &= io_load_img(RES_PATH "menu_play_true.png", &menu_index_img[MENU_INDEX_PLAY][1]);
		loaded &= io_load_img(RES_PATH "menu_high_score_false.png", &menu_index_img[MENU_INDEX_HIGH_SCORE][0]);
		loaded &= io_load_img(RES_PATH "menu_high_score_true.png", &menu_index_img[MENU_INDEX_HIGH_SCORE][1]);
		loaded &= io_load_img(RES_PATH "menu_exit_false.png", &menu_index_img[MENU_INDEX_EXIT][0]);
		loaded &= io_load_img(RES_PATH "menu_exit_true.png", &menu_index_img[MENU_INDEX_EXIT][1]);

		//

		loaded &= io_load_img(RES_PATH "high_score.png", &high_score_img);
		loaded &= io_load_img(RES_PATH "pause.png", &pause_img);
		loaded &= io_load_img(RES_PATH "game_over.png", &game_over_img);
		loaded &= io_load_img(RES_PATH "ready.png", &ready_img);
		loaded &= io_load_img(RES_PATH "go.png", &go_img);

		//

		if (!loaded) {
			return false;
		}
	}

	//

	return true;
}


void io_quit()
{
	SDL_Quit();
}


static button_t update_button(button_t button, bool touch)
{
	switch (button) {
	case BUTTON_UNTOUCHED: return touch ? BUTTON_PRESSED : BUTTON_UNTOUCHED;
	case BUTTON_PRESSED: return touch ? BUTTON_HELD : BUTTON_RELEASED;
	case BUTTON_HELD: return touch ? BUTTON_HELD : BUTTON_RELEASED;
	case BUTTON_RELEASED: return touch ? BUTTON_PRESSED : BUTTON_UNTOUCHED;
	default: assert(false); break;
	}
}


bool io_sync_input(input_t* input)
{
	SDL_Event event;

	//
	
	while(SDL_PollEvent(&event)) {
		switch(event.type) {
		case SDL_QUIT: return true;
		default: break;
		}
	}

	//
	
	const int start_idx = 0;
	const int a_idx = 1;
	const int b_idx = 2;
	bool touches[3] = { false, false, false };
	bool dir_touches[DIR_LIMIT] = { false, false, false, false };
	const Uint8* key_state = SDL_GetKeyState(NULL);

	//

	touches[start_idx] = key_state[SDLK_RETURN];
	touches[a_idx] = key_state[SDLK_LCTRL];
	touches[b_idx] = key_state[SDLK_LALT];

	dir_touches[DIR_LEFT] = key_state[SDLK_LEFT];
	dir_touches[DIR_RIGHT] = key_state[SDLK_RIGHT];
	dir_touches[DIR_UP] = key_state[SDLK_UP];
	dir_touches[DIR_DOWN] = key_state[SDLK_DOWN];

	//

	const dir_t prev_dir = input->dir;
	bool dir_touch = false;

	if (dir_touches[prev_dir]) {
		dir_touch = true;
	} else {
		for (int i = 0; i < DIR_LIMIT; i++) {
			if (dir_touches[i]) {
				input->dir = i;
				dir_touch = true;
				break;
			}
		}
	}

	if (prev_dir == input->dir) {
		input->dir_state = update_button(input->dir_state, dir_touch);
	} else {
		input->dir_state = BUTTON_PRESSED;
	}

	//

	input->start = update_button(input->start, touches[start_idx]);
	input->a = update_button(input->a, touches[a_idx]);
	input->b = update_button(input->b, touches[b_idx]);

	return false;
}


static void io_draw_menu(const menu_t* menu)
{
	SDL_BlitSurface(menu_img, NULL, screen, NULL);

	for (int i = 0; i < MENU_INDEX_LIMIT; i++) {
		SDL_BlitSurface(menu_index_img[i][menu->index == i], NULL, screen, NULL);
	}
}



static void io_draw_game_play(const game_t* game)
{
	const snake_t* snake = &game->snake;
	const food_t* food = &game->food; 

	for (int idx = snake->head, len = 0;
			len < snake->length;
			idx += 1 + SNAKE_BODY_LEN, idx %= SNAKE_BODY_LEN, len++) {
		SDL_Rect rect = {
			.x = (int) snake->body[idx].x,
			.y = (int) snake->body[idx].y,
			.w = 1,
			.h = 1,
		};
		// The fade has a linear range of [1.0f,0.3f).
		float fade = (snake->length * 0.1f + (snake->length - len) * 0.9f) / snake->length;
		SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0xff * fade, 0xff * fade, 0xff * fade));
	}

	//	

	SDL_Rect rect = {
		.x = (int) food->coor.x,
		.y = (int) food->coor.y,
		.w = 1,
		.h = 1,
	};
	SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0xff, 0x00, 0x00));
}


static void io_draw_game(const game_t* game)
{
	switch (game->state) {
	case GAME_STATE_START:
		io_draw_game_play(game);
		SDL_BlitSurface(ready_img, NULL, screen, NULL);
		break;
	case GAME_STATE_PLAY:
		io_draw_game_play(game);
		if (game->ticks <= 1.0f) 
			SDL_BlitSurface(go_img, NULL, screen, NULL);
		break;
	case GAME_STATE_PAUSE:
		io_draw_game_play(game);
		SDL_BlitSurface(pause_img, NULL, screen, NULL);
		break;
	case GAME_STATE_GAME_OVER:
		io_draw_game_play(game);
		SDL_BlitSurface(game_over_img, NULL, screen, NULL);
		break;
	default: assert(false); break;
	}
}


static void io_draw_high_score(int high_score)
{
	SDL_BlitSurface(high_score_img, NULL, screen, NULL);
}


void io_draw_skane(const skane_t* skane)
{
	SDL_FillRect(screen, NULL, 0);

	switch (skane->state) {
	case SKANE_STATE_MENU: io_draw_menu(&skane->menu); break;
	case SKANE_STATE_GAME: io_draw_game(&skane->game); break;
	case SKANE_STATE_HIGH_SCORE: io_draw_high_score(skane->high_score); break;
	default: assert(false); break;
	}

	SDL_Flip(screen);
	SDL_Delay(1000 / FPS);
}


