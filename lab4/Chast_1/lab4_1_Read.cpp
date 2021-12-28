#include <iostream>
#include <sstream>
#include <fstream>
#include <Windows.h>
#include <random>
#include <timeapi.h>

#pragma comment (lib, "winmm.lib")

using namespace std;

#define Pageprocze 4096 // Страница = размер оперативной памяти страницы
#define PageCount 15 // 930705 = 3 + 0 + 7 + 5 = 15


int main() {
	HANDLE hMapped = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "MappedFile");
	fstream logFile;	
	logFile.open("lab.log", fstream::app | fstream::out);
	int ProccessId;
	int index;
	unsigned int delay;
	HANDLE writeSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "SemForWrite");
	HANDLE readSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "SemForRead");
	HANDLE logFileMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "MutexForLog");
	stringstream NameX;
	
	if(hMapped == INVALID_HANDLE_VALUE) cout << GetLastError() << endl;
	else{
		void* address = MapViewOfFile(hMapped, FILE_MAP_READ, 0, 0, 0);
		HANDLE arrMutex[PageCount];
		
		for (int i = 0; i < PageCount; i++) {
			NameX << "Mutex_" << i;
			arrMutex[i] = OpenMutex(MUTEX_ALL_ACCESS, FALSE, NameX.str().c_str());
		}
		
		ProccessId = GetCurrentProcessId();
		WaitForSingleObject(logFileMutex, INFINITE);
		logFile << "R\t" << ProccessId << "\tStart\t" << timeGetTime() << endl;
		logFile.flush();
		ReleaseMutex(logFileMutex);
		
		VirtualLock(address, Pageprocze * PageCount);
		WaitForSingleObject(logFileMutex, INFINITE);
		
		logFile << "R\t" << ProccessId << "\tWait\t" << timeGetTime() << endl;
		logFile.flush();
		ReleaseMutex(logFileMutex);
		WaitForSingleObject(readSemaphore, INFINITE);
		
		index = WaitForMultipleObjects(PageCount, arrMutex, FALSE, INFINITE);
		WaitForSingleObject(logFileMutex, INFINITE);
		logFile << "R\t" << ProccessId << "\tPage_" << index << "\t" << timeGetTime() << endl;
		logFile.flush();
		ReleaseMutex(logFileMutex);
		
		delay = (rand() % 1000) + 500;
		Sleep((DWORD)delay);
		WaitForSingleObject(logFileMutex, INFINITE);
		logFile << "R\t" << ProccessId << "\tReady\t" << timeGetTime() << endl;
		logFile.flush();
		ReleaseMutex(logFileMutex);
		ReleaseMutex(arrMutex[index]);
		ReleaseSemaphore(writeSemaphore, 1, NULL);
		
		VirtualUnlock(address, Pageprocze * PageCount);
		
		for (int i = 0; i < PageCount; i++) {
			CloseHandle(arrMutex[i]);
		}
		
		WaitForSingleObject(logFileMutex, INFINITE);
		logFile << "R\t" << ProccessId << "\tFinish\t" << timeGetTime() << std::endl;
		logFile.flush();
		ReleaseMutex(logFileMutex);

		UnmapViewOfFile(address);
		CloseHandle(hMapped);
		CloseHandle(writeSemaphore);
		CloseHandle(readSemaphore);
		logFile.close();
	}
	return 0;
}