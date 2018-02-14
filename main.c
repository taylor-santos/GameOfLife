#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "definitions.h"
#include "GameState.h"
#include "parser.h"
#include "bot.h"

int main()
{
	struct FastState *state = instantiate_fastState();
	parse(stdin, state);	
	print_fast(state);
	clock_t time = clock();
	const unsigned short state_count = 5;
	struct FastState **state_list = malloc(sizeof(*state_list) * state_count);
	state_list[0] = state;
	for (int i = 0; i < state_count-1; i++) {
		state_list[i+1] = simulate_fast(state_list[i]);
		print_fast(state_list[i+1]);
	}
	time = clock() - time;
	printf("%f\n", (double)time / CLOCKS_PER_SEC * 1000.0);

	struct FastState **curr = &state;
	for (int i=0; i<FIELD_WIDTH; i++){
		(*curr)->changed[i] = 0;
	}
	set_cell(*curr, 5+FIELD_WIDTH+2, 1);
	print_fast(*curr);
	for (int i = 1; i < state_count-1; i++) {
		struct FastState *new_state = simulate_with_prediction(*curr, state_list[i]);
		print_fast(new_state);
		*curr = new_state;
	}

	return 0;
}