#ifndef GAMESTATE
#define GAMESTATE

#include "definitions.h"
#include <stdlib.h>
#include <string.h>

struct GameState
{
	int timebank;
	int round;
	int your_botid;
	int count0;
	int count1;
	int *field;
	int *adj_count0;
	int *adj_count1;
};

struct GameState *instantiate_state() 
{
	struct GameState *state = malloc(sizeof(*state));
	state->timebank = 10000;
	state->round = 0;
	state->count0 = 0;
	state->count1 = 1;
	state->field = malloc(sizeof(*(state->field)) * FIELD_WIDTH * FIELD_HEIGHT);
	for (int i=0; i<FIELD_WIDTH*FIELD_HEIGHT; i++){
		state->field[i] = -1;
	}
	state->adj_count0 = calloc(sizeof(*(state->adj_count0)), FIELD_WIDTH * FIELD_HEIGHT);
	state->adj_count1 = calloc(sizeof(*(state->adj_count1)), FIELD_WIDTH * FIELD_HEIGHT);
	return state;
}

struct GameState *copy(struct GameState *state){
	struct GameState *new_state = malloc(sizeof(*new_state));
	new_state->timebank = state->timebank;
	new_state->round = state->round;
	new_state->your_botid = state->your_botid;
	new_state->count0 = state->count0;
	new_state->count1 = state->count1;
	new_state->field = malloc(sizeof(*(new_state->field)) * FIELD_WIDTH * FIELD_HEIGHT);
	memcpy(new_state->field, state->field, sizeof(*(state->field)) * FIELD_WIDTH * FIELD_HEIGHT);
	new_state->adj_count0 = malloc(sizeof(*(new_state->adj_count0)) * FIELD_WIDTH * FIELD_HEIGHT);
	memcpy(new_state->adj_count0, state->adj_count0, sizeof(*(state->adj_count1)) * FIELD_WIDTH * FIELD_HEIGHT);
	new_state->adj_count1 = malloc(sizeof(*(new_state->adj_count1)) * FIELD_WIDTH * FIELD_HEIGHT);
	memcpy(new_state->adj_count1, state->adj_count1, sizeof(*(state->adj_count1)) * FIELD_WIDTH * FIELD_HEIGHT);
	return new_state;
}

void clear_state(struct GameState **state)
{
	free((*state)->field);
	free((*state)->adj_count0);
	free((*state)->adj_count1);
	free(*state);
	*state = NULL;
}

void print_field(struct GameState *state){
	for (int y = 0; y < FIELD_HEIGHT; y++) {
			for (int x = 0; x < FIELD_WIDTH; x++) {
				int adj = state->adj_count0[x + y*FIELD_WIDTH] + state->adj_count1[x + y*FIELD_WIDTH];
				if (state->field[x + y * FIELD_WIDTH] != -1) {
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
				int adj = state->adj_count0[x + y*FIELD_WIDTH] + state->adj_count1[x + y*FIELD_WIDTH];
				if (adj == 1){
					printf("%c",'-');
				}else if (adj == 2){
					printf("%c", '+');
				}else if (adj == 3){
					printf("%c",'|');
				}else if (adj >= 4){
					printf("%c", '0');
				}else{
					printf(" ");
				}
			}
			printf("\n");
		}
		printf("------\n");
}

#endif