#ifndef PARSER
#define PARSER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "definitions.h"
#include "GameState.h"
#include "bot.h"
#include "data.h"

void parse(FILE *input, struct FastState *state)
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
		print_fast(state);
		state->timebank = atoi(buffer + 12);
		t = clock();
		
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
			for (int y = 0; y < FIELD_HEIGHT; y++) {
				for (int x = 0; x < FIELD_WIDTH; x++) {
					char c = buffer[18 + 2 * (FIELD_WIDTH*y + x)];
					if (c == '0') {
						state->count0++;
						state->field[(x + 1) + (y + 1)*(FIELD_WIDTH + 2)] += 1;
						int mul = 3;
						for (int Y = y - 1; Y <= y + 1; Y++) {
							for (int X = x - 1; X <= x + 1; X++) {
								if (Y != y || X != x) {
									state->field[(X + 1) + (Y + 1)*(FIELD_WIDTH + 2)] += mul;
									mul *= 3;
								}
							}
						}
						unsigned short m = mask[y];
						if (x > 0)
							state->changed[x - 1] |= m;
						state->changed[x] |= m;
						if (x < FIELD_WIDTH - 1)
							state->changed[x + 1] |= m;
					}
					else if (c == '1') {
						state->count1++;
						state->field[(x + 1) + (y + 1)*(FIELD_WIDTH + 2)] += 2;
						int mul = 3;
						for (int Y = y - 1; Y <= y + 1; Y++) {
							for (int X = x - 1; X <= x + 1; X++) {
								if (Y != y || X != x) {
									state->field[(X + 1) + (Y + 1)*(FIELD_WIDTH + 2)] += 2 * mul;
									mul *= 3;
								}
							}
						}
						unsigned short m = mask[y];
						if (x > 0)
							state->changed[x - 1] |= m;
						state->changed[x] |= m;
						if (x < FIELD_WIDTH - 1)
							state->changed[x + 1] |= m;
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