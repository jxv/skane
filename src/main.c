#include "skane.h"


//--


int main()
{
	skane_t skane;

	// Initialize.
	{
		if (!io_init()) {
			return EXIT_FAILURE;
		}
		
		skane.skane_state = skane_state_menu;
	}

	// The main loop.
	bool quit = false;
	while (!quit) {
		input_t input;
		quit |= io_sync_input(&input);

		// Step skane logic.
		quit |= skane_step(&input, &skane);

		// Draw.
		io_draw_skane(&skane);
	}

	// Quit.
	io_quit();

	return EXIT_SUCCESS;
}
