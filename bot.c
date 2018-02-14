#ifndef BOT
#define BOT

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "bot.h"
#include "GameState.h"
#include "definitions.h"
#include "data.h"


void simulate_fast(struct FastState *state){
	unsigned short *new_field = calloc(sizeof(*new_field), (FIELD_WIDTH+2) * (FIELD_HEIGHT+2));
	memcpy(new_field, state->field, sizeof(*new_field) * (FIELD_WIDTH + 2) * (FIELD_HEIGHT + 2));
	unsigned short *new_changes = calloc(sizeof(*new_changes), FIELD_WIDTH);
	for (unsigned char y=0; y<FIELD_HEIGHT; y++){
		for (unsigned char x=0; x<FIELD_WIDTH; x++){
			if (state->changed[x]) {
				if (index_mask[y] & state->changed[x]) {
					unsigned short index = (x + 1) + (y + 1)*(FIELD_WIDTH + 2);
					unsigned short curr = state->field[index];
					unsigned char new_owner = *(table + curr);
					unsigned char old_owner = mod3[curr];
					char diff = 0;
					if (!old_owner){
						switch (new_owner) {
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
					else if (!new_owner) {
						switch (old_owner) {
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
						unsigned short *mul_ptr = mul + (diff - 1) * 9;
						new_field[index] += *(mul_ptr++);
						for (int i = 0; i < 8; i++) {
							new_field[index + adjacent[i]] += *mul_ptr;
							mul_ptr++;
						}
						unsigned short m = mask[y];
						if (x > 0)
							new_changes[x - 1] |= m;
						new_changes[x] |= m;
						if (x < FIELD_WIDTH - 1)
							new_changes[x + 1] |= m;
					}
				}
			}
		}
	}
	memcpy(state->field, new_field, sizeof(*state->field) * (FIELD_WIDTH + 2) * (FIELD_HEIGHT + 2));
	memcpy(state->changed, new_changes, sizeof(*state->changed) * FIELD_WIDTH);

	free(new_field);
	free(new_changes);
}

#endif