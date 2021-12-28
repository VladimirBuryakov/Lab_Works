#include <iostream>
#include <sstream>
#include <fstream>
#include <Windows.h>
#include <random>
#include <ctime>
#include <timeapi.h>

#pragma comment (lib, "winmm.lib")

using namespace std;

#define Pageprocze 4096 // Страница = размер оперативной памяти страницы
#define PageCount 15 // 930705 = 3 + 0 + 7 + 5 = 15

int main() {
	HANDLE hMapped = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "MappedFile"); 
	fstream logFile;
	logFile.open("lab.log", fstream::app | fstream::out);
	logFile.rdbuf()->pubsetbuf(NULL, 0);
	int ProccessId;
	int index;
	unsigned int delay;
	HANDLE writeSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "SemForWrite");
	HANDLE readSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "SemForRead");
	HANDLE loggerMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "MutexForLog");
	stringstream name;
	
	if(hMapped == INVALID_HANDLE_VALUE) cout << GetLastError() << endl;
	else{
		void* address = MapViewOfFile(hMapped, FILE_MAP_READ, 0, 0, 0);
		HANDLE arrMutex[PageCount];
		
		for (int i = 0; i < PageCount; i++) {
			name << "Mutex_" << i;
			arrMutex[i] = OpenMutex(MUTEX_ALL_ACCESS, FALSE, name.str().c_str());
		}
		
		ProccessId = GetCurrentProcessId();
		WaitForSingleObject(loggerMutex, INFINITE);
		logFile << "W\t" << ProccessId << "\tStart\t" << timeGetTime() << endl;
		logFile.flush();
		ReleaseMutex(loggerMutex);
		
		VirtualLock(address, Pageprocze * PageCount);

		WaitForSingleObject(loggerMutex, INFINITE);
		logFile << "W\t" << ProccessId << "\tWait\t" << timeGetTime() << endl;
		logFile.flush();
		ReleaseMutex(loggerMutex);
		WaitForSingleObject(writeSemaphore, INFINITE);

		index = WaitForMultipleObjects(PageCount, arrMutex, FALSE, INFINITE);
		WaitForSingleObject(loggerMutex, INFINITE);
		logFile << "W\t" << ProccessId << "\tPage_" << index << "\t" << timeGetTime() << endl;
		logFile.flush();
		ReleaseMutex(loggerMutex);

		delay = (rand() % 1000) + 500;
		Sleep((DWORD)delay);
		WaitForSingleObject(loggerMutex, INFINITE);
		logFile << "W\t" << ProccessId << "\tReady\t" << timeGetTime() << endl;
		logFile.flush();
		ReleaseMutex(loggerMutex);
		ReleaseMutex(arrMutex[index]);
		ReleaseSemaphore(readSemaphore, 1, NULL);

		VirtualUnlock(address, Pageprocze * PageCount);
		
		for (int i = 0; i < PageCount; i++) {
			CloseHandle(arrMutex[i]);
		}
		
		WaitForSingleObject(loggerMutex, INFINITE);
		logFile << "W\t" << ProccessId << "\tFinish\t" << timeGetTime() << endl;
		logFile.flush();
		ReleaseMutex(loggerMutex);

		CloseHandle(loggerMutex);
		UnmapViewOfFile(address);
		CloseHandle(hMapped);
		CloseHandle(writeSemaphore);
		CloseHandle(readSemaphore);
		logFile.close();
	}
	return 0;
}