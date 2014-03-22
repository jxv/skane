#include "skane.h"


//--


int main()
{
	input_t input;
	skane_t skane;

	// Initialize.
	{
		if (!io_init()) {
			return EXIT_FAILURE;
		}
		
		input_init(&input);

		skane_init(&skane);
	}

	// The main loop.
	bool quit = false;
	while (!quit) {
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
