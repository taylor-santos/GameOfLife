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
	extern int count[2][5][8][8];
	extern int total[2][5][8][8];
	for (int a = 0; a < 2; a++) {
		for (int b = 0; b < 5; b++) {
			for (int c = 0; c < 8; c++) {
				for (int d = 0; d < 8; d++) {
					count[a][b][c][d] = 0;
					total[a][b][c][d] = 0;
				}
			}
		}
	}
	struct State *state = instantiate_state();
	while (true) {
		parse(stdin, state);
	}
	return 0;
}