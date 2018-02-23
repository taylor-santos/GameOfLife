#ifndef BOT
#define BOT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include "bot.h"
#include "GameState.h"
#include "definitions.h"
#include "data.h"

int *minimax(const struct State *state, const unsigned char id, const unsigned char depth, int alpha, int beta)
{
	//Returns a short array of length depth+1. minimax[0] = score, minimax[1:depth+1] = sequence of moves.
	if (state->count0 == 0){
		int *move = malloc(sizeof(*move) * (depth+1));
		move[0] = SHRT_MIN + 2;
		if (state->count1 != 0 && id == 1){
			move[0] = SHRT_MAX-1;
		}
		for (int i=0; i<depth; i++)
			move[i+1] = -2;
		return move;
	}
	if (state->count1 == 0){
		int *move = malloc(sizeof(*move) * (depth+1));
		move[0] = SHRT_MIN + 2;
		if (state->count0 != 0 && id == 0){
			move[0] = SHRT_MAX - 1;
		}
		for (int i=0; i<depth; i++)
			move[i+1] = -2;
		return move;
	}
	if (depth == 0){
		int score = (int)(state->count0) - state->count1;
		if (id == 1)
			score = -score;
		int *move = malloc(sizeof(*move));
		move[0] = score;
		return move;
	}
	int *best_sequence = malloc(sizeof(*best_sequence) * depth);
	for (int i = 0; i < depth; i++) {
		best_sequence[i] = -1;
	}
	unsigned short owned_cells[288];
	unsigned short enemy_cells[288];
	unsigned short open_cells[288];
	unsigned short owned_index = 0;
	unsigned short enemy_index = 0;
	unsigned short open_index = 0;
	int best_score = SHRT_MIN + 1;

	for (int y=0; y<FIELD_HEIGHT; y++){
		for (int x=0; x<FIELD_WIDTH; x++){
			unsigned short index = x + y*FIELD_WIDTH;
			if (state->neighbors[index] != 0) {
				unsigned char owner = state->field[index];
				if (owner == 0)
					open_cells[open_index++] = index;
				else if (owner == id + 1)
					owned_cells[owned_index++] = index;
				else
					enemy_cells[enemy_index++] = index;
			}
		}
	}
	/*
	for (int i = 0; i < owned_index; i++) {
		struct FastState *state1 = copy_fastState(state, true);
		set_cell(state1, owned_cells[i], 0);
		for (int j = i + 1; j < owned_index; j++) {
			struct FastState *state2 = copy_fastState(state1, true);
			set_cell(state1, owned_cells[j], 0);
			for (int k = 0; k < open_index; k++) {
				struct FastState *new_state = copy_fastState(state2, true);
				set_cell(new_state, open_cells[k], id + 1);
				//struct FastState **next_predictions = malloc(sizeof(*predictions) * depth);
				//next_predictions[0] = simulate_with_prediction(new_state, predictions[0]);
				struct FastState *next_state = simulate_fast(new_state);
				free_fastState(&new_state);
				int *result = minimax(next_state, !id, depth - 1, -beta, -alpha);
				free_fastState(&next_state);
				int score = -result[0];
				if (score > best_score) {
					best_score = score;
					best_sequence[0] = open_cells[k] + 360 + 360 * owned_cells[i] + 129600 * owned_cells[j];
					for (int n = 1; n<depth; n++)
						best_sequence[n] = (int)result[n];
				}
				if (score > alpha) {
					alpha = score;
				}
				if (beta <= alpha) {
					free(best_sequence);
					int *move = malloc(sizeof(*move) *(depth + 1));
					move[0] = beta;
					for (int n = 1; n<depth + 1; n++)
						move[n] = -3;
					free(result);
					free_fastState(&state2);
					free_fastState(&state1);
					return move;
				}
				free(result);
			}
			free_fastState(&state2);
		}
		free_fastState(&state1);
	}
	*/
	for(int i=0; i<enemy_index; i++){
		unsigned short index = enemy_cells[i];
		struct State *new_state = copy_state(state, true);
		set_cell(new_state, index, 0);
		//struct FastState **next_predictions = malloc(sizeof(*predictions) * depth);
		//next_predictions[0] = simulate_with_prediction(new_state, predictions[0]);
		struct State *next_state = simulate(new_state);
		free_state(&new_state);
		/*
		for (int n = 1; n < depth; n++) {
			next_predictions[n] = simulate_with_prediction(next_predictions[n - 1], predictions[n]);
		}
		*/
		int *result = minimax(next_state, !id, depth - 1, -beta, -alpha);
		/*
		for (int n = 0; n<depth; n++) {
			free_fastState(&(next_predictions[n]));
		}
		free(next_predictions);
		*/
		free_state(&next_state);
		int score = -result[0];
		if (score > best_score) {
			best_score = score;
			best_sequence[0] = index;
			for (int n = 1; n<depth; n++)
				best_sequence[n] = (int)result[n];
		}
		if (score > alpha) {
			alpha = score;
		}
		if (beta <= alpha) {
			free(best_sequence);
			int *move = malloc(sizeof(*move) *(depth + 1));
			move[0] = beta;
			for (int n = 1; n<depth + 1; n++)
				move[n] = -3;
			free(result);
			return move;
		}
		free(result);
	}
	for(int i=0; i<owned_index; i++){
		unsigned short index = owned_cells[i];
		struct State *new_state = copy_state(state, true);
		set_cell(new_state, index, 0);
		//struct FastState **next_predictions = malloc(sizeof(*predictions) * depth);
		//next_predictions[0] = simulate_with_prediction(new_state, predictions[0]);
		struct State *next_state = simulate(new_state);
		free_state(&new_state);
		/*
		for (int n = 1; n < depth; n++) {
			next_predictions[n] = simulate_with_prediction(next_predictions[n - 1], predictions[n]);
		}
		*/
		int *result = minimax(next_state, !id, depth - 1, -beta, -alpha);
		/*
		for (int n = 0; n<depth; n++) {
			free_fastState(&(next_predictions[n]));
		}
		free(next_predictions);
		*/
		free_state(&next_state);
		int score = -result[0];
		if (score > best_score) {
			best_score = score;
			best_sequence[0] = index;
			for (int n = 1; n<depth; n++)
				best_sequence[n] = (int)result[n];
		}
		if (score > alpha) {
			alpha = score;
		}
		if (beta <= alpha) {
			free(best_sequence);
			int *move = malloc(sizeof(*move) *(depth + 1));
			move[0] = beta;
			for (int n = 1; n<depth + 1; n++)
				move[n] = -3;
			free(result);
			return move;
		}
		free(result);
	}
	//struct FastState *next_state = simulate_with_prediction(state, predictions[0]);
	struct State *next_state = simulate(state);
	int *pass_result = minimax(next_state, !id, depth - 1, -beta, -alpha);
	free_state(&next_state);
	int pass_score = -pass_result[0];
	if (pass_score > best_score) {
		best_score = pass_score;
		best_sequence[0] = -1;
		for (int i = 1; i<depth; i++)
			best_sequence[i] = pass_result[i];
	}
	free(pass_result);
	int *move = malloc(sizeof(*move) * (depth + 1));
	move[0] = best_score;
	for (int i=0; i<depth; i++)
		move[i+1] = best_sequence[i];
	free(best_sequence);
	return move;
}

struct State *simulate(const struct State *state){
	struct State *new_state = copy_state(state, false);
	for (unsigned char x=0; x<FIELD_WIDTH; x++){
		if (state->changed[x]){
			for (unsigned char y=0; y<FIELD_HEIGHT; y++){
				unsigned short index = x + y*FIELD_WIDTH;
				if (state->changed[x] & index_mask[y]){
					unsigned char n = state->neighbors[index] * (state->field[index] + 1);
					char change = diff[n];
					new_state->field[index] += change;
					unsigned short *adj;
					unsigned short m;
					switch(change){
						case (-2):
						new_state->count1--;
						adj = new_adjacent[index];
						for (int i=0; i<adj[0]; i++){
							new_state->neighbors[adj[i+1]] -= 9;
						}
						m = mask[y];
						if (x > 0)
							new_state->changed[x - 1] |= m;
						state->changed[x] |= m;
						if (x < FIELD_WIDTH - 1)
							new_state->changed[x + 1] |= m;
						break;
						case (-1):
						new_state->count0--;
						adj = new_adjacent[index];
						for (int i=0; i<adj[0]; i++){
							new_state->neighbors[adj[i+1]] -= 1;
						}
						m = mask[y];
						if (x > 0)
							new_state->changed[x - 1] |= m;
						state->changed[x] |= m;
						if (x < FIELD_WIDTH - 1)
							new_state->changed[x + 1] |= m;
						break;
						case (1):
						new_state->count0++;
						adj = new_adjacent[index];
						for (int i=0; i<adj[0]; i++){
							new_state->neighbors[adj[i+1]] += 1;
						}
						m = mask[y];
						if (x > 0)
							new_state->changed[x - 1] |= m;
						state->changed[x] |= m;
						if (x < FIELD_WIDTH - 1)
							new_state->changed[x + 1] |= m;
						break;
						case (2):
						new_state->count1++;
						adj = new_adjacent[index];
						for (int i=0; i<adj[0]; i++){
							new_state->neighbors[adj[i+1]] += 9;
						}
						m = mask[y];
						if (x > 0)
							new_state->changed[x - 1] |= m;
						state->changed[x] |= m;
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

struct FastState *simulate_fast(const struct FastState *state)
{
	struct FastState *new_state = copy_fastState(state, false);
	for (unsigned char x=0; x<FIELD_WIDTH; x++){
		//if (state->changed[x]){
			for (unsigned char y=0; y<FIELD_HEIGHT; y++){
				//if (index_mask[y] & state->changed[x]) {
					unsigned short index = (x + 1) + (y + 1)*(FIELD_WIDTH + 2);
					unsigned short curr = state->field[index];
					unsigned char new_owner = *(table + curr);
					unsigned char old_owner = mod3[curr];
					const short *mul_ptr;
					unsigned short m;
					if (!old_owner){
						switch (new_owner) {
							case 1:
							new_state->count0++;
							mul_ptr = mul + 18;
							new_state->field[index] += *mul_ptr;
							mul_ptr++;
							for (int i = 0; i < 8; i++) {
								new_state->field[index + adjacent[i]] += *mul_ptr;
								mul_ptr++;
							}
							/*
							m = mask[y];
							if (x > 0)
								new_state->changed[x - 1] |= m;
							new_state->changed[x] |= m;
							if (x < FIELD_WIDTH - 1)
								new_state->changed[x + 1] |= m;
							*/
							break;
							case 2:
							new_state->count1++;
							mul_ptr = mul + 27;
							new_state->field[index] += *mul_ptr;
							mul_ptr++;
							for (int i = 0; i < 8; i++) {
								new_state->field[index + adjacent[i]] += *mul_ptr;
								mul_ptr++;
							}
							/*
							m = mask[y];
							if (x > 0)
								new_state->changed[x - 1] |= m;
							new_state->changed[x] |= m;
							if (x < FIELD_WIDTH - 1)
								new_state->changed[x + 1] |= m;
							*/
							break;
						}
					}
					else if (!new_owner) {
						switch (old_owner) {
							case 1:
							new_state->count0--;
							mul_ptr = mul;
							new_state->field[index] += *mul_ptr;
							mul_ptr++;
							for (int i = 0; i < 8; i++) {
								new_state->field[index + adjacent[i]] += *mul_ptr;
								mul_ptr++;
							}
							/*
							m = mask[y];
							if (x > 0)
								new_state->changed[x - 1] |= m;
							new_state->changed[x] |= m;
							if (x < FIELD_WIDTH - 1)
								new_state->changed[x + 1] |= m;
							*/
							break;
							case 2:
							new_state->count1--;
							mul_ptr = mul + 9;
							new_state->field[index] += *mul_ptr;
							mul_ptr++;
							for (int i = 0; i < 8; i++) {
								new_state->field[index + adjacent[i]] += *mul_ptr;
								mul_ptr++;
							}
							/*
							m = mask[y];
							if (x > 0)
								new_state->changed[x - 1] |= m;
							new_state->changed[x] |= m;
							if (x < FIELD_WIDTH - 1)
								new_state->changed[x + 1] |= m;
							*/
							break;
						}
					}
				//}
			}
		//}
	}
	return new_state;
}
/*
struct FastState *simulate_with_prediction(const struct FastState *state, const struct FastState *prediction)
{
	struct FastState *new_state = copy_fastState(prediction, false);
	for (unsigned char x=0; x<FIELD_WIDTH; x++){
		if (state->changed[x]){
			for (unsigned char y=0; y<FIELD_HEIGHT; y++){
				if (index_mask[y] & state->changed[x]) {
					unsigned short index = (x + 1) + (y + 1)*(FIELD_WIDTH + 2);
					unsigned short old_owner = state->field[index];
					unsigned char new_owner = *(table + old_owner);
					unsigned char predicted_new_owner = mod3[prediction->field[index]];
					const short *mul_ptr;
					unsigned short m;
					if (!predicted_new_owner){
						switch (new_owner) {
							case 1:
							new_state->count0++;
							mul_ptr = mul + 18;
							new_state->field[index] += *mul_ptr;
							mul_ptr++;
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
							new_state->field[index] += *mul_ptr;
							mul_ptr++;
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
						switch (predicted_new_owner) {
							case 1:
							new_state->count0--;
							mul_ptr = mul;
							new_state->field[index] += *mul_ptr;
							mul_ptr++;
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
							new_state->field[index] += *mul_ptr;
							mul_ptr++;
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
	}
	return new_state;
}
*/

#endif