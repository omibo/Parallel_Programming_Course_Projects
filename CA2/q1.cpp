#include <sys/time.h> 
#include <stdio.h>
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

long findMaxSerial(){
    gettimeofday(&serialStart, NULL);

    float maxNum = -1.0/0.0;
    int maxIndex;
    for (long i = 0; i < arraySize; i += 1){
        float temp = randomNumbers[i];
        if (temp >= maxNum){
            maxNum = temp;
            maxIndex = i;
        }
    }
    gettimeofday(&serialEnd, NULL);
    printf("Serial Results:\n");
    printf("maxNum=%f  index=%d\n", maxNum, maxIndex);

    long seconds = (serialEnd.tv_sec - serialStart.tv_sec);
    long micros = ((seconds * 1000000) + serialEnd.tv_usec) - (serialStart.tv_usec);
    printf("Execution time is %ld s and %ld micros\n\n", seconds, micros);
    return seconds * 1000000 + micros;
}

__m128 horizontalExtermum(const __m128 &p){   
    // http://www.awsom3d.com/2014/06/sse-horizontal-minimum-and-maximum.html
    __m128 data = p;
    for (int i=0; i<3; i++){
        data = _mm_max_ps(data, _mm_shuffle_ps(data, data, 0b10010011));
    }  
    return data;
    // https://shybovycha.github.io/2017/02/21/speeding-up-algorithms-with-sse.html
 } 

__m128 updateIndex(__m128 cond, __m128 a, __m128 b){
    __m128 temp1 = _mm_and_ps(cond, b);
    __m128 temp2 = _mm_andnot_ps(cond, a);
    return _mm_or_ps(temp1, temp2);
}

long findMaxParallel(){
    gettimeofday(&parallelStart, NULL);

    __m128 index = _mm_set_ps(3.0, 2.0, 1.0, 0.0);
    __m128 maxValHolder = _mm_loadu_ps(&randomNumbers[0]);
    __m128 maxIndexHolder = index;
    __m128 incr = _mm_set1_ps(4.0);

    for (long i=4; i < arraySize; i += 4){
        index = _mm_add_ps(index, incr);
        __m128 vec = _mm_loadu_ps(&randomNumbers[i]);

        __m128 isGreater = _mm_cmpge_ps(vec, maxValHolder);  // ( vec >= maxValHolder ) ? 0xFFFFFFFF : 0

        // maxIndexHolder = updateIndex(isGreater, maxIndexHolder, index);

        maxIndexHolder = _mm_blendv_ps(maxIndexHolder, index, isGreater);

        maxValHolder = _mm_max_ps(maxValHolder, vec);
    }

    __m128 maxNum = horizontalExtermum(maxValHolder);
    __m128 isEqual = _mm_cmpeq_ps(maxValHolder, maxNum);
    __m128 maxNumIndex = horizontalExtermum(_mm_and_ps(maxIndexHolder, isEqual));

    float resultMax = _mm_cvtss_f32(maxNum); 
    float resultIndex = _mm_cvtss_f32(maxNumIndex); 

    gettimeofday(&parallelEnd, NULL);

    printf("Parallel Results:\n");
    printf("maxNum=%f  index=%d\n", resultMax, int(resultIndex));

    long seconds = (parallelEnd.tv_sec - parallelStart.tv_sec);
    long micros = ((seconds * 1000000) + parallelEnd.tv_usec) - (parallelStart.tv_usec);
    printf("Execution time is %ld s and %ld micros\n\n", seconds, micros);
    return seconds * 1000000 + micros;
}

int main(){
    printf("Mobina Shahbandeh  --  810196488\tOmid Bodaghi  --  810196423\n\n");
    fillArray();
    
    long serialExecTime = findMaxSerial();
    printf("\n");
    long parallelExecTime = findMaxParallel();

    printf("speed up = %f\n\n", float(serialExecTime) / parallelExecTime);
    return 1;
}