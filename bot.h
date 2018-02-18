#ifndef BOT_H
#define BOT_H

#include "GameState.h"

struct thread_state
{
	const struct FastState *state;
	struct FastState *new_state;
	unsigned char old_owner;
	int row;
	int col;
};

short *best_move(const struct FastState *state, const unsigned char depth);
short minimax(const struct FastState *state, struct FastState **predictions, const unsigned char depth, short alpha, short beta);
void *simulate_threaded(void *state_ptr);
struct FastState *simulate_fast(const struct FastState *state);
struct FastState *simulate_with_prediction(const struct FastState *state, const struct FastState *prediction);

#endif