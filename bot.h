#ifndef BOT_H
#define BOT_H

#include "GameState.h"

int minimax(struct GameState *state, int depth);
int *calculate_move(struct GameState *state, struct GameState *new_state, int depth);
void simulate_fast(struct FastState *state);
void simulate(const struct GameState *state, struct GameState *result);

#endif