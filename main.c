#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "definitions.h"
#include "GameState.h"
#include "parser.h"
#include "bot.h"

int main()
{
	struct GameState *state = instantiate_state();
	parse(stdin, state);

	struct FastState *fast_state = instantiate_fastState();
	/*
	print_fast(state);
	char buffer[BUFFER_SIZE];
	for (int i = 0; i<BUFFER_SIZE; i++) {
		buffer[i] = 0;
	}
	if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
		fprintf(stderr, "Failed to read from input.\n");
		exit(BAD_DATA);
	}
	*/
	for (int y = 0; y < FIELD_HEIGHT; y++) {
		for (int x = 0; x < FIELD_WIDTH; x++) {
			//char c = buffer[18 + 2 * (FIELD_WIDTH*y + x)];
			if (state->field[x + y*FIELD_WIDTH] == 0) {
				fast_state->count0++;
				int mul = 1;
				for (int Y = y - 1; Y <= y + 1; Y++) {
					for (int X = x - 1; X <= x + 1; X++) {
						fast_state->field[(X+1) + (Y+1)*FIELD_WIDTH] += mul;
						mul *= 3;
					}
				}
			}
			else if (state->field[x + y*FIELD_WIDTH] == 1) {
				fast_state->count1++;
				int mul = 1;
				for (int Y = y - 1; Y <= y + 1; Y++) {
					for (int X = x - 1; X <= x + 1; X++) {
						fast_state->field[(X+1) + (Y+1)*FIELD_WIDTH] += 2*mul;
						mul *= 3;
					}
				}
			}
		}
	}
	clock_t time = clock();
	for(int i=0; i<10000; i++){
		//print_fast(fast_state);
		simulate_fast(fast_state);
	}
	time = clock() - time;
	printf("%f\n", (double)time/CLOCKS_PER_SEC*1000.0);
	/*
	struct GameState *state = instantiate_state();
	while (1) {
		parse(stdin, state);
	}
	clear_state(&state);
	*/
	
	return 0;
}