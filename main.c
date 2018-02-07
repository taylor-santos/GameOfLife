#include <stdio.h>
#include <stdlib.h>
#include "definitions.h"
#include "GameState.c"
#include "parser.c"
#include "bot.c"

int main()
{
	struct GameState *state = instantiate_state();
	parse(stdin, state);

	for (int i=0; i<10000; i++){
		print_field(state);
		struct GameState *new_state = simulate(state);
		clear_state(&state);
		state = new_state;	
	}
	return 0;
}