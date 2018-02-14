#ifndef GAMESTATE
#define GAMESTATE

#include "GameState.h"
#include "definitions.h"
#include "data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct FastState *instantiate_fastState()
{
	struct FastState *state = malloc(sizeof(*state));
	state->timebank = 10000;
	state->round = 0;
	state->count0 = 0;
	state->count1 = 0;
	state->your_botid = 0;
	state->field = calloc(sizeof(*(state->field)), (FIELD_WIDTH + 2) * (FIELD_HEIGHT + 2));
	state->changed = calloc(sizeof(*(state->changed)), FIELD_WIDTH);
	return state;
}

void print_fast(const struct FastState *state)
{
	fprintf(stderr, "%d-%d\n", state->count0, state->count1);
	unsigned short mask = 1;
	for (int y = 0; y < FIELD_HEIGHT; y++) {
		for (int x = 0; x < FIELD_WIDTH; x++) {
			int owner = state->field[(x+1) + (y+1) * (FIELD_WIDTH+2)] % 3;
			if (owner == 1) {
				fprintf(stderr, "0");
			}
			else if (owner == 2) {
				fprintf(stderr, "1");
			}
			else {
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

struct FastState *copy_fastState(const struct FastState *state, _Bool copy_changes){
	struct FastState *new_state = malloc(sizeof(*new_state));
	new_state->timebank = state->timebank;
	new_state->round = state->round;
	new_state->count0 = state->count0;
	new_state->count1 = state->count1;
	new_state->your_botid = state->your_botid;
	new_state->field = malloc(sizeof(*(new_state->field)) * (FIELD_WIDTH+2) * (FIELD_HEIGHT+2));
	memcpy(new_state->field, state->field, sizeof(*(state->field)) * (FIELD_WIDTH+2) * (FIELD_HEIGHT+2));
	if (copy_changes){
		new_state->changed = malloc(sizeof(*(new_state->changed)) * FIELD_WIDTH);
		memcpy(new_state->changed, state->changed, sizeof(*(new_state->changed)) * FIELD_WIDTH);
	}else{
		new_state->changed = calloc(sizeof(*(new_state->changed)), FIELD_WIDTH);
	}
	return new_state;
}

void set_cell(struct FastState *state, const unsigned short index, const unsigned char value)
{
	unsigned char prev = mod3[state->field[index]];
	if (prev == value)
		return;
	char diff = 0;
	if (!prev){
		switch (value) {
			case 1:
				state->count0++;
				diff = 3;
				break;
			case 2:
				state->count1++;
				diff = 4;
				break;
		}
	}
	else if (!value) {
		switch (prev) {
			case 1:
				state->count0--;
				diff = 1;
				break;
			case 2:
				state->count1--;
				diff = 2;
				break;
		}
	}
	if (diff > 0) {
		const short *mul_ptr = mul + (diff - 1) * 9;
		state->field[index] += *(mul_ptr++);
		for (int i = 0; i < 8; i++) {
			state->field[index + adjacent[i]] += *mul_ptr;
			mul_ptr++;
		}
		unsigned char x = index % (FIELD_WIDTH + 2) - 1;
		unsigned char y = index / (FIELD_WIDTH + 2) - 1;
		const unsigned short m = mask[y];
		if (x > 0)
			state->changed[x - 1] |= m;
		state->changed[x] |= m;
		if (x < FIELD_WIDTH - 1)
			state->changed[x + 1] |= m;
	}
}

#endif