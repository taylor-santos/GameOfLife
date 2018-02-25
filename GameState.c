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

void set_cell(struct State *state, const unsigned short index, const unsigned char value)
{
	char diff = (char)state->field[index] - value;
	state->field[index] = value;
	unsigned char x = index % FIELD_WIDTH;
	unsigned char y = index / FIELD_WIDTH;
	unsigned short *adj;
	unsigned short m;
	switch(diff){
		case -2:
			state->count1--;
			adj = new_adjacent[index];
			for (int i=0; i<adj[0]; i++){
				state->neighbors[adj[i+1]] -= 9;
			}
			m = mask[y];
			if (x > 0)
				state->changed[x - 1] |= m;
			state->changed[x] |= m;
			if (x < FIELD_WIDTH - 1)
				state->changed[x + 1] |= m;
			break;
		case -1:
			state->count0--;
			adj = new_adjacent[index];
			for (int i=0; i<adj[0]; i++){
				state->neighbors[adj[i+1]] -= 1;
			}
			m = mask[y];
			if (x > 0)
				state->changed[x - 1] |= m;
			state->changed[x] |= m;
			if (x < FIELD_WIDTH - 1)
				state->changed[x + 1] |= m;
			break;
		case 1:
			state->count0++;
			adj = new_adjacent[index];
			for (int i=0; i<adj[0]; i++){
				state->neighbors[adj[i+1]] += 1;
			}
			m = mask[y];
			if (x > 0)
				state->changed[x - 1] |= m;
			state->changed[x] |= m;
			if (x < FIELD_WIDTH - 1)
				state->changed[x + 1] |= m;
			break;
		case 2:
			state->count1++;
			adj = new_adjacent[index];
			for (int i=0; i<adj[0]; i++){
				state->neighbors[adj[i+1]] += 9;
			}
			m = mask[y];
			if (x > 0)
				state->changed[x - 1] |= m;
			state->changed[x] |= m;
			if (x < FIELD_WIDTH - 1)
				state->changed[x + 1] |= m;
			break;
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

struct FastState *instantiate_fastState()
{
	struct FastState *state = malloc(sizeof(*state));
	state->count0 = 0;
	state->count1 = 0;
	state->field = calloc(sizeof(*(state->field)), (FIELD_WIDTH + 2) * (FIELD_HEIGHT + 2));
	//state->changed = calloc(sizeof(*(state->changed)), FIELD_WIDTH);
	return state;
}

void free_fastState(struct FastState **state)
{
	free((*state)->field);
	//free((*state)->changed);
	free(*state);
	*state = NULL;
}

void print_fast(const struct FastState *state)
{
	fprintf(stderr, "%d-%d\n", state->count0, state->count1);
	unsigned short mask = 1;
	for (int y = 0; y < FIELD_HEIGHT; y++) {
		for (int x = 0; x < FIELD_WIDTH; x++) {
			int owner = state->field[(x+1) + (y+1) * (FIELD_WIDTH+2)] % 3;
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
		/*
		fprintf(stderr, "  ");
		
		for (int i=0; i<FIELD_WIDTH; i++){
			fprintf(stderr, "%c", !!(mask & state->changed[i]) + '0');
		}
		mask <<= 1;
		fprintf(stderr, "  ");
		*/
		fprintf(stderr,"\n");
	}
	fprintf(stderr,"------\n");
}

struct FastState *copy_fastState(const struct FastState *state, const bool copy_changes){
	struct FastState *new_state = malloc(sizeof(*new_state));
	new_state->count0 = state->count0;
	new_state->count1 = state->count1;
	new_state->field = malloc(sizeof(*(new_state->field)) * (FIELD_WIDTH+2) * (FIELD_HEIGHT+2));
	memcpy(new_state->field, state->field, sizeof(*(state->field)) * (FIELD_WIDTH+2) * (FIELD_HEIGHT+2));
	/*
	if (copy_changes){
		new_state->changed = malloc(sizeof(*(new_state->changed)) * FIELD_WIDTH);
		memcpy(new_state->changed, state->changed, sizeof(*(new_state->changed)) * FIELD_WIDTH);
	}else{
		new_state->changed = calloc(sizeof(*(new_state->changed)), FIELD_WIDTH);
	}
	*/
	return new_state;
}

void set_cell_fast(struct FastState *state, const unsigned short index, const unsigned char value)
{
	unsigned char old_owner = mod3[state->field[index]];
	if (old_owner == value)
		return;
	const short *mul_ptr;
	//unsigned short m;
	char x = index_to_x[index];
	char y = index_to_y[index];
	if (!old_owner){
		switch (value) {
			case 1:
				state->count0++;
				mul_ptr = mul + 18;
				state->field[index] += *(mul_ptr++);
				for (int i = 0; i < 8; i++) {
					state->field[index + adjacent[i]] += *mul_ptr;
					mul_ptr++;
				}
				/*
				m = mask[y];
				if (x > 0)
					state->changed[x - 1] |= m;
				state->changed[x] |= m;
				if (x < FIELD_WIDTH - 1)
					state->changed[x + 1] |= m;
				*/
				break;
			case 2:
				state->count1++;
				mul_ptr = mul + 27;
				state->field[index] += *(mul_ptr++);
				for (int i = 0; i < 8; i++) {
					state->field[index + adjacent[i]] += *mul_ptr;
					mul_ptr++;
				}
				/*
				m = mask[y];
				if (x > 0)
					state->changed[x - 1] |= m;
				state->changed[x] |= m;
				if (x < FIELD_WIDTH - 1)
					state->changed[x + 1] |= m;
				*/
				break;
		}
	}
	else if (!value) {
		switch (old_owner) {
			case 1:
				state->count0--;
				mul_ptr = mul;
				state->field[index] += *(mul_ptr++);
				for (int i = 0; i < 8; i++) {
					state->field[index + adjacent[i]] += *mul_ptr;
					mul_ptr++;
				}
				/*
				m = mask[y];
				if (x > 0)
					state->changed[x - 1] |= m;
				state->changed[x] |= m;
				if (x < FIELD_WIDTH - 1)
					state->changed[x + 1] |= m;
				*/
				break;
			case 2:
				state->count1--;
				mul_ptr = mul + 9;
				state->field[index] += *(mul_ptr++);
				for (int i = 0; i < 8; i++) {
					state->field[index + adjacent[i]] += *mul_ptr;
					mul_ptr++;
				}
				/*
				m = mask[y];
				if (x > 0)
					state->changed[x - 1] |= m;
				state->changed[x] |= m;
				if (x < FIELD_WIDTH - 1)
					state->changed[x + 1] |= m;
				*/
				break;
		}
	}
}

bool verify_fastState(const struct FastState *state) {
	for (int y = 0; y < FIELD_HEIGHT; y++) {
		for (int x = 0; x < FIELD_WIDTH; x++) {
			int index = (x + 1) + (y + 1)*(FIELD_WIDTH + 2);
			int val = state->field[index];
			int owner = mod3[val];
			int mul = 3;
			int new_val = owner;
			for (int i = 7; i >= 0; i--) {
				int adj_index = index + adjacent[i];
				if (adj_index/(FIELD_WIDTH+2)-1 < 0){
					mul*=3;
					continue;
				}
				if (adj_index % (FIELD_WIDTH+2)-1 < 0 || adj_index % (FIELD_WIDTH+2)-1 == FIELD_WIDTH){
					mul*=3;
					continue;
				}
				int adj_value = state->field[adj_index];
				int adj_owner = mod3[adj_value];
				new_val += adj_owner * mul;
				mul *= 3;
			}
			if (new_val != val){
				fprintf(stderr, "%x %d %d %d\n", x, y, val, new_val);
				return true;
			}
		}
	}
	return false;
}

#endif