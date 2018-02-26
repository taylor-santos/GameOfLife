#ifndef BOT_H
#define BOT_H

#include "GameState.h"

int *minimax(const struct State *state, const struct State **predictions, const unsigned char id, const unsigned char depth, int alpha, int beta);
struct State *simulate(const struct State *state);
struct State *simulate_with_prediction(const struct State *state, const struct State *prediction);

#endif