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
	extern int count[5][5][5];
	for (int i=0; i<5; i++){
		for (int j=0; j<5; j++){
			for (int k=0; k<5; k++){
				count[i][j][k] = 0;
			}
		}
	}
	/*
	extern int count[2][9][9];
	for (int a = 0; a<2; a++) {
		for (int b = 0; b < 9; b++) {
			for (int c = 0; c < 9; c++) {
				for (int d = 0; d < 8; d++) {
					count[a][b][c] = 0;
				}
			}
		}
	}
	*/
	for (int test = 0; test < 10000; test++) {
		struct State *state = instantiate_state();
		for (int n = 0; n < 50; n++) {
			int x = rand() % FIELD_WIDTH;
			int y = rand() % FIELD_HEIGHT;
			int index = x + y * FIELD_WIDTH;
			int anti_index = 287 - index;
			set_cell(state, index, 1);
			set_cell(state, anti_index, 2);
		}
		unsigned char depth = 2;
		struct State **predictions = malloc(sizeof(*predictions) * depth);
		predictions[0] = simulate(state);
		for (int i = 1; i < depth; i++) {
			predictions[i] = simulate(predictions[i - 1]);
		}
		free(state->changed);
		state->changed = calloc(sizeof(*state->changed), FIELD_WIDTH);
		int *move = minimax(state, (const struct State **)predictions, 0, depth, SHRT_MIN + 1, SHRT_MAX);
		for (int i = 0; i < depth; i++)
			free_state(&predictions[i]);
		free(predictions);
		free(move);
		for (int k=0; k<5; k++){
			for (int i=0; i<5; i++){
				for (int j=0; j<5; j++){
					fprintf(stderr, "%d ", count[i][j][k]);
				}
				fprintf(stderr, "\t");
			}
			fprintf(stderr, "\n");
		}
		fprintf(stderr, "\n");
	}
	/*
	struct State *state = instantiate_state();
	while (true) {
		parse(stdin, state);
	}
	*/
	return 0;
}