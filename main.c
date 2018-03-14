#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include "definitions.h"
#include "data.h"
#include "GameState.h"
#include "parser.h"
#include "bot.h"

unsigned char your_botid = 0;

int main()
{
	/*
	extern int count[2][8][8][8][8][8][8][8];
	for (int a=0; a<2; a++){
		for (int b=0;b<8;b++){
			for (int c=0;c<8;c++){
				for (int d=0;d<8;d++){
					for (int e=0; e<8; e++){
						for (int f=0; f<8; f++){
							for (int g=0; g<8; g++){
								for (int h=0; h<8; h++){
									count[a][b][c][d][e][f][g][h] = 0;
								}
							}
						}
					}
				}
			}
		}
	}
	*/
	clock_t t;
	clock_t max = 0;
	clock_t min = INT_MAX;
	t = clock();
	unsigned int n = 0;
	srand(time(NULL));
	/*
	extern int count[8][8];
	extern int total[8][8];
	for (int a=0; a<8; a++){
		for (int b=0; b<8; b++){
			count[a][b] = 0;
			total[a][b] = 0;
		}
	}
	*/
	/*
	for (int test = 0; test < 100000; test++) {
		clock_t t1 = clock();
		struct State *state = instantiate_state();
		for (int n = 0; n < 50; n++) {
			int x = rand() % FIELD_WIDTH;
			int y = rand() % FIELD_HEIGHT;
			int index = x + y * FIELD_WIDTH;
			int anti_index = 287 - index;
			set_cell(state, index, 1);
			set_cell(state, anti_index, 2);
		}
		unsigned char depth = 2;
		struct State **predictions = malloc(sizeof(*predictions) * depth);
		predictions[0] = simulate(state);
		for (int i = 1; i < depth; i++) {
			predictions[i] = simulate(predictions[i - 1]);
		}
		free(state->changed);
		state->changed = calloc(sizeof(*state->changed), FIELD_WIDTH);
		int *move = minimax(state, (const struct State **)predictions, 0, depth, SHRT_MIN + 1, SHRT_MAX);
		print(state);
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
		
		for (int i = 0; i < depth; i++)
			free_state(&predictions[i]);
		free(predictions);
		free(move);
		
		for (int y=0; y<8; y++){
			for (int x=0; x<8; x++){
				fprintf(stderr, "%d\t", count[x][y]);
			}
			fprintf(stderr, "\n");
		}
		fprintf(stderr, "\n");
		for (int y=0; y<8; y++){
			for (int x=0; x<8; x++){
				fprintf(stderr, "%d\t", total[x][y]);
			}
			fprintf(stderr, "\n");
		}
		fprintf(stderr, "\n");
		
		clock_t t2 = clock() - t1;
		t += t2;
		n++;
		//max = t2 > max ? t2 : max;
		if (t2 > max){
			max = t2;
			print(state);
			fprintf(stderr, "Time: %fms\n", (double)t / CLOCKS_PER_SEC * 1000.0 / n);
			fprintf(stderr, "Max:  %fms\n", (double)max / CLOCKS_PER_SEC * 1000.0);
			fprintf(stderr, "%d\n", n);
		}
		min = t2 < min ? t2 : min;
		
		fprintf(stderr, "Time: %fms\n", (double)t / CLOCKS_PER_SEC * 1000.0 / n);
		fprintf(stderr, "Max:  %fms\n", (double)max / CLOCKS_PER_SEC * 1000.0);
		fprintf(stderr, "Min:  %fms\n", (double)min / CLOCKS_PER_SEC * 1000.0);
		
		free_state(&state);
	}
	*/
	
	struct State *state = instantiate_state();
	while (true) {
		parse(stdin, state);
	}
	return 0;
}