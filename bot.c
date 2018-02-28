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

int count[2][9][9];

int *minimax(const struct State *state, const struct State **predictions, const unsigned char id, const unsigned char depth, int alpha, int beta)
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
	unsigned short priority1_cells[288];
	unsigned short owned_cells[288];
	unsigned short enemy_cells[288];
	unsigned short priority_kill_cells[288];
	unsigned short pk = 0;
	unsigned short non_priority_kill_cells[288];
	unsigned short npk = 0;
	unsigned short priority_birth_cells[288];
	unsigned short pb = 0;
	unsigned short non_priority_birth_cells[288];
	unsigned short npb = 0;
	unsigned short priority1_index = 0;
	unsigned short owned_index = 0;
	unsigned short enemy_index = 0;

	int best_score = SHRT_MIN + 1;
	for (int y=0; y<FIELD_HEIGHT; y++){
		for (int x=0; x<FIELD_WIDTH; x++){
			unsigned short index = x + y*FIELD_WIDTH;
			if (state->neighbors[index] != 0) {
				unsigned char owner = state->field[index];
				unsigned char n0 = state->neighbors[index] % 9;
				unsigned char n1 = state->neighbors[index] / 9;
				
				if (owner == 0) {
					if (n0 == 1 && n1 == 1){
						priority_birth_cells[pb++] = index;
					}else if (n0+n1 > 1){
						non_priority_birth_cells[npb++] = index;
					}
				}
				else if (owner == id + 1) {
					owned_cells[owned_index++] = index;
					if (n0 + n1 == 2){
						priority_kill_cells[pk++] = index;
					}else{
						non_priority_kill_cells[npk++] = index;
					}
				}
				else {
					if (owner == 1 && n0 == 0 && n1 == 2) {
						priority1_cells[priority1_index++] = index;
					}
					else if (owner == 2 && n1 == 0 && n0 == 2) {
						priority1_cells[priority1_index++] = index;
					}
					else {
						enemy_cells[enemy_index++] = index;
					}
				}
			}
		}
	}

	struct State **next_predictions = malloc(sizeof(*next_predictions) * depth);
	for (int i = 0; i < depth; i++) {
		next_predictions[i] = instantiate_state();
	}
	struct State *next_state = copy_state(state, false);
	
	for (int i = 0; i < pb + npb; i++) {
		int birth_index;
		if (i >= pb)
			birth_index = non_priority_birth_cells[i - pb];
		else
			birth_index = priority_birth_cells[i];
		for (int j = 0; j < pk + npk; j++) {
			int kill_index1;
			if (j >= pk)
				kill_index1 = non_priority_kill_cells[j - pk];
			else
				kill_index1 = priority_kill_cells[j];
			unsigned char kill_cell1 = mod3[next_state->field[kill_index1]];
			for (int k = j + 1; k < pk + npk; k++) {
				int kill_index2;
				if (k >= pk)
					kill_index2 = non_priority_kill_cells[k - pk];
				else
					kill_index2 = priority_kill_cells[k];
				unsigned char kill_cell2 = mod3[next_state->field[kill_index2]];
				set_cell(next_state, birth_index, id + 1);
				set_cell(next_state, kill_index1, 0);
				set_cell(next_state, kill_index2, 0);
				simulate_with_prediction(next_state, predictions[0], next_predictions[0]);
				set_cell(next_state, kill_index2, kill_cell2);
				set_cell(next_state, kill_index1, kill_cell1);
				set_cell(next_state, birth_index, 0);
				for (int n = 0; n < FIELD_WIDTH; n++) {
					next_state->changed[n] = 0;
				}
				for (int n = 1; n < depth; n++) {
					simulate_with_prediction(next_predictions[n - 1], predictions[n], next_predictions[n]);
				}
				int *result = minimax(next_predictions[0], next_predictions + 1, !id, depth - 1, -beta, -alpha);
				int score = -result[0];
				if (score > best_score) {
					best_score = score;
					best_sequence[0] = birth_index + 288 + 288 * kill_index1 + 82944 * kill_index2;
					for (int n = 1; n < depth; n++)
						best_sequence[n] = (int)result[n];
				}
				if (score > alpha) {
					alpha = score;
				}
				if (beta <= alpha) {
					free(best_sequence);
					int *move = malloc(sizeof(*move) *(depth + 1));
					move[0] = beta;
					for (int n = 1; n < depth + 1; n++)
						move[n] = -3;
					free(result);
					free_state(&next_state);
					for (int n = 0; n < depth; n++) {
						free_state(&next_predictions[n]);
					}
					free(next_predictions);
					return move;
				}
				free(result);
			}
		}
	}

	for (int i = 0; i < priority1_index; i++) {
		unsigned short index = priority1_cells[i];
		unsigned char old_cell = mod3[next_state->field[index]];
		set_cell(next_state, index, 0);
		simulate_with_prediction(next_state, predictions[0], next_predictions[0]);
		set_cell(next_state, index, old_cell);
		for (int n = 0; n < FIELD_WIDTH; n++) {
			next_state->changed[n] = 0;
		}
		for (int j = 1; j < depth; j++) {
			simulate_with_prediction(next_predictions[j - 1], predictions[j], next_predictions[j]);
		}
		int *result = minimax(next_predictions[0], next_predictions + 1, !id, depth - 1, -beta, -alpha);
		int score = -result[0];
		if (score > best_score) {
			best_score = score;
			best_sequence[0] = index;
			for (int n = 1; n < depth; n++)
				best_sequence[n] = (int)result[n];
		}
		if (score > alpha) {
			alpha = score;
		}
		if (beta <= alpha) {
			free(best_sequence);
			int *move = malloc(sizeof(*move) *(depth + 1));
			move[0] = beta;
			for (int n = 1; n < depth + 1; n++)
				move[n] = -3;
			free(result);
			for (int n = 0; n < depth; n++) {
				free_state(&next_predictions[n]);
			}
			free(next_predictions);
			free_state(&next_state);
			return move;
		}
		free(result);
	}
	for (int i = 0; i < enemy_index; i++) {
		unsigned short index = enemy_cells[i];
		unsigned char old_cell = mod3[next_state->field[index]];
		set_cell(next_state, index, 0);
		simulate_with_prediction(next_state, predictions[0], next_predictions[0]);
		set_cell(next_state, index, old_cell);
		for (int n = 0; n < FIELD_WIDTH; n++) {
			next_state->changed[n] = 0;
		}
		for (int j = 1; j < depth; j++) {
			simulate_with_prediction(next_predictions[j - 1], predictions[j], next_predictions[j]);
		}
		int *result = minimax(next_predictions[0], next_predictions + 1, !id, depth - 1, -beta, -alpha);
		int score = -result[0];
		if (score > best_score) {
			best_score = score;
			best_sequence[0] = index;
			for (int n = 1; n < depth; n++)
				best_sequence[n] = (int)result[n];
		}
		if (score > alpha) {
			alpha = score;
		}
		if (beta <= alpha) {
			free(best_sequence);
			int *move = malloc(sizeof(*move) *(depth + 1));
			move[0] = beta;
			for (int n = 1; n < depth + 1; n++)
				move[n] = -3;
			free(result);
			for (int n = 0; n < depth; n++) {
				free_state(&next_predictions[n]);
			}
			free(next_predictions);
			free_state(&next_state);
			return move;
		}
		free(result);
	}
	for(int i=0; i<owned_index; i++){
		unsigned short index = owned_cells[i];
		unsigned char old_cell = mod3[next_state->field[index]];
		set_cell(next_state, index, 0);
		simulate_with_prediction(next_state, predictions[0], next_predictions[0]);
		set_cell(next_state, index, old_cell);
		for (int n = 0; n < FIELD_WIDTH; n++) {
			next_state->changed[n] = 0;
		}
		for (int j = 1; j < depth; j++) {
			simulate_with_prediction(next_predictions[j - 1], predictions[j], next_predictions[j]);
		}
		int *result = minimax(next_predictions[0], next_predictions + 1, !id, depth - 1, -beta, -alpha);
		int score = -result[0];
		if (score > best_score) {
			best_score = score;
			best_sequence[0] = index;
			for (int n = 1; n < depth; n++)
				best_sequence[n] = (int)result[n];
		}
		if (score > alpha) {
			alpha = score;
		}
		if (beta <= alpha) {
			free(best_sequence);
			int *move = malloc(sizeof(*move) *(depth + 1));
			move[0] = beta;
			for (int n = 1; n < depth + 1; n++)
				move[n] = -3;
			free(result);
			for (int n = 0; n < depth; n++) {
				free_state(&next_predictions[n]);
			}
			free(next_predictions);
			free_state(&next_state);
			return move;
		}
		free(result);
	}
	free_state(&next_state);
	int *pass_result = minimax(predictions[0], predictions + 1, !id, depth - 1, -beta, -alpha);
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

struct State *simulate(const struct State *state) {
	struct State *new_state = copy_state(state, false);
	for (unsigned char x = 0; x < FIELD_WIDTH; x++) {
		if (state->changed[x]) {
			for (unsigned char y = 0; y < FIELD_HEIGHT; y++) {
				unsigned short index = x + y * FIELD_WIDTH;
				if (state->changed[x] & index_mask[y]) {
					unsigned char n = state->neighbors[index] + 81 * state->field[index];
					char change = diff[n];
					if (change) {
						set_cell(new_state, index, state->field[index] + change);
					}
				}
			}
		}
	}
	return new_state;
}

void simulate_with_prediction(const struct State *state, const struct State *prediction, struct State *result) {
	result->count0 = prediction->count0;
	result->count1 = prediction->count1;
	memcpy(result->field, prediction->field, sizeof(*result->field)*FIELD_WIDTH*FIELD_HEIGHT);
	memcpy(result->neighbors, prediction->neighbors, sizeof(*result->neighbors)*FIELD_WIDTH*FIELD_HEIGHT);
	for (unsigned char x = 0; x < FIELD_WIDTH; x++) {
		result->changed[x] = 0;
	}
	for (unsigned char x = 0; x < FIELD_WIDTH; x++) {
		if (state->changed[x]) {
			for (unsigned char y = 0; y < FIELD_HEIGHT; y++) {
				if (state->changed[x] & index_mask[y]) {
					unsigned short index = x + y * FIELD_WIDTH;
					unsigned char n = state->neighbors[index] + 81 * state->field[index];
					char change = diff[n];
					if (state->field[index] + change != prediction->field[index]) {
						set_cell(result, index, state->field[index] + change);
					}
				}
			}
		}
	}
}

#endif