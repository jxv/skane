#include "skane.h"


//--


bool skane_step(const input_t* input, skane_t* skane)
{
	switch (skane->skane_state) {
	case skane_state_menu: {
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


