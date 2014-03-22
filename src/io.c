#include <SDL.h>
#include <SDL_image.h>

//

#include "skane.h"


//--

static SDL_Surface* screen;


bool io_init()
{

	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		fprintf(stderr, "Unable to initalize SDL.");
		return false;
	}

	SDL_ShowCursor(SDL_DISABLE);

	SDL_WM_SetCaption("skane", NULL);

	screen = SDL_SetVideoMode(320, 240, 32, SDL_SWSURFACE);
	if (!screen) {
		fprintf(stderr, "Unable to initalize screen.");
		return false;
	}

	return true;
}


void io_quit()
{
	SDL_Quit();
}


static button_t update_button(button_t button, bool touch) {
	static const button_t next[button_limit][2] = {
		[button_untouched] = {button_pressed, button_untouched},
		[button_pressed] = {button_held, button_released},
		[button_held] = {button_held, button_released},
		[button_released] = {button_pressed, button_untouched},
	};
	
	assert(button > button_invalid);
	assert(button < button_limit);
	return next[button][!!touch];
}


bool io_sync_input(input_t* input)
{
	SDL_Event event;

	const int start_idx = 0;
	const int a_idx = 1;
	const int b_idx = 2;
	bool touches[3] = { false, false, false };
	
	while(SDL_PollEvent(&event)) {
		switch(event.type) {
		case SDL_QUIT: return true;
		case SDL_KEYDOWN: {
			if (event.key.keysym.sym == SDLK_ESCAPE)  {
				return true;
			}
			
			switch (event.key.keysym.sym) {
			case SDLK_UP: input->dir = dir_up; break;
			case SDLK_LEFT: input->dir = dir_left; break;
			case SDLK_RIGHT: input->dir = dir_right; break;
			case SDLK_DOWN: input->dir = dir_down; break;
			case SDLK_RETURN: touches[start_idx] = true; break;
			case SDLK_LCTRL: touches[a_idx] = true; break;
			case SDLK_LALT: touches[b_idx] = true; break;
			default: break;
			}

			break;
		}
		default: break;
		}
	}

	input->start = update_button(input->start, touches[start_idx]);
	input->a = update_button(input->a, touches[a_idx]);
	input->b = update_button(input->b, touches[b_idx]);

	return false;
}


void io_draw_skane(skane_t* skane)
{
}


