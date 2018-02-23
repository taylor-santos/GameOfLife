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

struct FastState
{
	unsigned short *field;
	//unsigned short *changed;
	unsigned char count0; //Field is technically 288 cells large, assume a player can't get more than 256 cells
	unsigned char count1;
};

struct State *instantiate_state();
void free_state(struct State **state);
struct State *copy_state(const struct State *state, const bool copy_changes);
void set_cell(struct State *state, const unsigned short index, const unsigned char value);
void print(const struct State *state);
struct FastState *instantiate_fastState();
void free_fastState(struct FastState **state);
void print_fast(const struct FastState *state);
struct FastState *copy_fastState(const struct FastState *state, const bool copy_changes);
void set_cell_fast(struct FastState *state, const unsigned short index, const unsigned char value);
bool verify_fastState(const struct FastState *state);

#endif