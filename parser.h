#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>

enum EXIT_CODES;

void parse(FILE *input, struct GameState *state);

#endif