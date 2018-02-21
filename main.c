#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include "definitions.h"
#include "GameState.h"
#include "parser.h"
#include "bot.h"

unsigned char your_botid = 0;

int main()
{
	struct FastState *state = instantiate_fastState();
	parse(stdin, state);
	print_fast(state);
	clock_t time = clock();
	your_botid = 1;
	unsigned char depth = 4;
	struct FastState **predictions = malloc(sizeof(*predictions) * depth);
	predictions[0] = simulate_fast(state);
	for (int i = 1; i < depth; i++) {
		predictions[i] = simulate_fast(predictions[i - 1]);
	}
	short *move = minimax(state, your_botid, predictions, depth, SHRT_MIN + 1, SHRT_MAX);
	time = clock() - time;
	fprintf(stderr, "Score: %d\n", move[0]);
	for (int i=0; i<depth; i++) {
		fprintf(stderr, "%d\n", move[i+1]);
	}
	free(move);
	printf("Time: %fms\n", (double)time / CLOCKS_PER_SEC * 1000.0);
	return 0;
}