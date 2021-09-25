#include <sys/time.h> 
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "x86intrin.h"

#define arraySize 1048576

struct timeval serialStart, serialEnd, parallelStart, parallelEnd;

float randomNumbers[arraySize];

void fillArray(){
    srand(time(NULL));
    for (int i = 0; i < arraySize; i++){
        randomNumbers[i] = (float)rand();
    }
}

long findSerial(){
    gettimeofday(&serialStart, NULL);

    float sumResult[4] = {0.0, 0.0, 0.0, 0.0};
    for (int i=0; i<arraySize; i+=4){
        sumResult[0] += randomNumbers[i];
    }
    for (int i=1; i<arraySize; i+=4){
        sumResult[1] += randomNumbers[i];
    }
    for (int i=2; i<arraySize; i+=4){
        sumResult[2] += randomNumbers[i];
    }
    for (int i=3; i<arraySize; i+=4){
        sumResult[3] += randomNumbers[i];
    }

    float avg = ((sumResult[0] + sumResult[1]) + (sumResult[2] + sumResult[3])) / arraySize;

    float sdResult[4] = {0.0, 0.0, 0.0, 0.0};
    for (int i=0; i<arraySize; i+=4){
        sdResult[0] += (randomNumbers[i] - avg) * (randomNumbers[i] - avg);
    }
    for (int i=1; i<arraySize; i+=4){
        sdResult[1] += (randomNumbers[i] - avg) * (randomNumbers[i] - avg);
    }
    for (int i=2; i<arraySize; i+=4){
        sdResult[2] += (randomNumbers[i] - avg) * (randomNumbers[i] - avg);
    }
    for (int i=3; i<arraySize; i+=4){
        sdResult[3] += (randomNumbers[i] - avg) * (randomNumbers[i] - avg);
    }

    float sd = sqrt(((sdResult[0] + sdResult[1]) + (sdResult[2] + sdResult[3])) / arraySize);

    gettimeofday(&serialEnd, NULL);

    printf("Serial Results:\n");
    printf("avg: %f  sd: %f  \n", avg, sd);

    long seconds = (serialEnd.tv_sec - serialStart.tv_sec);
    long micros = ((seconds * 1000000) + serialEnd.tv_usec) - (serialStart.tv_usec);
    printf("Execution time is %ld s and %ld micros\n\n", seconds, micros);
    return seconds * 1000000 + micros;
}

long findParallel(){
    gettimeofday(&parallelStart, NULL);

    __m128 vec;
    __m128 avg = _mm_set1_ps(0.0f);
    __m128 size = _mm_set1_ps(float(arraySize));


    for (long i=0; i < arraySize; i += 4){
        vec = _mm_loadu_ps(&randomNumbers[i]);
        avg = _mm_add_ps(avg, vec);
    }
    avg = _mm_hadd_ps(avg, avg);
    avg = _mm_hadd_ps(avg, avg);
    avg = _mm_div_ps(avg, size);

    __m128 sd = _mm_set1_ps(0.0f);
    for (long i=0; i < arraySize; i += 4){
        vec = _mm_loadu_ps(&randomNumbers[i]);
        vec = _mm_sub_ps(vec, avg);
        vec = _mm_mul_ps(vec, vec);
        sd = _mm_add_ps(sd, vec);
    }
    sd = _mm_hadd_ps(sd, sd);
    sd = _mm_hadd_ps(sd, sd);
    sd = _mm_div_ps(sd, size);
    sd = _mm_sqrt_ps(sd);

    float avgNum = _mm_cvtss_f32(avg); 
    float sdNum = _mm_cvtss_f32(sd); 


    gettimeofday(&parallelEnd, NULL);

    printf("Parallel Results:\n");
    printf("avg: %f  sd: %f  \n", avgNum, sdNum);

    long seconds = (parallelEnd.tv_sec - parallelStart.tv_sec);
    long micros = ((seconds * 1000000) + parallelEnd.tv_usec) - (parallelStart.tv_usec);
    printf("Execution time is %ld s and %ld micros\n\n", seconds, micros);
    return seconds * 1000000 + micros;
}

int main(){
    printf("Mobina Shahbandeh  --  810196488\tOmid Bodaghi  --  810196423\n\n");
    fillArray();
    
    long serialExecTime = findSerial();
    printf("\n");
    long parallelExecTime = findParallel();

    printf("speed up = %f\n\n", float(serialExecTime) / parallelExecTime);
    return 1;
}