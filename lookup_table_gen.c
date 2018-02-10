#include <stdio.h>

int main()
{
	printf("int table[19683][10] = {");
	for (int i = 0; i<19683; i++){
		int field[9];
		int index = i;
		int adj1 = 0;
		int adj2 = 0;
		for (int y=2; y>=0; y--){
			for (int x=2; x>=0; x--){
				int val = index % 3;
				index /= 3;
				field[x + 3*y] = val;
				if (val == 1)
					adj1++;
				else if(val == 2)
					adj2++;
			}
		}
		int result = field[4];
		if (result == 1)
			adj1--;
		else if (result == 2)
			adj2--;
		int adj = adj1+adj2;
		if (field[4] == 0){
			if (adj == 3){
				if (adj1 > adj2)
					result = 1;
				else
					result = 2;
			}
		}else{
			if (adj < 2 || adj > 3){
				result = 0;
			}
		}
		int mul = 1;
		printf("{");
		for (int j=0; j<9; j++){
			printf("%d,",result*mul);
			mul *= 3;
		}
		if (field[4] == 0){
			printf("%d}", result);
		}else{
			if (result == 0){
				printf("-%d}", field[4]);
			}else{
				if (result != field[4])
					while(1);
				else{
					printf("0}");
				}
			}
		}
		if (i < 19682)
			printf(",\n");
	}
	printf("};\n");
}