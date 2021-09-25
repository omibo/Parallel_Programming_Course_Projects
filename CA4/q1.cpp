#include <omp.h> 
#include <iostream>
#include <assert.h>

#define arraySize 1048576
using namespace std; 

float* fillArray(){
    static float randomNumbers[arraySize];
    srand(time(NULL));
    for (int i = 0; i < arraySize; i++){
        randomNumbers[i] = (float) rand() / rand();
    }
    return randomNumbers;
}

void printResult(double serExecTime, double parExecTime, float serMaxVal, float parMaxVal, int serMaxIndex, int parMaxIndex){
    cout << "Serial Result" << endl;
    cout << "maxVal=" << serMaxVal << "  index=" << serMaxIndex << endl;
    cout << "Parallel Result" << endl;
    cout << "maxVal=" << parMaxVal << "  index=" << parMaxIndex << endl << endl;
    cout << "Serial Exec Time=" << serExecTime << endl;
    cout << "Parallel Exec Time=" << parExecTime << endl;
    cout << "Speedup=" << serExecTime / parExecTime << endl << endl;
}

void checkResults(float numbers[], int serMaxIndex, float serMaxVal, int parMaxIndex, float parMaxVal){
    assert(parMaxVal == serMaxVal);
    assert(numbers[parMaxIndex] == parMaxVal);
    assert(numbers[serMaxIndex] == serMaxVal);
    for(int i=0; i<arraySize; i++)
        assert(numbers[i] <= serMaxVal);
}

double findMaxSerial(float nums[], int &maxIndex, float &maxVal){
	double serialStart = omp_get_wtime();
    maxVal = -1.0/0.0;
    for (int i = 0; i < arraySize; i += 1){
        if (nums[i] >= maxVal){
            maxVal = nums[i];
            maxIndex = i;
        }
    }
    return omp_get_wtime() - serialStart;
}

double findMaxParallel(float numbers[], int &maxIndex, float &maxVal, int threadNums){
	double parallelStart = omp_get_wtime();
    maxVal = -1.0/0.0;
	float thrMaxNum = -1.0/0.0;
    int thrMaxIndex;

	#pragma omp parallel num_threads(threadNums) shared(numbers)
	{
		#pragma omp for nowait
		{
			for (int i = 0; i < arraySize; i++){
				if (numbers[i] >= thrMaxNum){
					thrMaxNum = numbers[i];
					thrMaxIndex = i;
				}
			}
		}
        if(thrMaxNum > maxVal){
            maxVal = thrMaxNum;
            maxIndex = thrMaxIndex;
        }
	}
    return omp_get_wtime() - parallelStart;
}

int main(){
    cout << "Mobina Shahbandeh  --  810196488\tOmid Bodaghi  --  810196423" << endl << endl;
    float* numbers = fillArray();

    int physicalCores = omp_get_num_procs() / 2;

    float serMaxVal, parMaxVal;
    int serMaxIndex, parMaxIndex;
    
    double serExecTime = findMaxSerial(numbers, serMaxIndex, serMaxVal);
    double parExecTime = findMaxParallel(numbers, parMaxIndex, parMaxVal, physicalCores);

    checkResults(numbers, serMaxIndex, serMaxVal, parMaxIndex, parMaxVal);
    printResult(serExecTime, parExecTime, serMaxVal, parMaxVal, serMaxIndex, parMaxIndex);

    return 1;
}

