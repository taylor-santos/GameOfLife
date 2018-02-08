#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "definitions.h"
#include "GameState.h"
#include "parser.h"
#include "bot.h"

int main()
{
	setlocale(LC_ALL, "");
	struct GameState *state = instantiate_state();
	while (1) {
		parse(stdin, state);
	}
	clear_state(&state);
	return 0;
}