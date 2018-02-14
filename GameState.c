#ifndef GAMESTATE
#define GAMESTATE

#include "GameState.h"
#include "definitions.h"
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

void print_fast(struct FastState *state)
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

#endif