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

int count[2][7][15];

void add(struct State *state, unsigned short index){
	unsigned char dead_adj_with_count[3] = {0,0,0}; //2,3,4
	unsigned char alive_adj_with_count[4] = {0,0,0,0}; //1,2,3,4
	for (int i=0; i<new_adjacent[index][0]; i++){
		unsigned short n = state->neighbors[new_adjacent[index][i+1]];
		unsigned short c = n%9 + n/9;
		if (state->field[n] == 0){
			switch(c){
				case 2:
					dead_adj_with_count[0]++;
					break;
				case 3:
					dead_adj_with_count[1]++;
					break;
				case 4:
					dead_adj_with_count[2]++;
					break;
			}
		}else{
			switch(c){
				case 1:
					alive_adj_with_count[0]++;
					break;
				case 2:
					alive_adj_with_count[1]++;
					break;
				case 3:
					alive_adj_with_count[2]++;
					break;
				case 4:
					alive_adj_with_count[3]++;
					break;
			}
		}
	}
	count[!!state->field[index]][!!dead_adj_with_count[0] + 2*!!dead_adj_with_count[1] + 4*!!dead_adj_with_count[2]][!!alive_adj_with_count[0] + 2*!!alive_adj_with_count[1] + 4*!!alive_adj_with_count[2] + 8*!!alive_adj_with_count[3]]++;
}

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
	unsigned short kill_cells[5][288];
	unsigned short kill_index[5] = {0,0,0,0,0};
	unsigned short kill_count = 0;
	unsigned short birth_cells[5][288];
	unsigned short birth_index[5] = {0,0,0,0,0};
	unsigned short birth_count = 0;
	unsigned short priority1_index = 0;
	unsigned short owned_index = 0;
	unsigned short enemy_index = 0;

	int best_score = SHRT_MIN + 1;
	int best_n[3];
	for (int y=0; y<FIELD_HEIGHT; y++){
		for (int x=0; x<FIELD_WIDTH; x++){
			unsigned short index = x + y*FIELD_WIDTH;
			if (state->neighbors[index] != 0) {
				unsigned char owner = state->field[index];
				unsigned char n0 = state->neighbors[index] % 9;
				unsigned char n1 = state->neighbors[index] / 9;
				
				if (owner == 0) {
					if (n0 == 1 && n1 == 1){
						birth_cells[0][birth_index[0]++] = index;
					}else if (n0+n1 == 1){
						birth_cells[1][birth_index[1]++] = index;
					}else if ((n0==2 && n1 < 2) || (n1==2 && n0 < 2)){
						birth_cells[2][birth_index[2]++] = index;
					}else if ((n0==3 && n1 < 2) || (n0==2 && n1==2) || (n1==3 && n0<2)){
						birth_cells[3][birth_index[3]++] = index;
					}else{
						birth_cells[4][birth_index[4]++] = index;
					}
					birth_count++;
				}
				else if (owner == id + 1) {
					owned_cells[owned_index++] = index;
					if (n0 == 1 && n1 == 1){
						kill_cells[0][kill_index[0]++] = index;
					}else if (n0+n1 == 1){
						kill_cells[1][kill_index[1]++] = index;
					}else if ((n0==2 && n1 < 2) || (n1==2 && n0 < 2)){
						kill_cells[2][kill_index[2]++] = index;
					}else if ((n0==3 && n1 < 2) || (n0==2 && n1==2) || (n1==3 && n0<2)){
						kill_cells[3][kill_index[3]++] = index;
					}else{
						kill_cells[4][kill_index[4]++] = index;
					}
					kill_count++;
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
	
	for (int i = 0; i < birth_count; i++) {
		int curr_index = i;
		int n_i;
		for (n_i = 0; birth_index[n_i] <= curr_index; n_i++)
			curr_index -= birth_index[n_i];
		int birth_i = birth_cells[n_i][curr_index];
		for (int j = 0; j < kill_count-1; j++) {
			curr_index = j;
			int n_j;
			for (n_j = 0; kill_index[n_j] <= curr_index; n_j++)
				curr_index -= kill_index[n_j];
			int kill_j = kill_cells[n_j][curr_index];
			unsigned char kill_cell_j = state->field[kill_j];
			for (int k = j + 1; k < kill_count; k++) {
				curr_index = k;

				int n_k;
				for (n_k = 0; kill_index[n_k] <= curr_index; n_k++)
					curr_index -= kill_index[n_k];
				int kill_k = kill_cells[n_k][curr_index];
				unsigned char kill_cell_k = state->field[kill_k];
				set_cell(next_state, birth_i, id + 1);
				set_cell(next_state, kill_j, 0);
				set_cell(next_state, kill_k, 0);
				simulate_with_prediction(next_state, predictions[0], next_predictions[0]);
				set_cell(next_state, kill_k, kill_cell_k);
				set_cell(next_state, kill_j, kill_cell_j);
				set_cell(next_state, birth_i, 0);
				for (int n = 0; n < FIELD_WIDTH; n++) {
					next_state->changed[n] = 0;
				}
				for (int n = 1; n < depth; n++) {
					simulate_with_prediction(next_predictions[n - 1], predictions[n], next_predictions[n]);
				}
				int *result = minimax(next_predictions[0], (const struct State **)(next_predictions + 1), !id, depth - 1, -beta, -alpha);
				int score = -result[0];
				if (score > best_score) {
					best_score = score;
					best_sequence[0] = birth_i + 288 + 288 * kill_j + 82944 * kill_k;
					best_n[0] = n_i;
					best_n[1] = n_j;
					best_n[2] = n_k;
					for (int n = 1; n < depth; n++)
						best_sequence[n] = (int)result[n];
				}
				if (score > alpha) {
					alpha = score;
				}
				if (beta <= alpha) {
					//count[best_n[0]][best_n[1]][best_n[2]]++;
					add(state, birth_index[best_n[0]]);
					add(state, kill_index[best_n[1]]);
					add(state, kill_index[best_n[2]]);
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
		int *result = minimax(next_predictions[0], (const struct State **)(next_predictions + 1), !id, depth - 1, -beta, -alpha);
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
			//count[best_n[0]][best_n[1]][best_n[2]]++;
			add(state, birth_index[best_n[0]]);
			add(state, kill_index[best_n[1]]);
			add(state, kill_index[best_n[2]]);
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
		int *result = minimax(next_predictions[0], (const struct State **)(next_predictions + 1), !id, depth - 1, -beta, -alpha);
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
			//count[best_n[0]][best_n[1]][best_n[2]]++;
			add(state, birth_index[best_n[0]]);
			add(state, kill_index[best_n[1]]);
			add(state, kill_index[best_n[2]]);
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
		int *result = minimax(next_predictions[0], (const struct State **)(next_predictions + 1), !id, depth - 1, -beta, -alpha);
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
			//count[best_n[0]][best_n[1]][best_n[2]]++;
			add(state, birth_index[best_n[0]]);
			add(state, kill_index[best_n[1]]);
			add(state, kill_index[best_n[2]]);
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
	int *pass_result = minimax(predictions[0], (const struct State **)(predictions + 1), !id, depth - 1, -beta, -alpha);
	int pass_score = -pass_result[0];
	if (pass_score > best_score) {
		best_score = pass_score;
		best_sequence[0] = -1;
		for (int i = 1; i<depth; i++)
			best_sequence[i] = pass_result[i];
	}
	free(pass_result);
	//count[best_n[0]][best_n[1]][best_n[2]]++;
	add(state, birth_index[best_n[0]]);
	add(state, kill_index[best_n[1]]);
	add(state, kill_index[best_n[2]]);
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