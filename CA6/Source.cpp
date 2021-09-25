#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

#define N 14
#define NUM_THREADS 8

int solutions;

int put(int Queens[], int row, int column)
{
	int i;

	for (i = 0; i < row; i++) {
		if (Queens[i] == column || abs(Queens[i] - column) == (row - i))
			return -1;
	}

	Queens[row] = column;
	if (row == N - 1) {
	#pragma omp atomic
		solutions++;
	}
	else {
		for (i = 0; i<N; i++) { // increment row
			put(Queens, row + 1, i);
		}
	}
	return 0;
}


void solveSerial(int Queens[]) {
	Queens = new int[N];
	for (int i = 0; i<N; i++) {
		put(Queens, 0, i);
	}
	delete[] Queens;
}

void solveParallel(int Queens[]) {
	int i;
	#pragma omp parallel num_threads(NUM_THREADS) private(Queens)
	{
		Queens = new int[N];
		#pragma omp for private(i) schedule(dynamic, 1)
		for (i = 0; i<N; i++) {
			put(Queens, 0, i);
		}
		delete[] Queens;
	}
}



int main()
{
	int Queens[N];
	double parStart = 0, parFin = 0, serStart = 0, serFin = 0;

	parStart = omp_get_wtime();
	solveParallel(Queens);
	parFin = omp_get_wtime();
	int parSolutions = solutions;
	solutions = 0;

	serStart = omp_get_wtime();
	solveSerial(Queens);
	serFin = omp_get_wtime();
	int serSolutions = solutions;

	printf("Omid Bodaghi -- 810196423\tMobina Shahbandeh -- 810196488\n");
	printf("N: %d\nnum threads: %d\n# solutions %d  serial time: %f  parallel time: %f \n",
		N, NUM_THREADS, solutions, serFin - serStart, parFin - parStart);
	printf("speedup = %f\n", (serFin - serStart) / (parFin - parStart));
	// getchar();
	return 0;
}