#ifndef BOT_H
#define BOT_H

#include "GameState.h"

short minimax(const struct FastState *state, char depth);
struct FastState *simulate_fast(const struct FastState *state);
struct FastState *simulate_with_prediction(const struct FastState *state, const struct FastState *prediction);

#endif