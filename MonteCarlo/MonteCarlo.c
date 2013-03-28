#include <stdio.h> 
#include <stdlib.h> 
#include <time.h> 

#define N 50001

int main(void) { 
	srand(time(NULL)); 
	int sum = 0;
	int i;
	int j = 0;

	while (j < 1000) {
		for(i = 1; i < N; i++) { 
			double x = (double) rand() / RAND_MAX; 
			double y = (double) rand() / RAND_MAX; 
			if((x * x + y * y) < 1)
			    sum++; 
		} 
		j++;
	}
	printf("PI = %f\n", (double) 4 * sum / (N - 1));
	return 0; 
}
