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
	clock_t time = clock();
	short* move = best_move(state, 3);
	time = clock() - time;
	fprintf(stderr, "%d\n", move[0]);
	if (move[0]) {
		fprintf(stderr, "%d\n", move[1]);
	}
	free(move);
	printf("%f\n", (double)time / CLOCKS_PER_SEC * 1000.0);
	/*
	//print_fast(state);
	const unsigned short state_count = 600;
	struct FastState **state_list = malloc(sizeof(*state_list) * state_count);
	state_list[0] = state;
	for (int i = 0; i < state_count-1; i++) {
		state_list[i+1] = simulate_fast(state_list[i]);
		//print_fast(state_list[i+1]);
	}
	struct FastState *copy = copy_fastState(state, true);
	struct FastState **curr = &copy;
	set_cell(*curr, 282, 1);
	set_cell(*curr, 301, 1);
	clock_t time = clock();
	for (int i = 1; i < state_count - 1; i++) {
		struct FastState *new_state = simulate_fast(*curr);
		//print_fast(new_state);
		//verify_fastState(new_state);
		*curr = new_state;
	}
	time = clock() - time;
	printf("%f\n", (double)time / CLOCKS_PER_SEC * 1000.0);

	copy = copy_fastState(state, false);
	curr = &copy;
	set_cell(*curr, 282, 1);
	set_cell(*curr, 301, 1);
	time = clock();
	for (int i = 1; i < state_count-1; i++) {
		struct FastState *new_state = simulate_with_prediction(*curr, state_list[i]);
		//struct FastState *new_state = simulate_fast(*curr);
		//print_fast(new_state);
		//verify_fastState(new_state);
		
		*curr = new_state;
	}
	time = clock() - time;
	printf("%f\n", (double)time / CLOCKS_PER_SEC * 1000.0);
	*/
	return 0;
}