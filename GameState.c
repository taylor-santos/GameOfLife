#ifndef GAMESTATE
#define GAMESTATE

#include "GameState.h"
#include "definitions.h"
#include "data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct State *instantiate_state()
{
	struct State *state = malloc(sizeof(*state));
	state->count0 = 0;
	state->count1 = 0;
	state->field = calloc(sizeof(*state->field), FIELD_WIDTH*FIELD_HEIGHT);
	state->neighbors = calloc(sizeof(*state->neighbors), FIELD_WIDTH*FIELD_HEIGHT);
	state->changed = calloc(sizeof(*state->changed), FIELD_WIDTH);
	return state;
}

void free_state(struct State **state)
{
	free((*state)->field);
	free((*state)->neighbors);
	free((*state)->changed);
	free(*state);
	*state = NULL;
}

struct State *copy_state(const struct State *state, const bool copy_changes)
{
	struct State *new_state = malloc(sizeof(*new_state));
	new_state->count0 = state->count0;
	new_state->count1 = state->count1;
	new_state->field = malloc(sizeof(*new_state->field) * FIELD_WIDTH * FIELD_HEIGHT);
	memcpy(new_state->field, state->field, sizeof(*new_state->field)*FIELD_WIDTH*FIELD_HEIGHT);
	new_state->neighbors = malloc(sizeof(*new_state->neighbors)*FIELD_WIDTH*FIELD_HEIGHT);
	memcpy(new_state->neighbors, state->neighbors, sizeof(*new_state->neighbors)*FIELD_WIDTH*FIELD_HEIGHT);
	if (copy_changes){
		new_state->changed = malloc(sizeof(*new_state->changed) * FIELD_WIDTH);
		memcpy(new_state->changed, state->changed, sizeof(*new_state->changed) * FIELD_WIDTH);
	}else{
		new_state->changed = calloc(sizeof(*new_state->changed), FIELD_WIDTH);
	}
	return new_state;
}

void copy_into(const struct State *state, const bool copy_changes, struct State *result)
{
	result->count0 = state->count0;
	result->count1 = state->count1;
	memcpy(result->field, state->field, sizeof(*result->field)*FIELD_WIDTH*FIELD_HEIGHT);
	memcpy(result->neighbors, state->neighbors, sizeof(*result->neighbors)*FIELD_WIDTH*FIELD_HEIGHT);
	if (copy_changes) {
		memcpy(result->changed, state->changed, sizeof(*result->changed) * FIELD_WIDTH);
	}
	else {
		for (int i = 0; i < FIELD_WIDTH; i++)
			result->changed[i] = 0;
	}
}

void set_cell(struct State *state, const unsigned short index, const unsigned char value)
{
	unsigned char old = state->field[index];
	state->field[index] = value;
	unsigned char x = index_to_x[index];
	unsigned char y = index_to_y[index];
	if (old && value) {
		if (old != value) {
			if (old == 1) {
				state->count0--;
				state->count1++;
				unsigned short *adj = new_adjacent[index];
				for (int i = 0; i<adj[0]; i++) {
					state->neighbors[adj[i + 1]] += 8;
				}
				unsigned short m = mask[y];
				if (x > 0)
					state->changed[x - 1] |= m;
				state->changed[x] |= m;
				if (x < FIELD_WIDTH - 1)
					state->changed[x + 1] |= m;
			}
			else if (old == 2) {
				state->count0++;
				state->count1--;
				unsigned short *adj = new_adjacent[index];
				for (int i = 0; i<adj[0]; i++) {
					state->neighbors[adj[i + 1]] -= 8;
				}
				unsigned short m = mask[y];
				if (x > 0)
					state->changed[x - 1] |= m;
				state->changed[x] |= m;
				if (x < FIELD_WIDTH - 1)
					state->changed[x + 1] |= m;
			}
		}
	}
	else if (old == 1) {
		state->count0--;
		unsigned short *adj = new_adjacent[index];
		for (int i = 0; i<adj[0]; i++) {
			state->neighbors[adj[i + 1]] -= 1;
		}
		unsigned short m = mask[y];
		if (x > 0)
			state->changed[x - 1] |= m;
		state->changed[x] |= m;
		if (x < FIELD_WIDTH - 1)
			state->changed[x + 1] |= m;
	}
	else if (old == 2) {
		state->count1--;
		unsigned short *adj = new_adjacent[index];
		for (int i = 0; i<adj[0]; i++) {
			state->neighbors[adj[i + 1]] -= 9;
		}
		unsigned short m = mask[y];
		if (x > 0)
			state->changed[x - 1] |= m;
		state->changed[x] |= m;
		if (x < FIELD_WIDTH - 1)
			state->changed[x + 1] |= m;
	}
	else if (value == 1) {
		state->count0++;
		unsigned short *adj = new_adjacent[index];
		for (int i = 0; i<adj[0]; i++) {
			state->neighbors[adj[i + 1]] += 1;
		}
		unsigned short m = mask[y];
		if (x > 0)
			state->changed[x - 1] |= m;
		state->changed[x] |= m;
		if (x < FIELD_WIDTH - 1)
			state->changed[x + 1] |= m;
	}
	else if (value == 2) {
		state->count1++;
		unsigned short *adj = new_adjacent[index];
		for (int i = 0; i<adj[0]; i++) {
			state->neighbors[adj[i + 1]] += 9;
		}
		unsigned short m = mask[y];
		if (x > 0)
			state->changed[x - 1] |= m;
		state->changed[x] |= m;
		if (x < FIELD_WIDTH - 1)
			state->changed[x + 1] |= m;
	}
}

void print(const struct State *state){
	printf("%d-%d\n", state->count0, state->count1);
	unsigned short mask = 1;
	for (int y = 0; y < FIELD_HEIGHT; y++) {
		for (int x = 0; x < FIELD_WIDTH; x++) {
			int owner = state->field[x + y*FIELD_WIDTH];
			if (owner == 1) {
				//fprintf(stderr, "%c",177);
				fprintf(stderr, "0");
			}
			else if (owner == 2) {
				//fprintf(stderr, "%c",178);
				fprintf(stderr, "1");
			}
			else {
				//fprintf(stderr, "%c", 176);
				fprintf(stderr, ".");
			}
		}
		fprintf(stderr, "  ");
		
		for (int i=0; i<FIELD_WIDTH; i++){
			fprintf(stderr, "%c", !!(mask & state->changed[i]) + '0');
		}
		mask <<= 1;
		fprintf(stderr, "  ");
		fprintf(stderr,"\n");
	}
	fprintf(stderr,"------\n");
}

bool verify(const struct State *state) 
{
	int total0 = 0;
	int total1 = 0;
	for (int y = 0; y < FIELD_HEIGHT; y++) {
		for (int x = 0; x < FIELD_WIDTH; x++) {
			if (state->field[x + y * FIELD_WIDTH] == 1)
				total0++;
			if (state->field[x + y * FIELD_WIDTH] == 2)
				total1++;
			int count0 = 0;
			int count1 = 0;
			for (int Y = y - 1; Y <= y + 1; Y++) {
				if (Y >= 0 && Y < FIELD_HEIGHT) {
					for (int X = x - 1; X <= x + 1; X++) {
						if (X >= 0 && X < FIELD_WIDTH) {
							if (X != x || Y != y) {
								if (state->field[X + FIELD_WIDTH * Y] == 1) {
									count0++;
								}
								else if (state->field[X + FIELD_WIDTH * Y] == 2) {
									count1++;
								}
							}
						}
					}
				}
			}
			if (state->neighbors[x + y * FIELD_WIDTH] != count0 + 9 * count1) {
				return true;
			}
		}
	}
	if (total0 != state->count0 || total1 != state->count1) {
		print(state);
		return true;
	}
	return false;
}

bool verify_simulation(const struct State *state1, const struct State *state2)
{
	for (int y = 0; y < FIELD_HEIGHT; y++) {
		for (int x = 0; x < FIELD_WIDTH; x++) {
			int count0 = 0;
			int count1 = 0;
			for (int Y = y - 1; Y <= y + 1; Y++) {
				if (Y >= 0 && Y < FIELD_HEIGHT) {
					for (int X = x - 1; X <= x + 1; X++) {
						if (X >= 0 && X < FIELD_WIDTH) {
							if (X != x || Y != y) {
								if (state1->field[X + FIELD_WIDTH * Y] == 1) {
									count0++;
								}
								else if (state1->field[X + FIELD_WIDTH * Y] == 2) {
									count1++;
								}
							}
						}
					}
				}
			}
			if (state1->field[x + y * FIELD_WIDTH] == 0) {
				if (count0 + count1 == 3) {
					if (count0 > count1) {
						if (state2->field[x + y * FIELD_WIDTH] != 1)
							return false;
					}
					else {
						if (state2->field[x + y * FIELD_WIDTH] != 2)
							return false;
					}
				}
				else {
					if (state2->field[x + y * FIELD_WIDTH] != 0)
						return false;
				}
			}
			else {
				if (count0 + count1 < 2 || count0 + count1 > 3) {
					if (state2->field[x + y * FIELD_WIDTH] != 0)
						return false;
				}
				else {
					if (state2->field[x + y * FIELD_WIDTH] != state1->field[x + y * FIELD_WIDTH])
						return false;
				}
			}
		}
	}
	return false;
}

#endif