#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <stdbool.h>
#include "definitions.h"

struct State
{
	unsigned char *field;
	unsigned char *neighbors;
	unsigned short *changed;
	unsigned char count0;
	unsigned char count1;
};
struct State *instantiate_state();
void free_state(struct State **state);
struct State *copy_state(const struct State *state, const bool copy_changes);
void copy_into(const struct State *state, const bool copy_changes, struct State *result);
void set_cell(struct State *state, const unsigned short index, const unsigned char value);
void print(const struct State *state);
bool verify(const struct State *state);
bool verify_simulation(const struct State *state1, const struct State *state2);
#endif