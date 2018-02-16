#ifndef BOT_H
#define BOT_H

#include "GameState.h"

short *best_move(const struct FastState *state, const unsigned char depth);
short minimax(const struct FastState *state, const struct FastState **predictions, const unsigned char depth, short alpha, short beta);
struct FastState *simulate_fast(const struct FastState *state);
struct FastState *simulate_with_prediction(const struct FastState *state, const struct FastState *prediction);

#endif