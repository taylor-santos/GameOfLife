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

unsigned short *best_move(const struct FastState *state, const unsigned char depth) {
	if (depth == 0) {
		short *move = malloc(sizeof(*move));
		move[0] = 0;
		return move;
	}
	struct FastState **predictions = malloc(sizeof(*predictions) * depth);
	predictions[0] = simulate_fast(state);
	predictions[0]->your_botid = !predictions[0]->your_botid;
	for (int i = 1; i < depth; i++) {
		predictions[i] = simulate_fast(predictions[i-1]);
	}
	short best_score = -minimax(predictions[0], predictions + 1, depth - 1, SHRT_MIN+1, SHRT_MAX);
	short best_kill = -1;
	for (int y = 0; y<FIELD_HEIGHT; y++) {
		for (int x = 0; x<FIELD_WIDTH; x++) {
			unsigned short index = (x + 1) + (y + 1) * (FIELD_WIDTH + 2);
			if (mod3[state->field[index]] != 0) {
				struct FastState *new_state = copy_fastState(state, true);
				new_state->your_botid = !state->your_botid;
				set_cell(new_state, index, 0);
				struct FastState *next_state = simulate_with_prediction(new_state, predictions[0]);
				free_fastState(&new_state);
				int score = -minimax(next_state, predictions + 1, depth - 1, SHRT_MIN+1, SHRT_MAX);
				free_fastState(&next_state);
				if (score > best_score) {
					best_score = score;
				}
			}
		}
	}
	for (int i = 0; i < depth; i++) {
		free_fastState(&(predictions[i]));
	}
	free(predictions);
	if (best_kill == -1) {
		short *move = malloc(sizeof(*move));
		move[0] = 0;
		return move;
	}
	else {
		short *move = malloc(sizeof(*move) * 2);
		move[0] = 1;
		move[1] = best_kill;
		return move;
	}
}

short minimax(const struct FastState *state, const struct FastState **predictions, const unsigned char depth, short alpha, short beta)
{
	if (depth == 0){
		short score = (short)(state->count0) - state->count1;
		if (state->your_botid == 1)
			score = -score;
		return score;
	}
	struct FastState *next_state = simulate_with_prediction(state, predictions[0]);
	next_state->your_botid = !state->your_botid;
	short best_score = -minimax(next_state, predictions + 1, depth - 1, -beta, -alpha);
	free_fastState(&next_state);
	for (int y=0; y<FIELD_HEIGHT; y++){
		for (int x=0; x<FIELD_WIDTH; x++){
			unsigned short index = (x + 1) + (y + 1) * (FIELD_WIDTH + 2);
			if (mod3[state->field[index]] != 0){
				struct FastState *new_state = copy_fastState(state, true);
				new_state->your_botid = !state->your_botid;
				set_cell(new_state, index, 0);
				next_state = simulate_with_prediction(new_state, predictions[0]);
				free_fastState(&new_state);
				int score = -minimax(next_state, predictions+1, depth - 1, -beta, -alpha);
				free_fastState(&next_state);
				if (score > best_score) {
					best_score = score;
				}
				if (score > alpha) {
					alpha = score;
				}
				if (beta <= alpha) {
					return beta;
				}
			}
		}
	}
	return best_score;
}

struct FastState *simulate_fast(const struct FastState *state)
{
	struct FastState *new_state = copy_fastState(state, false);
	for (unsigned char y=0; y<FIELD_HEIGHT; y++){
		for (unsigned char x=0; x<FIELD_WIDTH; x++){
			if (index_mask[y] & state->changed[x]) {
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
	struct FastState *new_state = copy_fastState(prediction, false);
	for (unsigned char y=0; y<FIELD_HEIGHT; y++){
		for (unsigned char x=0; x<FIELD_WIDTH; x++){
			if (index_mask[y] & state->changed[x]) {
				unsigned short index = (x + 1) + (y + 1)*(FIELD_WIDTH + 2);
				unsigned short curr = state->field[index];
				unsigned char new_owner = *(table + curr);
				unsigned char old_owner = mod3[prediction->field[index]];
				const short *mul_ptr;
				unsigned short m;
				if (!old_owner){
					switch (new_owner) {
						case 1:
							new_state->count0++;
							mul_ptr = mul + 18;
							new_state->field[index] += *(mul_ptr++);
							for (int i = 0; i < 8; i++) {
								new_state->field[index + adjacent[i]] += *mul_ptr;
								mul_ptr++;
							}
							m = mask[y];
							if (x > 0)
								new_state->changed[x - 1] |= m;
							new_state->changed[x] |= m;
							if (x < FIELD_WIDTH - 1)
								new_state->changed[x + 1] |= m;
							break;
						case 2:
							new_state->count1++;
							mul_ptr = mul + 27;
							new_state->field[index] += *(mul_ptr++);
							for (int i = 0; i < 8; i++) {
								new_state->field[index + adjacent[i]] += *mul_ptr;
								mul_ptr++;
							}
							m = mask[y];
							if (x > 0)
								new_state->changed[x - 1] |= m;
							new_state->changed[x] |= m;
							if (x < FIELD_WIDTH - 1)
								new_state->changed[x + 1] |= m;
							break;
					}
				}
				else if (!new_owner) {
					switch (old_owner) {
						case 1:
							new_state->count0--;
							mul_ptr = mul;
							new_state->field[index] += *(mul_ptr++);
							for (int i = 0; i < 8; i++) {
								new_state->field[index + adjacent[i]] += *mul_ptr;
								mul_ptr++;
							}
							m = mask[y];
							if (x > 0)
								new_state->changed[x - 1] |= m;
							new_state->changed[x] |= m;
							if (x < FIELD_WIDTH - 1)
								new_state->changed[x + 1] |= m;
							break;
						case 2:
							new_state->count1--;
							mul_ptr = mul + 9;
							new_state->field[index] += *(mul_ptr++);
							for (int i = 0; i < 8; i++) {
								new_state->field[index + adjacent[i]] += *mul_ptr;
								mul_ptr++;
							}
							m = mask[y];
							if (x > 0)
								new_state->changed[x - 1] |= m;
							new_state->changed[x] |= m;
							if (x < FIELD_WIDTH - 1)
								new_state->changed[x + 1] |= m;
							break;
					}
				}
			}
		}
	}
	return new_state;
}

#endif