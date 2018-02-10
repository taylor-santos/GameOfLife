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
struct FastState
{
	unsigned short timebank; //Maimum 10,000
	unsigned char round;
	unsigned char your_botid;
	unsigned char count0; //Field is technically 288 cells large, assume a player can't get more than 256 cells
	unsigned char count1;
	short *field;
};

struct FastState *instantiate_fastState();
void print_fast(struct FastState *state);

struct GameState *instantiate_state();
void clear_cell(struct GameState *state, int x, int y);
void set_cell(struct GameState *state, int x, int y, int player);
struct GameState *copy(const struct GameState *state);
void copy_into(const struct GameState *src, struct GameState *dst);
void clear_state(struct GameState **state);
void print_field(const struct GameState *state);

#endif