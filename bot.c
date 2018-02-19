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

short *minimax(const struct FastState *state, const struct FastState **predictions, const unsigned char depth, short alpha, short beta)
{
	if (depth == 0){
		short score = (short)(state->count0) - state->count1;
		if (state->your_botid == 1)
			score = -score;
		short *move = malloc(sizeof(*move) * 2);
		move[0] = score;
		move[1] = 0;;
		return move;
	}
	struct FastState *next_state = simulate_with_prediction(state, predictions[0]);
	next_state->your_botid = !state->your_botid;
	short *pass_result = minimax(next_state, predictions + 1, depth - 1, -beta, -alpha);
	short best_score = -pass_result[0];
	free(pass_result);
	short best_kill = -1;
	free_fastState(&next_state);
	for (int y=0; y<FIELD_HEIGHT; y++){
		for (int x=0; x<FIELD_WIDTH; x++){
			unsigned short index = (x + 1) + (y + 1) * (FIELD_WIDTH + 2);
			if (mod3[state->field[index]] == state->your_botid + 1){
				struct FastState *new_state = copy_fastState(state, true);
				set_cell(new_state, index, 0);
				next_state = simulate_with_prediction(new_state, predictions[0]);
				next_state->your_botid = !state->your_botid;
				free_fastState(&new_state);
				short *kill_result = minimax(next_state, predictions + 1, depth - 1, -beta, -alpha);
				short score = -kill_result[0];
				free(kill_result);
				free_fastState(&next_state);
				if (score > best_score) {
					best_score = score;
					best_kill = index;
				}
				if (score > alpha) {
					alpha = score;
				}
				if (beta <= alpha) {
					short *move = malloc(sizeof(*move));
					move[0] = beta;
					return move;
				}
			}
		}
	}
	for (int y=0; y<FIELD_HEIGHT; y++){
		for (int x=0; x<FIELD_WIDTH; x++){
			unsigned short index = (x + 1) + (y + 1) * (FIELD_WIDTH + 2);
			if (mod3[state->field[index]] == !state->your_botid + 1){
				struct FastState *new_state = copy_fastState(state, true);
				set_cell(new_state, index, 0);
				next_state = simulate_with_prediction(new_state, predictions[0]);
				next_state->your_botid = !state->your_botid;
				free_fastState(&new_state);
				short *kill_result = minimax(next_state, predictions + 1, depth - 1, -beta, -alpha);
				short score = -kill_result[0];
				free(kill_result);
				free_fastState(&next_state);
				if (score > best_score) {
					best_score = score;
					best_kill = index;
				}
				if (score > alpha) {
					alpha = score;
				}
				if (beta <= alpha) {
					short *move = malloc(sizeof(*move));
					move[0] = beta;
					return move;
				}
			}
		}
	}
	if (best_kill == -1) {
		short *move = malloc(sizeof(*move) * 2);
		move[0] = best_score;
		move[1] = 0;
		return move;
	}
	else {
		short *move = malloc(sizeof(*move) * 3);
		move[0] = best_score;
		move[1] = 1;
		move[2] = best_kill;
		return move;
	}
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