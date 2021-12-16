#include <iostream>
#include <windows.h>

using namespace std;

#define N 100000000
#define BlockSize 9307050
double InvertedN = (1.0/N);
#define ThreadCount 16
DWORD OperationSumm = 0;

typedef struct {
    DWORD counter;
    double pi;
    HANDLE Thread;
} SyncStruct;

DWORD WINAPI Sync(LPVOID parametr){
    SyncStruct* ThreadBlock = (SyncStruct*) parametr;
    ThreadBlock->counter = OperationSumm;
    OperationSumm += BlockSize;
    double sum_operations = 0;
    double x;
    DWORD SizeProgression;
    while(1){
        if(ThreadBlock->counter + BlockSize < N) 
            SizeProgression = BlockSize + ThreadBlock->counter ;
        else 
            SizeProgression = N;
        for(DWORD i = ThreadBlock->counter; i < SizeProgression ; i++){
            x = ( i + 0.5) * InvertedN; 
            sum_operations += (1.0/(1.0 + x*x));
        }
        ThreadBlock->counter = OperationSumm;
        if(OperationSumm > N ){
            ThreadBlock->pi += sum_operations;
            TerminateThread(ThreadBlock->Thread, 0);
        }
        OperationSumm += BlockSize;
    }
}

int main()
{
    SyncStruct ThreadInformation[ThreadCount]={};
    HANDLE Threads[ThreadCount];
    DWORD ID[ThreadCount];
    DWORD start_time;
    DWORD end_time;
    double pi = 0;
    
    for(int k = 0; k<ThreadCount; k++){
        Threads[k] = CreateThread(NULL, 0, Sync, &ThreadInformation[k], CREATE_SUSPENDED, &ID[k]);
        ThreadInformation[k].Thread = Threads[k];
    }
    
    start_time = GetTickCount();
    
    for(int k = 0; k<ThreadCount; k++){
        ResumeThread(Threads[k]);
        if(NULL == Threads[k]) cout << "Error Create!" << endl;
    }    
    
    WaitForMultipleObjects(ThreadCount, Threads, TRUE, INFINITE);
    
    for(int k = 0; k<ThreadCount; k++){
        pi += ThreadInformation[k].pi;
    } 
    
    pi = 4*pi*InvertedN;
    end_time = GetTickCount();
    cout.precision(10);
    cout << "Pi: " << pi << endl;
    cout << "Time: " << (double)(end_time-start_time)/1000 << endl;
    
    return 0;
}