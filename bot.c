#ifndef BOT
#define BOT

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "bot.h"
#include "GameState.h"
#include "definitions.h"

int minimax(struct GameState *state, int depth) {
	if (depth <= 0) {
		int score = state->count0 - state->count1;
		if (state->your_botid == 1) {
			score = -score;
		}
		return score;
	}
	struct GameState *new_state = instantiate_state();
	simulate(state, new_state);
	new_state->your_botid = !state->your_botid;
	int score_pass = -minimax(new_state, depth - 1);
	int score_kill = INT_MIN;
	for (int y = 0; y < FIELD_HEIGHT; y++) {
		for (int x = 0; x < FIELD_WIDTH; x++) {
			int owner = state->field[x + y * FIELD_WIDTH];
			if (owner != -1) {
				clear_cell(state, x, y);
				simulate(state, new_state);
				new_state->your_botid = !state->your_botid;
				int score = -minimax(new_state, depth - 1);
				if (score > score_kill) {
					score_kill = score;
				}
				set_cell(state, x, y, owner);
			}
		}
	}
	int best_move_birth[3] = { -1,-1,-1 };
	int score_birth = INT_MIN;

	score_birth = INT_MIN;
	for (int i = 0; i < FIELD_WIDTH*FIELD_HEIGHT; i++) {
		if (state->field[i] == state->your_botid) {
			clear_cell(state, i % FIELD_WIDTH, i / FIELD_WIDTH);
			simulate(state, new_state);
			new_state->your_botid = !state->your_botid;
			int score = -minimax(new_state, depth - 1);
			if (score > score_birth) {
				score_birth = score;
				best_move_birth[1] = i;
			}
			set_cell(state, i % FIELD_WIDTH, i / FIELD_WIDTH, state->your_botid);
		}
	}
	clear_cell(state, best_move_birth[1] % FIELD_WIDTH, best_move_birth[1] / FIELD_WIDTH);

	score_birth = INT_MIN;
	for (int i = 0; i < FIELD_WIDTH*FIELD_HEIGHT; i++) {
		if (state->field[i] == state->your_botid) {
			clear_cell(state, i % FIELD_WIDTH, i / FIELD_WIDTH);
			simulate(state, new_state);
			new_state->your_botid = !state->your_botid;
			int score = -minimax(new_state, depth - 1);
			if (score > score_birth) {
				score_birth = score;
				best_move_birth[2] = i;
			}
			set_cell(state, i % FIELD_WIDTH, i / FIELD_WIDTH, state->your_botid);
		}
	}
	clear_cell(state, best_move_birth[2] % FIELD_WIDTH, best_move_birth[2] / FIELD_WIDTH);

	score_birth = INT_MIN;
	for (int i = 0; i < FIELD_WIDTH*FIELD_HEIGHT; i++) {
		if (i != best_move_birth[1] &&
			i != best_move_birth[2] &&
			state->field[i] == -1 &&
			(state->adj_count0[i] != 0 || state->adj_count1[i] != 0)) {
			set_cell(state, i % FIELD_WIDTH, i / FIELD_WIDTH, state->your_botid);
			simulate(state, new_state);
			new_state->your_botid = !state->your_botid;
			int score = -minimax(new_state, depth - 1);
			if (score > score_birth) {
				score_birth = score;
				best_move_birth[0] = i;
			}
			clear_cell(state, i % FIELD_WIDTH, i / FIELD_WIDTH);
		}
	}

	set_cell(state, best_move_birth[2] % FIELD_WIDTH, best_move_birth[2] / FIELD_WIDTH, state->your_botid);
	set_cell(state, best_move_birth[1] % FIELD_WIDTH, best_move_birth[1] / FIELD_WIDTH, state->your_botid);

	clear_state(&new_state);
	int score = score_pass;
	if (score_kill > score)
		score = score_kill;
	if (score_birth > score_kill)
		score = score_birth;
	return score;
}

int *calculate_move(struct GameState *state, struct GameState *new_state, int depth) {
	simulate(state, new_state);
	new_state->your_botid = !state->your_botid;
	int best_score = -minimax(new_state, depth - 1);
	int best_move_type = 0; //pass
	int best_move_kill = -1;
	for (int y = 0; y < FIELD_HEIGHT; y++) {
		for (int x = 0; x < FIELD_WIDTH; x++) {
			int owner = state->field[x + y * FIELD_WIDTH];
			if (owner != -1) {
				clear_cell(state, x, y);
				simulate(state, new_state);
				new_state->your_botid = !state->your_botid;
				int score = -minimax(new_state, depth - 1);
				if (score > best_score) {
					best_score = score;
					best_move_kill = x + y * FIELD_WIDTH;
					best_move_type = 1;
				}
				set_cell(state, x, y, owner);
			}
		}
	}
	fprintf(stderr, "Best kill score: %d\n", best_score);
	int owner = state->field[best_move_kill];
	clear_cell(state, best_move_kill % FIELD_WIDTH, best_move_kill / FIELD_WIDTH);
	print_field(new_state);
	set_cell(state, best_move_kill % FIELD_WIDTH, best_move_kill / FIELD_WIDTH, owner);

	int best_move_birth[3] = { -1,-1,-1 };
	int best_birth_score = INT_MIN;

	best_birth_score = INT_MIN;
	for (int i = 0; i < FIELD_WIDTH*FIELD_HEIGHT; i++) {
		if (state->field[i] == state->your_botid) {
			clear_cell(state, i % FIELD_WIDTH, i / FIELD_WIDTH);
			simulate(state, new_state);
			new_state->your_botid = !state->your_botid;
			int score = -minimax(new_state, depth - 1);
			if (score > best_birth_score) {
				best_birth_score = score;
				best_move_birth[1] = i;
			}
			set_cell(state, i % FIELD_WIDTH, i / FIELD_WIDTH, state->your_botid);
		}
	}
	clear_cell(state, best_move_birth[1] % FIELD_WIDTH, best_move_birth[1] / FIELD_WIDTH);
	
	best_birth_score = INT_MIN;
	for (int i = 0; i < FIELD_WIDTH*FIELD_HEIGHT; i++) {
		if (state->field[i] == state->your_botid) {
			clear_cell(state, i % FIELD_WIDTH, i / FIELD_WIDTH);
			simulate(state, new_state);
			int score = new_state->count0 - new_state->count1;
			if (score > best_birth_score) {
				best_birth_score = score;
				best_move_birth[2] = i;
			}	
			set_cell(state, i % FIELD_WIDTH, i / FIELD_WIDTH, state->your_botid);
		}
	}
	clear_cell(state, best_move_birth[2] % FIELD_WIDTH, best_move_birth[2] / FIELD_WIDTH);

	best_birth_score = INT_MIN;
	for (int i = 0; i < FIELD_WIDTH*FIELD_HEIGHT; i++) {
		if (i != best_move_birth[1] && 
			i != best_move_birth[2] && 
			state->field[i] == -1 && 
			(state->adj_count0[i] != 0 || state->adj_count1[i] != 0)) {
			set_cell(state, i % FIELD_WIDTH, i / FIELD_WIDTH, state->your_botid);
			simulate(state, new_state);
			new_state->your_botid = !state->your_botid;
			int score = -minimax(new_state, depth - 1);
			if (score > best_birth_score) {
				best_birth_score = score;
				best_move_birth[0] = i;
			}
			clear_cell(state, i % FIELD_WIDTH, i / FIELD_WIDTH);
		}
	}
	set_cell(state, best_move_birth[0] % FIELD_WIDTH, best_move_birth[0] / FIELD_WIDTH, state->your_botid);

	fprintf(stderr, "Best birth score: %d\n", best_birth_score);
	print_field(new_state);
	set_cell(state, best_move_birth[2] % FIELD_WIDTH, best_move_birth[2] / FIELD_WIDTH, state->your_botid);
	set_cell(state, best_move_birth[1] % FIELD_WIDTH, best_move_birth[1] / FIELD_WIDTH, state->your_botid);
	clear_cell(state, best_move_birth[0] % FIELD_WIDTH, best_move_birth[0] / FIELD_WIDTH);

	if (best_birth_score > best_score) {
		best_score = best_birth_score;
		best_move_type = 2;
	}
	if (best_move_type == 0) {
		int* ret = malloc(sizeof(*ret));
		ret[0] = 0;
		return ret;
	}
	else if (best_move_type == 1) {
		int* ret = malloc(sizeof(*ret) * 2);
		ret[0] = 1;
		ret[1] = best_move_kill;
		return ret;
	}
	else {
		int* ret = malloc(sizeof(*ret) * 4);
		ret[0] = 2;
		ret[1] = best_move_birth[0];
		ret[2] = best_move_birth[1];
		ret[3] = best_move_birth[2];
		return ret;
	}
}

void simulate(const struct GameState *state, struct GameState *result) {
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