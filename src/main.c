#include "skane.h"

int main()
{
	input_t input;
	game_t game;

	io_init();
	game_init(&game);

	bool quit = false;
	do {
		io_sync_input(&input);
		quit |= game_step(&input, &game);
		io_draw_game(&game);
	} while (!quit);

	io_quit();

	return 0;
}
