#include <pthread.h>
#include <iostream>
#include <sys/time.h> 
#include <time.h>
#include <assert.h>

using namespace std; 

#define arraySize 1048576
#define physicalCores 6

typedef struct{
    int startIndex;
    int endIndex;
} parFunInput;

float serMaxVal = -1.0/0.0;
int serMaxIndex = -1;
float parMaxVal = -1.0/0.0;
int parMaxIndex = -1;

float randomNums[arraySize];

pthread_mutex_t lockVar;

void fillArray(){
    srand(time(NULL));
    for (int i = 0; i < arraySize; i++){
        randomNums[i] = (float) rand() / rand();
    }
}

double getTime()
{
	struct timeval time;
	struct timezone zone;
	gettimeofday(&time, &zone);
	return time.tv_sec + time.tv_usec*1e-6;
}

void printResult(double serExecTime, double parExecTime){
    cout << "Serial Result" << endl;
    cout << "maxVal=" << serMaxVal << "  index=" << serMaxIndex << endl;
    cout << "Parallel Result" << endl;
    cout << "maxVal=" << parMaxVal << "  index=" << parMaxIndex << endl << endl;
    cout << "Serial Exec Time=" << serExecTime << endl;
    cout << "Parallel Exec Time=" << parExecTime << endl;
    cout << "Speedup=" << serExecTime / parExecTime << endl << endl;
}

void checkResults(){
    assert(parMaxVal == serMaxVal);
    assert(randomNums[parMaxIndex] == parMaxVal);
    assert(randomNums[serMaxIndex] == serMaxVal);
    for(int i=0; i<arraySize; i++)
        assert(randomNums[i] <= serMaxVal);
}

double findMaxSerial(){
	double serialStart = getTime();
    for (int i = 0; i < arraySize; i += 1){
        if (randomNums[i] >= serMaxVal){
            serMaxVal = randomNums[i];
            serMaxIndex = i;
        }
    }
    return getTime() - serialStart;
}

void *findLocalMax(void *arg){
    parFunInput *input = (parFunInput*) arg;
    float locMaxVal = -1.0/0.0;
    int locMaxIndex = -1;
    for(int i=input->startIndex; i<input->endIndex; i++){
        if (randomNums[i] >= locMaxVal){
            locMaxVal = randomNums[i];
            locMaxIndex = i;
        }
    }
    pthread_mutex_lock(&lockVar);
    if (locMaxVal >= parMaxVal){
        parMaxVal = locMaxVal;
        parMaxIndex = locMaxIndex;
    }
    pthread_mutex_unlock(&lockVar);
    pthread_exit(NULL);
}

double findMaxParallel(int threadNums){

	double parallelStart = getTime();
    pthread_mutex_init(&lockVar, NULL);
    pthread_t thread[threadNums];
    parFunInput thrdsInputs[threadNums];
    for(int i=0; i<threadNums; i++){
        thrdsInputs[i].startIndex = i * arraySize / threadNums;
        thrdsInputs[i].endIndex = (i+1) * arraySize / threadNums;
    }

    for(int i=0; i<threadNums; i++)
        pthread_create(&thread[i], NULL, findLocalMax, (void *) &thrdsInputs[i]);
    
    for(int i=0; i<threadNums; i++)
        pthread_join(thread[i], NULL);

    pthread_mutex_destroy(&lockVar);
    double tempTime = getTime() - parallelStart;
    return tempTime;
}

int main(){
    cout << "Mobina Shahbandeh  --  810196488\tOmid Bodaghi  --  810196423" << endl << endl;
    fillArray();
    
    double serExecTime = findMaxSerial();
    double parExecTime = findMaxParallel(physicalCores);

    checkResults();
    printResult(serExecTime, parExecTime);

    return 1;
}

