#ifndef GAMESTATE
#define GAMESTATE

struct GameState
{
	int timebank;
	int round;
	int your_botid;
	int **field;
	int **adj_count0;
	int **adj_count1;
};

#endif