#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "definitions.h"
#include "GameState.h"

enum EXIT_CODES
{
	INVALID_POINTER = -1,
	BAD_DATA = -2
};

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
		state->timebank = atoi(buffer + 12);
	}
	else if (strstr(buffer, "update") != NULL) {
		if (strstr(buffer, "round") != NULL) {
			state->round = atoi(buffer + 18);
		}
		else if (strstr(buffer, "field") != NULL) {
			for (int y = 0; y < FIELD_HEIGHT; y++) {
				for (int x = 0; x < FIELD_WIDTH; x++) {
					char c = buffer[18 + 2 * (FIELD_WIDTH*y + x)];
					if (c == '0') {
						state->field[x][y] = 0;
						for (int Y = y - 1; Y <= y + 1; Y++) {
							for (int X = x - 1; X <= x + 1; X++) {
								if ((X != x || Y != y) && X >= 0 && X < FIELD_WIDTH && Y >= 0 && Y < FIELD_HEIGHT) {
									state->adj_count0[X][Y]++;
								}
							}
						}
					}
					else if (c == '1') {
						state->field[x][y] = 1;
						for (int Y = y - 1; Y <= y + 1; Y++) {
							for (int X = x - 1; X <= x + 1; X++) {
								if ((X != x || Y != y) && X >= 0 && X < FIELD_WIDTH && Y >= 0 && Y < FIELD_HEIGHT) {
									state->adj_count1[X][Y]++;
								}
							}
						}
					}
					else {
						state->field[x][y] = -1;
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