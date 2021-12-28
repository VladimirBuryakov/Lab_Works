#include <windows.h>
#include <iostream>

using namespace std;


void WINAPI Callback(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) {
	cout << "Info Accept!" << endl;
}

int main()
{

    system("chcp 1251");
    system("cls");
    
    BOOL   Connect = false;
    char  Buf[512];
	HANDLE Pipe = CreateFile((LPCSTR)("\\\\.\\pipe\\Volodya"), GENERIC_READ, 0, NULL,	OPEN_EXISTING,	FILE_FLAG_OVERLAPPED,	NULL);
	

	if ( Pipe != INVALID_HANDLE_VALUE )
	{
            OVERLAPPED overlapped;
            cout << "Connect Server" << endl;
            cout << "Accept the message:" << endl;
            while(strcmp(Buf, "0") != 0){
                ZeroMemory(Buf, 512);
                Connect = ReadFileEx(Pipe, Buf, 512, &overlapped, Callback);
                SleepEx(INFINITE, true);
                overlapped.Offset = 0;
                overlapped.OffsetHigh = 0; 
                cout << "Wait" << endl;
                cout << Buf << endl;
            }
            
            Connect = CloseHandle(Pipe);
            if (Connect) cout << "Connect Succesfull Closed!" << endl;
            else cout << "Not Connected!" << endl;
            
            Connect = FALSE;
            system("pause");

	}
	else cout << "Error!" << endl;

	return 0;
}