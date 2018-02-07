#ifndef GAMESTATE_H
#define GAMESTATE_H

struct GameState
{
	int timebank;
	int round;
	int your_botid;
	int count0;
	int count1;
	int *field;
	int *adj_count0;
	int *adj_count1;
};

struct GameState *instantiate_state();
struct GameState *copy(struct GameState *state);
void clear_state(struct GameState **state);
void print_field(struct GameState *state);

#endif