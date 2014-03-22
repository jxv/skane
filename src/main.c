#include "skane.h"


//--


int main()
{
	game_t game;

	// Initialize.
	{
		if (!io_init()) {
			return EXIT_FAILURE;
		}
		
		game.game_state = game_state_menu;
	}

	// The main loop.
	bool quit = false;
	while (!quit) {
		input_t input;
		io_sync_input(&input);

		// Step game logic.
		quit |= game_step(&input, &game);

		// Draw.
		io_draw_game(&game);
	}

	// Quit.
	io_quit();

	return EXIT_SUCCESS;
}
