#include <iostream>
#include <fstream>
#include <sstream>
#include <Windows.h>

using namespace std;

#define Pageprocze 4096 // Страница = размер оперативной памяти страницы
#define PageCount 15 // 930705 = 3 + 0 + 7 + 5 = 15
#define SumCount 17

int main() {
	
	HANDLE FileHand = CreateFile("fileMapping", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE MappHand = CreateFileMapping(FileHand, NULL, PAGE_READWRITE, 0, 0, "MappedFile");
	
	HANDLE writeSemaphore = CreateSemaphore(NULL, PageCount, PageCount, "SemForWrite");
	HANDLE readSemaphore = CreateSemaphore(NULL, 0, PageCount, "SemForRead");
	HANDLE loggerMutex = CreateMutex(NULL, FALSE, "MutexForLog");
	
	stringstream NameX;
	
	if(MappHand == INVALID_HANDLE_VALUE) cout << GetLastError() << endl;
	else{
		if (FileHand != INVALID_HANDLE_VALUE) {
			HANDLE arrMX[PageCount];
			
			for (int i = 0; i < PageCount; i++) {
				NameX << "Mutex_" << i;
				arrMX[i] = CreateMutex(NULL, FALSE, NameX.str().c_str());
			}

			HANDLE hChild[2 * SumCount];
			HANDLE hThreadC[2 * SumCount];

			fstream logFile;
			logFile.open("lab.log", fstream::app | fstream::out);
			logFile.flush();
			logFile.close();

			for (int i = 0; i < SumCount; i++) {
				STARTUPINFOA proc;
				proc.cb = sizeof(STARTUPINFOA);
				PROCESS_INFORMATION pi;
				if (CreateProcess("lab4_1_Write.exe", NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &proc, &pi)) cout << "Start Write_" << i << endl;
				hChild[i] = pi.hProcess;
				hThreadC[i] = pi.hThread;
			}

			for (int i = 0; i < SumCount; i++) {
				STARTUPINFOA proc;
				proc.cb = sizeof(STARTUPINFOA);
				PROCESS_INFORMATION pi;
				if (CreateProcess("lab4_1_Read.exe", NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &proc, &pi)) cout << "Start Read_" << i << endl;
				hChild[SumCount+i] = pi.hProcess;
				hThreadC[SumCount+i] = pi.hThread;
			}

			WaitForMultipleObjects(2 * SumCount, hChild, TRUE, INFINITE);

			cout << "Ready" << endl;
			cin.get();
			
			for (int i = SumCount; i < 2 * SumCount; i++) {
				CloseHandle(hThreadC[i]);
				CloseHandle(hChild[i]);
			}

			for (int i = 0; i < PageCount; i++) {
				CloseHandle(arrMX[i]);
			}

			CloseHandle(writeSemaphore);
			CloseHandle(readSemaphore);
			CloseHandle(MappHand);
		}
		CloseHandle(FileHand);
	}

	return 0;
}