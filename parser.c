#ifndef PARSER
#define PARSER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include "definitions.h"
#include "GameState.h"
#include "bot.h"
#include "data.h"

void parse(FILE *input, struct State *state)
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
		extern unsigned char your_botid;
		clock_t t;
		t = clock();
		unsigned char depth = 2;
		struct State **predictions = malloc(sizeof(*predictions) * depth);
		predictions[0] = simulate(state);
		for (int i = 1; i < depth; i++) {
			predictions[i] = simulate(predictions[i - 1]);
		}
		int *move = minimax(state, (const struct State **)predictions, your_botid, depth, SHRT_MIN + 1, SHRT_MAX);
		fprintf(stderr, "\n");
		fprintf(stderr, "%d\n", move[1]);
		if (move[1] == -1) {
			fprintf(stdout, "pass\n");
		}
		else if (move[1] < 288){
			unsigned char x = index_to_x[move[1]];
			unsigned char y = index_to_y[move[1]];
			fprintf(stdout, "kill %d,%d\n", x, y);
		}
		else {
			unsigned short birthIndex = move[1] % 288;
			unsigned short kill1Index = (move[1] / 288) % 288;
			unsigned short kill2Index = move[1] / 82944;
			fprintf(stdout, "birth %d,%d %d,%d %d,%d\n",
				index_to_x[birthIndex], index_to_y[birthIndex],
				index_to_x[kill1Index], index_to_y[kill1Index],
				index_to_x[kill2Index], index_to_y[kill1Index]);
		}
		print(state);
		fprintf(stderr, "Score: %d\n", move[0]);
		for (int i = 0; i < depth; i++) {
			int val = move[i + 1];
			if (val == -1) {
				fprintf(stderr, "(pass)");
			}
			else {
				fprintf(stderr, "%d ", val);
				if (val >= 288)
					val -= 288;
				while (val != 0) {
					fprintf(stderr, "(%d,%d) ", (val % 288) % FIELD_WIDTH, (val % 288) / FIELD_WIDTH);
					val /= 288;
				}
			}
			fprintf(stderr, "\n");
		}
		fflush(stdout);
		free(move);
		for (int i = 0; i < depth; i++)
			free_state(&predictions[i]);
		free(predictions);
		t = clock() - t;
		fprintf(stderr, "Time: %fms\n", (double)t / CLOCKS_PER_SEC * 1000.0);
		extern int count[3][5];
		for (int y=0;y<3;y++){
			for (int x=0; x<5; x++){
				fprintf(stderr, "%d ", count[y][x]);
			}
			fprintf(stderr, "\n");
		}
		free_state(&state);
		state = instantiate_state();
	}
	else if (strstr(buffer, "update") != NULL) {
		if (strstr(buffer, "field") != NULL) {
			state->count0 = 0;
			state->count1 = 0;
			for (int y = 0; y < FIELD_HEIGHT; y++) {
				for (int x = 0; x < FIELD_WIDTH; x++) {
					char c = buffer[18 + 2 * (FIELD_WIDTH*y + x)];
					if (c == '0') {
						state->count0++;
						int index = x + y*FIELD_WIDTH;
						state->field[index] = 1;
						const unsigned short *adj = new_adjacent[index];
						for (int i=0; i<adj[0]; i++){
							state->neighbors[adj[i+1]]++;
						}
						const unsigned short m = mask[y];
						if (x > 0)
							state->changed[x - 1] |= m;
						state->changed[x] |= m;
						if (x < FIELD_WIDTH - 1)
							state->changed[x + 1] |= m;
					}
					else if (c == '1') {
						state->count1++;
						int index = x + y*FIELD_WIDTH;
						state->field[index] = 2;
						const unsigned short *adj = new_adjacent[index];
						for (int i=0; i<adj[0]; i++){
							state->neighbors[adj[i+1]] += 9;
						}
						const unsigned short m = mask[y];
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
			extern unsigned char your_botid;
			your_botid = atoi(buffer + 20);
		}
	}
	else {
		fprintf(stderr, "Invalid input.\n");
		exit(BAD_DATA);
	}
}

#endif