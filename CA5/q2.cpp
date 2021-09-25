#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/time.h>
#include <cstring>
#include <cassert>

#define ARR_SIZE 1048576
#define THRESH_ARR_SIZE 50000

char ColorCodes[6][250] = {
	"\033[1m\033[33m",		// Bold YELLOW
	"\033[1m\033[31m",		// Bold RED
	"\033[1m\033[32m",		// Bold GREEN
	"\033[1m\033[34m",		// Bold BLUE
  "\033[1m\033[35m",    // Bold VIOLET
	"\x1B[0m"				      // Reset
};

typedef struct
{
  float* arr;
  int p;
  int q;
} threadParams;

double timeGetTime()
{
  struct timeval time;
  struct timezone zone;
  gettimeofday(&time, &zone);
  return time.tv_sec + time.tv_usec*1e-6;
}

float* getRandomArray()
{
  static float randomNumbers[ARR_SIZE];
  srand(time(NULL));
  for (int i = 0; i < ARR_SIZE; i++){
    randomNumbers[i] = (float) rand() / rand();
  }
  return randomNumbers;
}

void swap(float* a, float* b)
{
	float t = *a;
	*a = *b;
	*b = t;
}

int partition(float arr[], int p, int q)
{
  float pivot = arr[q];
  int i = p - 1;
  for (int j = p; j < q; j++) {
      if (arr[j] <= pivot) {
          i++;
          swap(&arr[i], &arr[j]);
      }
  }
  swap(&arr[i + 1], &arr[q]);
  return i + 1;
}

void quickSort(float arr[], int p, int q)
{
  if (p < q) {
    int pi = partition(arr, p, q);
    quickSort(arr, p, pi - 1);
    quickSort(arr, pi + 1, q);
  }
}

void parallelQuickSort(float *arr, int p, int q);

void* threadQuickSort(void * initialValues)
{
  threadParams* params = (threadParams*)initialValues;
  parallelQuickSort(params->arr, params->p, params->q);
  return NULL;
}

void parallelQuickSort(float *arr, int p, int q)
{
  if (p < q) {
    int pivotPos = partition(arr, p, q);
    pthread_t thread;
    if ((q - p) > THRESH_ARR_SIZE) {
      threadParams threadParam = {arr, p, pivotPos - 1};
      int res = pthread_create(&thread, NULL, threadQuickSort, &threadParam);
      parallelQuickSort(arr, pivotPos + 1, q);
      pthread_join(thread, NULL);
    } else {
      quickSort(arr, p, pivotPos-1);
      quickSort(arr, pivotPos + 1, q);
    }
  }
}

float* deepcopy(float data[])
{
  static float newData[ARR_SIZE];
  for (int i = 0; i < ARR_SIZE; i++){
    newData[i] = data[i];
  }
  return newData;
}

void checkResults(float* arr, float* arrCpy) {
  for (int i = 0; i < ARR_SIZE; i++) {
    if (i > 0)
      assert(arr[i] >= arr[i-1]);

    assert(arr[i] == arrCpy[i]);
  }
}

void run() {
  double serialStartTime, serialExecTime, parallelStartTime, parallelExecTime;

  float* arr = getRandomArray();
  float* arrCpy = deepcopy(arr);

  printf("%sOmid Bodaghi -- 810196423\tMobina Shahbandeh -- 810196488%s\n\n",
        ColorCodes[4],
        ColorCodes[5]);

  serialStartTime = timeGetTime();
  quickSort(arrCpy, 0, ARR_SIZE - 1);
  serialExecTime = timeGetTime() - serialStartTime;

  printf("%sSerial execution time: %lf secs\n",
        ColorCodes[2],
        serialExecTime);

  parallelStartTime = timeGetTime();
  parallelQuickSort(arr, 0, ARR_SIZE - 1);
  parallelExecTime = timeGetTime() - parallelStartTime;

  printf("%sParallel execution time: %lf secs\n",
        ColorCodes[3],
        parallelExecTime);

  printf("%sSpeedup: %lf %s\n\n",
        ColorCodes[0],
        serialExecTime / parallelExecTime,
        ColorCodes[5]);

  checkResults(arr, arrCpy);

}

int main(void)
{
  run();
  pthread_exit(0);
  return 0;
}