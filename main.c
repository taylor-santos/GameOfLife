#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include "definitions.h"
#include "GameState.h"
#include "parser.h"
#include "bot.h"

unsigned char your_botid = 0;

int main()
{
	extern int count[8][8];
	extern int total[8][8];
	extern int sum;
	sum = 0;
	for (int c = 0; c < 8; c++) {
		for (int d = 0; d < 8; d++) {
			count[c][d] = 0;
			total[c][d] = 0;
		}
	}
	for (int test = 0; test < 1000; test++) {
		struct State *state = instantiate_state();
		for (int n = 0; n < 50; n++) {
			int x = rand() % FIELD_WIDTH;
			int y = rand() % FIELD_HEIGHT;
			int index = x + y * FIELD_WIDTH;
			int anti_index = 287 - index;
			set_cell(state, index, 1);
			set_cell(state, anti_index, 2);
		}
		unsigned char depth = 4;
		struct State **predictions = malloc(sizeof(*predictions) * depth);
		predictions[0] = simulate(state);
		for (int i = 1; i < depth; i++) {
			predictions[i] = simulate(predictions[i - 1]);
		}
		free(state->changed);
		state->changed = calloc(sizeof(*state->changed), FIELD_WIDTH);
		int *move = minimax(state, predictions, your_botid, depth, SHRT_MIN + 1, SHRT_MAX);
		for (int i = 0; i < depth; i++)
			free_state(&predictions[i]);
		free(predictions);
		free(move);
		extern int count[8][8];
		extern int total[8][8];
		for (int b = 0; b < 8; b++) {
			for (int a = 0; a < 8; a++) {
				double n = (double)(count[a][b]) / sum;
				fprintf(stderr, "%f\t", n);
			}
			fprintf(stderr, "\n");
		}
		fprintf(stderr, "\n");
		free_state(&state);
	}
	/*
	while (true) {
		parse(stdin, state);
	}
	*/
	return 0;
}