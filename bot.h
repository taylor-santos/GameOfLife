#ifndef BOT_H
#define BOT_H

#include "GameState.h"

int *minimax(const struct FastState *state, const unsigned char id, struct FastState **predictions, const unsigned char depth, int alpha, int beta);
struct FastState *simulate_fast(const struct FastState *state);
struct FastState *simulate_with_prediction(const struct FastState *state, const struct FastState *prediction);

#endif