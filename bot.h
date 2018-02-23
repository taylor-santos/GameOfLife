#ifndef BOT_H
#define BOT_H

#include "GameState.h"

int *minimax(const struct State *state, const unsigned char id, const unsigned char depth, int alpha, int beta);
struct State *simulate(const struct State *state);
struct FastState *simulate_fast(const struct FastState *state);
//struct FastState *simulate_with_prediction(const struct FastState *state, const struct FastState *prediction);

#endif