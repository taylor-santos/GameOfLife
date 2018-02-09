#ifndef GAMESTATE
#define GAMESTATE

#include "GameState.h"
#include "definitions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct GameState *instantiate_state() 
{
	struct GameState *state = malloc(sizeof(*state));
	state->timebank = 10000;
	state->round = 0;
	state->count0 = 0;
	state->count1 = 0;
	state->your_botid = 0;
	state->field = malloc(sizeof(*(state->field)) * FIELD_WIDTH * FIELD_HEIGHT);
	for (int i=0; i<FIELD_WIDTH*FIELD_HEIGHT; i++){
		state->field[i] = -1;
	}
	state->adj_count0 = calloc(sizeof(*(state->adj_count0)), FIELD_WIDTH * FIELD_HEIGHT);
	state->adj_count1 = calloc(sizeof(*(state->adj_count1)), FIELD_WIDTH * FIELD_HEIGHT);
	return state;
}

void clear_cell(struct GameState *state, int x, int y) {
	int owner = state->field[x + y * FIELD_WIDTH];
	if (owner == 0) {
		state->count0--;
		for (int Y = y - 1; Y <= y + 1; Y++) {
			for (int X = x - 1; X <= x + 1; X++) {
				if ((Y != y || X != x) && Y >= 0 && Y < FIELD_HEIGHT && X >= 0 && X < FIELD_WIDTH) {
					state->adj_count0[X + Y * FIELD_WIDTH]--;
				}
			}
		}
	}
	else if (owner == 1) {
		state->count1--;
		for (int Y = y - 1; Y <= y + 1; Y++) {
			for (int X = x - 1; X <= x + 1; X++) {
				if ((Y != y || X != x) && Y >= 0 && Y < FIELD_HEIGHT && X >= 0 && X < FIELD_WIDTH) {
					state->adj_count1[X + Y * FIELD_WIDTH]--;
				}
			}
		}
	}
	state->field[x + y * FIELD_WIDTH] = -1;
}

void set_cell(struct GameState *state, int x, int y, int player) {
	if (x < 0 || x >= FIELD_WIDTH || y < 0 || y >= FIELD_HEIGHT) {
		exit(BAD_DATA);
	}
	if (state->field[x + y * FIELD_WIDTH] != player) {
		if (state->field[x + y * FIELD_WIDTH] != -1) {
			clear_cell(state, x, y);
		}
		if (player == 0) {
			state->count0++;
			for (int Y = y - 1; Y <= y + 1; Y++) {
				for (int X = x - 1; X <= x + 1; X++) {
					if ((Y != y || X != x) && Y >= 0 && Y < FIELD_HEIGHT && X >= 0 && X < FIELD_WIDTH) {
						state->adj_count0[X + Y * FIELD_WIDTH]++;
					}
				}
			}
		}else if (player == 1) {
			state->count1++;
			for (int Y = y - 1; Y <= y + 1; Y++) {
				for (int X = x - 1; X <= x + 1; X++) {
					if ((Y != y || X != x) && Y >= 0 && Y < FIELD_HEIGHT && X >= 0 && X < FIELD_WIDTH) {
						state->adj_count1[X + Y * FIELD_WIDTH]++;
					}
				}
			}
		}
		state->field[x + y * FIELD_WIDTH] = player;
	}
}

struct GameState *copy(const struct GameState *state){
	struct GameState *new_state = malloc(sizeof(*new_state));
	new_state->timebank = state->timebank;
	new_state->round = state->round;
	new_state->your_botid = state->your_botid;
	new_state->count0 = state->count0;
	new_state->count1 = state->count1;
	new_state->field = malloc(sizeof(*(new_state->field)) * FIELD_WIDTH * FIELD_HEIGHT);
	new_state->adj_count0 = malloc(sizeof(*(new_state->adj_count0)) * FIELD_WIDTH * FIELD_HEIGHT);
	new_state->adj_count1 = malloc(sizeof(*(new_state->adj_count1)) * FIELD_WIDTH * FIELD_HEIGHT);
	for (int i = 0; i < FIELD_WIDTH*FIELD_HEIGHT; i++) {
		new_state->field[i] = state->field[i];
		new_state->adj_count0[i] = state->adj_count0[i];
		new_state->adj_count1[i] = state->adj_count1[i];
	}
	return new_state;
}

void copy_into(const struct GameState *src, struct GameState *dst) {
	dst->timebank = src->timebank;
	dst->round = src->round;
	dst->your_botid = src->your_botid;
	dst->count0 = src->count0;
	dst->count1 = src->count1;
	for (int i = 0; i < FIELD_WIDTH*FIELD_HEIGHT; i++) {
		dst->field[i] = src->field[i];
		dst->adj_count0[i] = src->adj_count0[i];
		dst->adj_count1[i] = src->adj_count1[i];
	}
}

void clear_state(struct GameState **state)
{
	free((*state)->field);
	free((*state)->adj_count0);
	free((*state)->adj_count1);
	free(*state);
	*state = NULL;
}

void print_field(const struct GameState *state){
	fprintf(stderr, "%d-%d\n", state->count0, state->count1);
	for (int y = 0; y < FIELD_HEIGHT; y++) {
			for (int x = 0; x < FIELD_WIDTH; x++) {
				int adj = state->adj_count0[x + y*FIELD_WIDTH] + state->adj_count1[x + y*FIELD_WIDTH];
				int owner = state->field[x + y * FIELD_WIDTH];
				if (owner == 0) {
					if (adj < 2 || adj > 3) {
						fprintf(stderr, "a");
					}
					else {
						fprintf(stderr, "A");
					}
				}
				else if (owner == 1) {
					if (adj < 2 || adj > 3) {
						fprintf(stderr, "b");
					}
					else {
						fprintf(stderr, "B");
					}
				}
				else {
					if (adj == 3) {
						if (state->adj_count0[x + y * FIELD_WIDTH] > state->adj_count1[x + y * FIELD_WIDTH]) {
							fprintf(stderr, ".");
						}
						else {
							fprintf(stderr, ",");
						}
					}
					else
						fprintf(stderr," ");
				}
			}
			fprintf(stderr,"   ");
			for (int x = 0; x < FIELD_WIDTH; x++) {
				int adj = state->adj_count0[x + y*FIELD_WIDTH] + state->adj_count1[x + y*FIELD_WIDTH];
				if (adj == 1){
					fprintf(stderr,"%c",'-');
				}else if (adj == 2){
					fprintf(stderr,"%c", '+');
				}else if (adj == 3){
					fprintf(stderr,"%c",'|');
				}else if (adj >= 4){
					fprintf(stderr,"%c", '0');
				}else{
					fprintf(stderr," ");
				}
			}
			fprintf(stderr,"\n");
		}
		fprintf(stderr,"------\n");
}

#endif