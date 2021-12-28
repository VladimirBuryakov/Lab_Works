#include <windows.h>
#include <string>
#include <stdio.h>
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
    system("chcp 1251");
    system("cls");
    
    BOOL   Connect = false;
    char  Buf[512];
    
    HANDLE HServer;
    DWORD Error = GetLastError();
    
    HServer = CreateNamedPipe("\\\\.\\pipe\\Volodya",PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED | WRITE_DAC, PIPE_TYPE_MESSAGE | PIPE_WAIT, 1, 0, 0, 0, NULL);
    
    if (HServer!=INVALID_HANDLE_VALUE)
    {
            cout << "Create Pipe:" << endl;
            
            cout << "Connect to name Chanel:" << endl;
            Connect = ConnectNamedPipe(HServer, NULL);
            //WaitForSingleObject(Event, INFINITE);
            if(!Connect){
                switch(Error){
                        case ERROR_NO_DATA:
                            cout << "Connect named Pipe: Error no data" << endl;
                            CloseHandle(HServer);
                            break;
                        case ERROR_PIPE_CONNECTED:
                            cout << "Connect named Pipe: Error Pipe Connect" << endl;
                            CloseHandle(HServer);
                            break;
                        case ERROR_PIPE_LISTENING:
                            cout << "Connect named Pipe: Error Pipe Listening" << endl;
                            CloseHandle(HServer);
                            break;
                        case ERROR_CALL_NOT_IMPLEMENTED:
                            cout << "Connect named Pipe: Error Call not implemented" << endl;
                            CloseHandle(HServer);
                            break;
                        default:
                            cout << "Connect named Pipe: Error" << endl;
                            CloseHandle(HServer);
                            break;
                }
            }
            else cout << "Connected Complete!" << endl;
            
            cout << "Write message(Please)" << endl;
            if(Connect == FALSE) cout << "Error Connected!" << endl;
            while(true){
                cout << "Input text or input 0 to Exit: " << endl;
                cin >> Buf;
                OVERLAPPED Event;
                Event.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
                WriteFile(HServer, (LPCVOID)Buf, 512, NULL, &Event); 
                WaitForSingleObject(Event.hEvent, INFINITE);
                if(strcmp(Buf, "0") == 0) break;
            }
            
            Connect = DisconnectNamedPipe(HServer);
                        
            if (Connect)
                cout << "Disconnected!" << endl;
            else cout << "Error Disconnect!" << endl;
            Connect = FALSE;    
            system("pause");
            
            CloseHandle(HServer);
    }   
    else cout << "Error! Restart Programm" << endl;
    return 0;
}
