#include <stdio.h>

int main()
{
	for (int i=0; i<43046721; i++){
		int index = i;
		int table[16];
		for (int y=0; y<4; y++){
			for (int x=0; x<4; x++){
				int val = index % 3;
				table[x+4*y] = val;
				index /= 3;
			}
		}
		int values[4];
		for (int y=2; y>=1; y--){
			for (int x=2;x>=1;x--){
				int neighbor1 = 0;
				int neighbor2 = 0;
				for (int Y=y-1; Y<=y+1; Y++){
					for (int X=x-1; X<=x+1; X++){
						if (Y!=y || X!=x){
							if (table[X+4*Y] == 1)
								neighbor1++;
							else if (table[X+4*Y] == 2)
								neighbor2++;
						}
					}
				}
				int adj = neighbor2+neighbor1;
				values[x-2+(y-2)*2] = 0;
				if (table[x+4*y] != 0){
					if (adj >= 2 && adj <= 3){
						values[x-2+(y-2)*2] = table[x+4*y];
					}
				}else{
					if (adj == 3){
						if (neighbor1 > neighbor2){
							values[x-2+(y-2)*2] = 1;
						}else{
							values[x-2+(y-2)*2] = 2;
						}
					}
				}
			}
		}
	}
	return 0;
}