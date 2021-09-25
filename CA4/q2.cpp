#include <omp.h> 
#include <iostream>
#include <assert.h>

#define arraySize 1048576
#define CUTOFF 100
using namespace std; 

float* fillArray(){
    static float randomNumbers[arraySize];
    srand(time(NULL));
    for (int i = 0; i < arraySize; i++){
        randomNumbers[i] = rand();
    }
    return randomNumbers;
}

void printResult(double serExecTime, double parExecTime){
    cout << "Serial Exec Time=" << serExecTime << endl;
    cout << "Parallel Exec Time=" << parExecTime << endl;
    cout << "Speedup=" << serExecTime / parExecTime << endl << endl;
}

float* deepcopy(float data[]){
    static float newData[arraySize];
    srand(time(NULL));
    for (int i = 0; i < arraySize; i++){
        newData[i] = data[i];
    }
    return newData;
}

void checkResults(float parSorted[], float serSorted[]){
    for(int i=0; i<arraySize-1; i++){
        assert(parSorted[i] == serSorted[i]);
        assert(parSorted[i] <= parSorted[i+1]);
        assert(serSorted[i] <= serSorted[i+1]);
    }
    assert(parSorted[arraySize-1] == serSorted[arraySize-1]);
}

void swap(float *a, float *b) {
    float t = *a;
    *a = *b;
    *b = t;
}

int partition(float data[], int low, int high) {
    float pivot = data[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (data[j] <= pivot) {
            i++;
            swap(&data[i], &data[j]);
        }
    }
    swap(&data[i + 1], &data[high]);
    return i + 1;
}

void sortSerial(float data[], int low, int high) {
    if (low < high) {
        int pi = partition(data, low, high);
        sortSerial(data, low, pi - 1);
        sortSerial(data, pi + 1, high);
    }
}

void sortParallelUtil(float data[], int low, int high) {
    if (low < high) {
        int pi = partition(data, low, high);
        #pragma omp task shared(data) final(pi-1-low<CUTOFF)
            sortParallelUtil(data, low, pi - 1);
        #pragma omp task shared(data) final(high-pi-1<CUTOFF)
            sortParallelUtil(data, pi + 1, high);
        #pragma omp taskwait
    }
}

void sortParallel(float data[], int threadNums){
    omp_set_num_threads(threadNums);
    #pragma omp parallel num_threads(threadNums)
    {
        #pragma omp single
        {
            sortParallelUtil(data, 0, arraySize-1);
        }
    }
}

int main(){
    cout << "Mobina Shahbandeh  --  810196488\tOmid Bodaghi  --  810196423" << endl << endl;
    float* numbers1 = fillArray();
    float* numbers2 = deepcopy(numbers1);

    int physicalCores = omp_get_num_procs() / 2;

    double start = omp_get_wtime();
    sortSerial(numbers1, 0, arraySize-1);
    double serExecTime = omp_get_wtime() - start;

    start = omp_get_wtime();
    sortParallel(numbers2, physicalCores);
    double parExecTime = omp_get_wtime() - start;

    checkResults(numbers1, numbers2);
    printResult(serExecTime, parExecTime);

    return 1;
}