#ifndef BOT
#define BOT

#include "bot.h"
#include "GameState.h"
#include "definitions.h"

struct GameState *simulate(struct GameState *state) {
	struct GameState *new_state = copy(state);
	for (int y = 0; y < FIELD_HEIGHT; y++) {
		for (int x = 0; x < FIELD_WIDTH; x++) {
			if (state->field[x + y * FIELD_WIDTH] == -1) {
				if (state->adj_count0[x + y * FIELD_WIDTH] + state->adj_count1[x + y * FIELD_WIDTH] == 3) {
					if (state->adj_count0[x + y * FIELD_WIDTH] > state->adj_count1[x + y * FIELD_WIDTH]) {
						new_state->field[x + y * FIELD_WIDTH] = 0;
						for (int Y = y - 1; Y <= y + 1; Y++) {
							for (int X = x - 1; X <= x + 1; X++) {
								if ((X != x || Y != y) && X >= 0 && X < FIELD_WIDTH && Y >= 0 && Y < FIELD_HEIGHT) {
									new_state->adj_count0[X + Y * FIELD_WIDTH]++;
								}
							}
						}
					}
					else {
						new_state->field[x + y * FIELD_WIDTH] = 1;
						for (int Y = y - 1; Y <= y + 1; Y++) {
							for (int X = x - 1; X <= x + 1; X++) {
								if ((X != x || Y != y) && X >= 0 && X < FIELD_WIDTH && Y >= 0 && Y < FIELD_HEIGHT) {
									new_state->adj_count1[X + Y * FIELD_WIDTH]++;
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
									new_state->adj_count0[X + Y * FIELD_WIDTH]--;
								}
							}
						}
					}
					else {
						for (int Y = y - 1; Y <= y + 1; Y++) {
							for (int X = x - 1; X <= x + 1; X++) {
								if ((X != x || Y != y) && X >= 0 && X < FIELD_WIDTH && Y >= 0 && Y < FIELD_HEIGHT) {
									new_state->adj_count1[X + Y * FIELD_WIDTH]--;
								}
							}
						}
					}
					new_state->field[x + y * FIELD_WIDTH] = -1;
				}
			}
		}
	}
	return new_state;
}

#endif