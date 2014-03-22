#include <SDL.h>
#include <SDL_image.h>

//

#include "skane.h"


//--

static SDL_Surface* screen;


bool io_init()
{
	screen = NULL;
	return true;
}


void io_quit()
{
}


void io_sync_input(input_t* input)
{
}


void io_draw_game(game_t* game)
{
}


