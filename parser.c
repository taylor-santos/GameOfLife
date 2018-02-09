#ifndef PARSER
#define PARSER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "definitions.h"
#include "GameState.h"
#include "bot.h"

void parse(FILE *input, struct GameState *state)
{
	char buffer[BUFFER_SIZE];
	for (int i = 0; i<BUFFER_SIZE; i++) {
		buffer[i] = 0;
	}
	if (fgets(buffer, BUFFER_SIZE, input) == NULL) {
		fprintf(stderr, "Failed to read from input.\n");
		exit(BAD_DATA);
	}
	if (strstr(buffer, "action") != NULL) {
		clock_t t;
		print_field(state);
		state->timebank = atoi(buffer + 12);
		t = clock();
		struct GameState *new_state = instantiate_state();
		int* move = calculate_move(state, new_state, 2);
		clear_state(&new_state);

		if (move[0] == 0) {
			fprintf(stderr, "Best move: pass\n");
			fprintf(stdout, "pass\n");
		}
		else if (move[0] == 1) {
			int x = move[1] % FIELD_WIDTH;
			int y = move[1] / FIELD_WIDTH;
			fprintf(stderr, "Best move: kill %d,%d\n", x, y);
			fprintf(stdout, "kill %d,%d\n", x, y);
		}
		else {
			int i = move[1];
			int ix = i % FIELD_WIDTH;
			int iy = i / FIELD_WIDTH;
			int j = move[2];
			int jx = j % FIELD_WIDTH;
			int jy = j / FIELD_WIDTH;
			int k = move[3];
			int kx = k % FIELD_WIDTH;
			int ky = k / FIELD_WIDTH;
			fprintf(stderr, "Best move: birth %d,%d %d,%d %d,%d\n", ix, iy, jx, jy, kx, ky);
			fprintf(stdout, "birth %d,%d %d,%d %d,%d\n", ix, iy, jx, jy, kx, ky);
		}
		free(move);
		fflush(stdout);
		t = clock() - t;
		double ms = ((double)t) / CLOCKS_PER_SEC * 1000;
		fprintf(stderr, "Time taken: %dms\n", (int)ms);
	}
	else if (strstr(buffer, "update") != NULL) {
		if (strstr(buffer, "round") != NULL) {
			state->round = atoi(buffer + 18);
		}
		else if (strstr(buffer, "field") != NULL) {
			state->count0 = 0;
			state->count1 = 0;
			for (int i = 0; i < FIELD_WIDTH*FIELD_HEIGHT; i++) {
				state->adj_count0[i] = 0;
				state->adj_count1[i] = 0;
			}
			for (int y = 0; y < FIELD_HEIGHT; y++) {
				for (int x = 0; x < FIELD_WIDTH; x++) {
					char c = buffer[18 + 2 * (FIELD_WIDTH*y + x)];
					if (c == '0') {
						state->field[x + y * FIELD_WIDTH] = 0;
						state->count0++;
						for (int Y = y - 1; Y <= y + 1; Y++) {
							for (int X = x - 1; X <= x + 1; X++) {
								if ((X != x || Y != y) && X >= 0 && X < FIELD_WIDTH && Y >= 0 && Y < FIELD_HEIGHT) {
									state->adj_count0[X + Y*FIELD_WIDTH]++;
								}
							}
						}
					}
					else if (c == '1') {
						state->field[x + y * FIELD_WIDTH] = 1;
						state->count1++;
						for (int Y = y - 1; Y <= y + 1; Y++) {
							for (int X = x - 1; X <= x + 1; X++) {
								if ((X != x || Y != y) && X >= 0 && X < FIELD_WIDTH && Y >= 0 && Y < FIELD_HEIGHT) {
									state->adj_count1[X + Y*FIELD_WIDTH]++;
								}
							}
						}
					}
					else {
						state->field[x + y * FIELD_WIDTH] = -1;
					}
				}
			}
		}
	}
	else if (strstr(buffer, "settings") != NULL) {
		if (strstr(buffer, "your_botid") != NULL) {
			state->your_botid = atoi(buffer + 20);
		}
	}
	else {
		fprintf(stderr, "Invalid input.\n");
		exit(BAD_DATA);
	}
}

#endif