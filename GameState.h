#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "definitions.h"

struct FastState
{
	unsigned short timebank; //Maimum 10,000
	unsigned char round;
	unsigned char your_botid;
	unsigned char count0; //Field is technically 288 cells large, assume a player can't get more than 256 cells
	unsigned char count1;
	unsigned short *field;
	unsigned short *changed;
};

struct FastState *instantiate_fastState();
void print_fast(struct FastState *state);

#endif