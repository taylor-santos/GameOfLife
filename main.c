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
	while (true) {
		parse(stdin, state);
	}
	return 0;
}