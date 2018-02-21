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
	clock_t time = clock();
	unsigned char depth = 4;
	struct FastState **predictions = malloc(sizeof(*predictions) * depth);
	predictions[0] = simulate_fast(state);
	for (int i = 1; i < depth; i++) {
		predictions[i] = simulate_fast(predictions[i - 1]);
	}
	short *move = minimax(state, your_botid, predictions, depth, SHRT_MIN + 1, SHRT_MAX);
	time = clock() - time;
	fprintf(stderr, "%d\n%d\n", move[1], -move[0]);
	if (move[1]) {
		fprintf(stderr, "%d\n", move[2]);
	}
	free(move);
	printf("%f\n", (double)time / CLOCKS_PER_SEC * 1000.0);
	return 0;
}