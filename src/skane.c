#include "skane.h"


//--


bool skane_step(const input_t* input, skane_t* skane)
{
	switch (skane->skane_state) {
	case skane_state_menu: {

		int move = menu_index_limit;
		switch (input->dir) {
		case dir_up: move--; break;
		case dir_down: move++; break;
		default: break;
		}
		skane->menu.menu_index += move;
		skane->menu.menu_index %= menu_index_limit;

		return false;
	}
	case skane_state_game: {
		return false;
	}
	case skane_state_high_score: {
		return false;
	}
	default: break;
	}
	assert(false);
}


