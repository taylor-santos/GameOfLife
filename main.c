#include <stdio.h>
#include <stdlib.h>
#include "definitions.h"
#include "GameState.h"
#include "parser.c"

struct GameState *instantiate_state() 
{
	struct GameState *state = malloc(sizeof(*state));
	state->timebank = 10000;
	state->round = 0;
	state->field = malloc(sizeof(*(state->field)) * FIELD_WIDTH);
	state->adj_count0 = malloc(sizeof(*(state->adj_count0)) * FIELD_WIDTH);
	state->adj_count1 = malloc(sizeof(*(state->adj_count1)) * FIELD_WIDTH);
	for (int x = 0; x < FIELD_WIDTH; x++) {
		state->field[x] = calloc(sizeof(*((state->field)[x])), FIELD_HEIGHT);
		state->adj_count0[x] = calloc(sizeof(*((state->adj_count0)[x])), FIELD_HEIGHT);
		state->adj_count1[x] = calloc(sizeof(*((state->adj_count1)[x])), FIELD_HEIGHT);
	}
	return state;
}

void clear_state(struct GameState **state)
{
	for (int x = 0; x < FIELD_WIDTH; x++) {
		free((*state)->field[x]);
		free((*state)->adj_count0[x]);
		free((*state)->adj_count1[x]);
	}
	free((*state)->field);
	free((*state)->adj_count0);
	free((*state)->adj_count1);
	free(*state);
	*state = NULL;
}

int main()
{
	struct GameState *state = instantiate_state();
	parse(stdin, state);
	for (int i=0; i<10; i++){
		for (int y = 0; y < FIELD_HEIGHT; y++) {
			for (int x = 0; x < FIELD_WIDTH; x++) {
				int adj = state->adj_count0[x][y] + state->adj_count1[x][y];
				if (state->field[x][y] != -1) {
					if (adj < 2 || adj > 3)
						printf("O");
					else
						printf("O");
				}
				else {
					if (adj == 3)
						printf(" ");
					else
						printf(" ");
				}
			}
			printf("   ");
			for (int x = 0; x < FIELD_WIDTH; x++) {
				int adj = state->adj_count0[x][y] + state->adj_count1[x][y];
				if (adj == 1){
					printf("%c",'░');
				}else if (adj == 2){
					printf("%c",177);
				}else if (adj == 3){
					printf("%c",178);
				}else if (adj >= 4){
					printf("%c", 219);
				}else{
					printf(" ");
				}
			}
			printf("\n");
		}
		printf("------\n");


		int **changed = malloc(sizeof(*changed) * FIELD_WIDTH);
		for (int x = 0; x < FIELD_WIDTH; x++) {
			changed[x] = calloc(sizeof(*(changed[x])), FIELD_HEIGHT);
		}

		for (int y = 0; y < FIELD_HEIGHT; y++) {
			for (int x = 0; x < FIELD_WIDTH; x++) {
				if (state->field[x][y] == -1) {
					if (state->adj_count0[x][y] + state->adj_count1[x][y] == 3) {
						if (state->adj_count0[x][y] > state->adj_count1[x][y]) {
							state->field[x][y] = 0;
							changed[x][y] = 1;
						}
						else {
							state->field[x][y] = 1;
							changed[x][y] = 2;
						}
					}
				}
				else {
					if (state->adj_count0[x][y] + state->adj_count1[x][y] < 2) {
						if (state->field[x][y] == 0)
							changed[x][y] = -1;
						else 
							changed[x][y] = -2;
						state->field[x][y] = -1;
					}
					else if (state->adj_count0[x][y] + state->adj_count1[x][y] > 3) {
						if (state->field[x][y] == 0)
							changed[x][y] = -1;
						else 
							changed[x][y] = -2;
						state->field[x][y] = -1;
					}
				}
			}
		}
		for (int y = 0; y < FIELD_HEIGHT; y++) {
			for (int x = 0; x < FIELD_WIDTH; x++) {
				for (int Y = y - 1; Y <= y + 1; Y++) {
					for (int X = x - 1; X <= x + 1; X++) {
						if ((X != x || Y != y) && 
							X >= 0 && 
							X < FIELD_WIDTH && 
							Y >= 0 && 
							Y < FIELD_HEIGHT) {
							int change = changed[X][Y];
							if (change == -2)
								state->adj_count1[x][y]--;
							else if (change == -1)
								state->adj_count0[x][y]--;
							else if (change == 1)
								state->adj_count0[x][y]++;
							else if (change == 2)
								state->adj_count1[x][y]++;
						}
					}
				}
			}
		}
	}
	clear_state(&state);
	return 0;
}