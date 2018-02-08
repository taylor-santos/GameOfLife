#ifndef BOT
#define BOT

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "bot.h"
#include "GameState.h"
#include "definitions.h"

int calculate_move(struct GameState *state) {
	int best_move = -1;
	int best_score = INT_MIN;
	struct GameState *new_state = instantiate_state();
	simulate(state, new_state);
	best_score = new_state->count0 - new_state->count1;
	if (state->your_botid == 1) {
		best_score = -best_score;
	}
	for (int y = 0; y < FIELD_HEIGHT; y++) {
		for (int x = 0; x < FIELD_WIDTH; x++) {
			int owner = state->field[x + y * FIELD_WIDTH];
			if (owner != -1) {
				clear_cell(state, x, y);
				simulate(state, new_state);

				int score = new_state->count0 - new_state->count1;
				if (state->your_botid == 1) {
					score = -score;
				}
				if (score > best_score) {
					best_score = score;
					best_move = x + y * FIELD_WIDTH;
				}
				set_cell(state, x, y, owner);
			}
		}
	}
	int owner = state->field[best_move];
	clear_cell(state, best_move%FIELD_WIDTH, best_move / FIELD_WIDTH);
	print_field(state);
	set_cell(state, best_move%FIELD_WIDTH, best_move / FIELD_WIDTH, owner);
	free(new_state);
	fprintf(stderr, "Score with optimal move: %d\n", best_score);
	return best_move;
}

void simulate(struct GameState *state, struct GameState *result) {
	copy_into(state, result);
	for (int y = 0; y < FIELD_HEIGHT; y++) {
		for (int x = 0; x < FIELD_WIDTH; x++) {
			if (state->field[x + y * FIELD_WIDTH] == -1) {
				if (state->adj_count0[x + y * FIELD_WIDTH] + state->adj_count1[x + y * FIELD_WIDTH] == 3) {
					if (state->adj_count0[x + y * FIELD_WIDTH] > state->adj_count1[x + y * FIELD_WIDTH]) {
						result->field[x + y * FIELD_WIDTH] = 0;
						result->count0++;
						for (int Y = y - 1; Y <= y + 1; Y++) {
							for (int X = x - 1; X <= x + 1; X++) {
								if ((X != x || Y != y) && X >= 0 && X < FIELD_WIDTH && Y >= 0 && Y < FIELD_HEIGHT) {
									result->adj_count0[X + Y * FIELD_WIDTH]++;
								}
							}
						}
					}
					else {
						result->field[x + y * FIELD_WIDTH] = 1;
						result->count1++;
						for (int Y = y - 1; Y <= y + 1; Y++) {
							for (int X = x - 1; X <= x + 1; X++) {
								if ((X != x || Y != y) && X >= 0 && X < FIELD_WIDTH && Y >= 0 && Y < FIELD_HEIGHT) {
									result->adj_count1[X + Y * FIELD_WIDTH]++;
								}
							}
						}
					}
				}
			}
			else {
				int adj = state->adj_count0[x + y * FIELD_WIDTH] + state->adj_count1[x + y * FIELD_WIDTH];
				if (adj < 2 || adj > 3) {
					if (state->field[x + y * FIELD_WIDTH] == 0) {
						for (int Y = y - 1; Y <= y + 1; Y++) {
							for (int X = x - 1; X <= x + 1; X++) {
								if ((X != x || Y != y) && X >= 0 && X < FIELD_WIDTH && Y >= 0 && Y < FIELD_HEIGHT) {
									result->adj_count0[X + Y * FIELD_WIDTH]--;
								}
							}
						}
						result->count0--;
					}
					else {
						for (int Y = y - 1; Y <= y + 1; Y++) {
							for (int X = x - 1; X <= x + 1; X++) {
								if ((X != x || Y != y) && X >= 0 && X < FIELD_WIDTH && Y >= 0 && Y < FIELD_HEIGHT) {
									result->adj_count1[X + Y * FIELD_WIDTH]--;
								}
							}
						}
						result->count1--;
					}
					result->field[x + y * FIELD_WIDTH] = -1;
				}
			}
		}
	}
}

#endif