#ifndef BOT
#define BOT

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include "bot.h"
#include "GameState.h"
#include "definitions.h"
#include "data.h"

short *best_move(const struct FastState *state, const unsigned char depth) {
	if (depth == 0){
		short score = (short)(state->count0) - state->count1;
		if (state->your_botid == 1)
			score = -score;
		short *move = malloc(sizeof(*move) * 2);
		move[0] = 0;
		move[1] = score;
		return move;
	}
	/*
	struct FastState **predictions = malloc(sizeof(*predictions) * depth);
	predictions[0] = simulate_fast(state);
	predictions[0]->your_botid = !predictions[0]->your_botid;
	for (int i = 1; i < depth; i++) {
		predictions[i] = simulate_fast(predictions[i-1]);
	}
	*/

	short alpha = SHRT_MIN + 1;
	short beta = SHRT_MAX;
	struct FastState *new_state = simulate_fast(state);
	new_state->your_botid = !state->your_botid;
	short best_score = -minimax(new_state, NULL, depth - 1, -beta, -alpha);
	free_fastState(&new_state);
	if (best_score > alpha) {
		alpha = best_score;
	}
	short best_kill = -1;
	for (int y = 0; y<FIELD_HEIGHT; y++) {
		for (int x = 0; x<FIELD_WIDTH; x++) {
			unsigned short index = (x + 1) + (y + 1) * (FIELD_WIDTH + 2);
			if (mod3[state->field[index]] == !state->your_botid + 1) {
				new_state = copy_fastState(state, true);
				new_state->your_botid = !state->your_botid;
				set_cell(new_state, index, 0);
				//struct FastState *next_state = simulate_with_prediction(new_state, predictions[0]);
				struct FastState *next_state = simulate_fast(new_state);
				free_fastState(&new_state);
				int score = -minimax(next_state, NULL, depth - 1, -beta, -alpha);
				free_fastState(&next_state);
				if (score > best_score) {
					best_score = score;
					best_kill = index;
				}
				if (score > alpha) {
					alpha = score;
				}
			}
		}
	}
	for (int y = 0; y<FIELD_HEIGHT; y++) {
		for (int x = 0; x<FIELD_WIDTH; x++) {
			unsigned short index = (x + 1) + (y + 1) * (FIELD_WIDTH + 2);
			if (mod3[state->field[index]] == state->your_botid + 1) {
				new_state = copy_fastState(state, true);
				new_state->your_botid = !state->your_botid;
				set_cell(new_state, index, 0);
				//struct FastState *next_state = simulate_with_prediction(new_state, predictions[0]);
				struct FastState *next_state = simulate_fast(new_state);
				free_fastState(&new_state);
				int score = -minimax(next_state, NULL, depth - 1, -beta, -alpha);
				free_fastState(&next_state);
				if (score > best_score) {
					best_score = score;
					best_kill = index;
				}
				if (score > alpha) {
					alpha = score;
				}
			}
		}
	}
	/*
	for (int i = 0; i < depth; i++) {
		free_fastState(&(predictions[i]));
	}
	free(predictions);
	*/
	if (best_kill == -1) {
		short *move = malloc(sizeof(*move) * 2);
		move[0] = 0;
		move[1] = best_score;
		return move;
	}
	else {
		short *move = malloc(sizeof(*move) * 3);
		move[0] = 1;
		move[1] = best_score;
		move[2] = best_kill;
		return move;
	}
}

short minimax(const struct FastState *state, struct FastState **predictions, const unsigned char depth, short alpha, short beta)
{
	if (depth == 0){
		short score = (short)(state->count0) - state->count1;
		if (state->your_botid == 1)
			score = -score;
		return score;
	}
	/*
	struct FastState *next_state = simulate_with_prediction(state, predictions[0]);
	next_state->your_botid = !state->your_botid;
	*/
	struct FastState *next_state = simulate_fast(state);
	short best_score = -minimax(next_state, NULL, depth - 1, -beta, -alpha);
	free_fastState(&next_state);
	for (int y=0; y<FIELD_HEIGHT; y++){
		for (int x=0; x<FIELD_WIDTH; x++){
			unsigned short index = (x + 1) + (y + 1) * (FIELD_WIDTH + 2);
			if (mod3[state->field[index]] == state->your_botid + 1){
				struct FastState *new_state = copy_fastState(state, true);
				new_state->your_botid = !state->your_botid;
				set_cell(new_state, index, 0);
				//next_state = simulate_with_prediction(new_state, predictions[0]);
				next_state = simulate_fast(new_state);
				free_fastState(&new_state);
				int score = -minimax(next_state, NULL, depth - 1, -beta, -alpha);
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
	for (int y=0; y<FIELD_HEIGHT; y++){
		for (int x=0; x<FIELD_WIDTH; x++){
			unsigned short index = (x + 1) + (y + 1) * (FIELD_WIDTH + 2);
			if (mod3[state->field[index]] == !state->your_botid + 1){
				struct FastState *new_state = copy_fastState(state, true);
				new_state->your_botid = !state->your_botid;
				set_cell(new_state, index, 0);
				//next_state = simulate_with_prediction(new_state, predictions[0]);
				next_state = simulate_fast(new_state);
				free_fastState(&new_state);
				int score = -minimax(next_state, NULL, depth - 1, -beta, -alpha);
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

pthread_mutex_t print_mutex;

void *simulate_threaded(void *state_ptr)
{
	struct thread_state *thread_struct = state_ptr;
	const struct FastState *state = thread_struct->state;
	struct FastState *new_state = thread_struct->new_state;
	unsigned char old_owner = thread_struct->old_owner;
	unsigned char y = thread_struct->row;
	unsigned char x = thread_struct->col;
	unsigned short index = (x + 1) + (y + 1)*(FIELD_WIDTH + 2);
	pthread_mutex_lock(&print_mutex);

	print_fast(state);
	fprintf(stderr, "%d\n", index);
	pthread_mutex_unlock(&print_mutex);
	unsigned short curr = state->field[index];
	unsigned char new_owner = *(table + curr);
	char diff = 0;
	short *mul_ptr;
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
	return NULL;
}

struct FastState *simulate_fast(const struct FastState *state)
{
	struct FastState *new_state = copy_fastState(state, false);
	bool *used = calloc(sizeof(*used), FIELD_HEIGHT * FIELD_WIDTH);
	pthread_t tid[FIELD_HEIGHT*FIELD_WIDTH];
	for (unsigned char y=0; y<FIELD_HEIGHT; y++){
		for (unsigned char x=0; x<FIELD_WIDTH; x++){
			if (index_mask[y] & state->changed[x]) {
				struct thread_state *thread = malloc(sizeof(*thread));
				thread->state = state;
				thread->new_state = new_state;
				thread->old_owner = mod3[state->field[(x+1) + (y+1)*(FIELD_WIDTH+2)]];
				thread->row = y;
				thread->col = x;
				if (pthread_create(&tid[x+FIELD_WIDTH*y], NULL, simulate_threaded, (void *)thread)){
					fprintf(stderr, "Create failed.\n");
				}
				free(thread);
				used[x+FIELD_WIDTH*y] = true;
			}
		}
	}
	for (int y = 0; y < FIELD_HEIGHT; y++){
		for (unsigned char x=0; x<FIELD_WIDTH; x++){
			if (used[x + FIELD_WIDTH*y]){
				if(pthread_join(tid[x+FIELD_WIDTH*y], NULL)){
					fprintf(stderr, "Join failed.\n");
				}
			}
		}
	}
	free(used);
	return new_state;
}

struct FastState *simulate_with_prediction(const struct FastState *state, const struct FastState *prediction)
{
	struct FastState *new_state = copy_fastState(prediction, false);
	bool *used = calloc(sizeof(*used), FIELD_HEIGHT * FIELD_WIDTH);
	pthread_t tid[FIELD_HEIGHT*FIELD_WIDTH];
	for (unsigned char y=0; y<FIELD_HEIGHT; y++){
		for (unsigned char x=0; x<FIELD_WIDTH; x++){
			struct thread_state *thread = malloc(sizeof(*thread));
			thread->state = state;
			thread->new_state = new_state;
			thread->old_owner = mod3[prediction->field[(x+1) + (y+1)*(FIELD_WIDTH+2)]];
			thread->row = y;
			thread->col = x;
			if (pthread_create(&tid[x+FIELD_WIDTH*y], NULL, simulate_threaded, (void *)thread)){
				fprintf(stderr, "Create failed.\n");
			}
			free(thread);
			used[x+FIELD_WIDTH*y] = true;
			/*
			if (index_mask[y] & state->changed[x]) {
				unsigned short index = (x + 1) + (y + 1)*(FIELD_WIDTH + 2);
				unsigned char old_owner = mod3[prediction->field[index]];

				unsigned short curr = state->field[index];
				unsigned char new_owner = *(table + curr);
				
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
			*/
		}
	}
	for (int y = 0; y < FIELD_HEIGHT; y++){
		for (unsigned char x=0; x<FIELD_WIDTH; x++){
			if (used[x + FIELD_WIDTH*y]){
				if(pthread_join(tid[x+FIELD_WIDTH*y], NULL)){
					fprintf(stderr, "Join failed.\n");
				}
			}
		}
	}
	free(used);
	return new_state;
}

#endif