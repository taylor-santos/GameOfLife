#ifndef BOT_H
#define BOT_H

#include "GameState.h"

int calculate_move(struct GameState *state);
void simulate(struct GameState *state, struct GameState *result);

#endif