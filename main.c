#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "definitions.h"
#include "GameState.h"
#include "parser.h"
#include "bot.h"

int main()
{
	struct FastState *state = instantiate_fastState();
	parse(stdin, state);	
	print_fast(state);
	clock_t time = clock();
	for (int i = 0; i < 1000000; i++) {
		simulate_fast(state);
		print_fast(state);
	}
	time = clock() - time;
	printf("%f\n", (double)time / CLOCKS_PER_SEC * 1000.0);
	return 0;
}