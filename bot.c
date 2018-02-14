#ifndef BOT
#define BOT

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "bot.h"
#include "GameState.h"
#include "definitions.h"
#include "data.h"

short minimax(const struct FastState *state, char depth)
{
	if (depth <= 0){
		short score = (short)state->count0 - state->count1;
		if (state->your_botid == 1)
			score = -score;
		return score;
	}
	short best_score = SHRT_MIN;
	for (int y=0; y<FIELD_HEIGHT; y++){
		for (int x=0; x<FIELD_WIDTH; x++){
			if (mod3[state->field[(x+1) + (y+1) * (FIELD_WIDTH+2)]] == 0){

			}
		}
	}
}

struct FastState *simulate_fast(const struct FastState *state)
{
	struct FastState *new_state = copy_fastState(state, false);
	//unsigned short *new_field = calloc(sizeof(*new_field), (FIELD_WIDTH+2) * (FIELD_HEIGHT+2));
	//memcpy(new_field, state->field, sizeof(*new_field) * (FIELD_WIDTH + 2) * (FIELD_HEIGHT + 2));
	//unsigned short *new_changes = calloc(sizeof(*new_changes), FIELD_WIDTH);
	for (unsigned char y=0; y<FIELD_HEIGHT; y++){
		for (unsigned char x=0; x<FIELD_WIDTH; x++){
			if (state->changed[x] && index_mask[y] & state->changed[x]) {
				unsigned short index = (x + 1) + (y + 1)*(FIELD_WIDTH + 2);
				unsigned short curr = state->field[index];
				unsigned char new_owner = *(table + curr);
				unsigned char old_owner = mod3[curr];
				char diff = 0;
				if (!old_owner){
					switch (new_owner) {
						case 1:
							new_state->count0++;
							diff = 3;
							break;
						case 2:
							new_state->count1++;
							diff = 4;
							break;
					}
				}
				else if (!new_owner) {
					switch (old_owner) {
						case 1:
							new_state->count0--;
							diff = 1;
							break;
						case 2:
							new_state->count1--;
							diff = 2;
							break;
					}
				}
				if (diff > 0) {
					const short *mul_ptr = mul + (diff - 1) * 9;
					new_state->field[index] += *(mul_ptr++);
					for (int i = 0; i < 8; i++) {
						new_state->field[index + adjacent[i]] += *mul_ptr;
						mul_ptr++;
					}
					const unsigned short m = mask[y];
					if (x > 0)
						new_state->changed[x - 1] |= m;
					new_state->changed[x] |= m;
					if (x < FIELD_WIDTH - 1)
						new_state->changed[x + 1] |= m;
				}
			}
		}
	}
	return new_state;
}

struct FastState *simulate_with_prediction(const struct FastState *state, const struct FastState *prediction)
{
	struct FastState *new_state = copy_fastState(state, false);
	for (unsigned char y=0; y<FIELD_HEIGHT; y++){
		for (unsigned char x=0; x<FIELD_WIDTH; x++){
			unsigned short index = (x + 1) + (y + 1)*(FIELD_WIDTH + 2);
			if (state->changed[x] && index_mask[y] & state->changed[x]) {
				unsigned short curr = state->field[index];
				unsigned char new_owner = *(table + curr);
				unsigned char old_owner = mod3[curr];
				char diff = 0;
				if (!old_owner){
					switch (new_owner) {
						case 1:
							new_state->count0++;
							diff = 3;
							break;
						case 2:
							new_state->count1++;
							diff = 4;
							break;
					}
				}
				else if (!new_owner) {
					switch (old_owner) {
						case 1:
							new_state->count0--;
							diff = 1;
							break;
						case 2:
							new_state->count1--;
							diff = 2;
							break;
					}
				}
				if (diff > 0) {
					const short *mul_ptr = mul + (diff - 1) * 9;
					new_state->field[index] += *(mul_ptr++);
					for (int i = 0; i < 8; i++) {
						new_state->field[index + adjacent[i]] += *mul_ptr;
						mul_ptr++;
					}
					const unsigned short m = mask[y];
					if (x > 0)
						new_state->changed[x - 1] |= m;
					new_state->changed[x] |= m;
					if (x < FIELD_WIDTH - 1)
						new_state->changed[x + 1] |= m;
				}
			}else{
				new_state->field[index] = prediction->field[index];
			}
		}
	}
	return new_state;
}

#endif