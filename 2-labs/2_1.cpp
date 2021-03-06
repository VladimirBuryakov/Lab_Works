#include <iostream>
#include <windows.h>

using namespace std;

void Menu() {
    cout << "1 - Information Systems" << endl;
    cout << "2 - Information Memory" << endl;
    cout << "3 - Information Memory at a certain address" << endl;
    cout << "4 - Reservation Memory" << endl;
    cout << "5 - Input Data Memory" << endl;
    cout << "6 - Protect Memory" << endl;
    cout << "0 - Exit Program" << endl;
}

void SystemInformation(){
    SYSTEM_INFO Info;
    GetSystemInfo(&Info);
    if(Info.wProcessorArchitecture == 9){
        cout << "x64 (AMD or Intel)" << endl;
    }
    else if(Info.wProcessorArchitecture == 6){
        cout << "Intel Itanium Processor Family (IPF)" << endl;
    } 
    else if(Info.wProcessorArchitecture == 0){
        cout << "x86" << endl;
    }
    cout <<"OEM ID: " << Info.dwOemId << endl; 
    cout <<"Number of processors: " << Info.dwNumberOfProcessors << endl;
    cout <<"Page size: " << Info.dwPageSize << endl;
    cout <<"Processor type:" <<  Info.dwProcessorType << endl;
    cout <<"Minimum application address: " << Info.lpMinimumApplicationAddress << endl;
    cout <<"Maximum application address: " << Info.lpMaximumApplicationAddress << endl;
    cout <<"Active processor mask: " << Info.dwActiveProcessorMask << endl;
    cout <<"Granularity for the starting address where virtual memory can be allocated: " << Info.dwAllocationGranularity << endl;
    cout << "Architecture dependent processor level: " << Info.wProcessorLevel << endl;
    cout << "Architecture dependent processor revision: " << Info.wProcessorRevision <<endl;
}

void MemoryInformation(){
    MEMORYSTATUS state;
    state.dwLength = sizeof(state);
    
    GlobalMemoryStatus(&state);
    
    cout << "OZU: " << state.dwTotalPhys / (1024 * 1024) << " Mb" << endl;
    cout << "Sizeof  bytes: " << state.dwLength << endl;
    cout << "Procent Memory use: " << state.dwMemoryLoad << endl;
    cout << "The amount of physical memory currently available, in bytes: " << state.dwAvailPhys << endl;
    cout << "Bytes of paging file: " << state.dwTotalPageFile << endl;
    cout << "Free bytes of paging file: " << state.dwAvailPageFile << endl;
    cout << "User bytes of address space: " << state.dwTotalVirtual << endl;
    cout << "Free user bytes: " << state.dwAvailVirtual;
    
}

void InformationMemorySpec(){
    LPVOID adress;
    MEMORY_BASIC_INFORMATION lpBuffer;
    SIZE_T l;
    l = sizeof(lpBuffer);
    
    cout << "Input adress memory: " << endl;
    cin >> adress;
    
    l = VirtualQuery(adress, &lpBuffer, l);
    if (l != 0){
        cout << "Base Adress: " << lpBuffer.BaseAddress << endl; // ???????????????? ???? ???? ????????????????, ?????? ?? ???????????????? lpAddress, ???? ?????????????????????? ???? ???????????????????? ???????????????? ????????????, ???????????????? ?????????????? ????????????????
        cout << "AllocationBase: " << lpBuffer.AllocationBase << endl; // ???????????????????????????? ?????????????? ?????????? ??????????????, ?????????????????????? ?? ???????? ??????????, ?????????????????? ?? ?????????????????? lpAddress
        cout << "AllocationProtect: " << lpBuffer.AllocationProtect << endl; // ???????????????????????????? ?????????????? ????????????, ?????????????????????? ?????????????? ?????? ?????? ????????????????????????????
        cout << "RegionSize: " << lpBuffer.RegionSize << endl; // ???????????????? ?????????????????? ???????????? (?? ????????????) ???????????? ??????????????, ?????????????? ???????????????????? ?? ???????????????? ???????????? BaseAddress ?? ?????????? ???? ???? ???????????????? ????????????, ?????????????????? ?? ??????, ?????? ?? ????????????????, ?????????????????????????? ???? ????????????, ???????????????????? ?? ?????????????????? lpAddress
        cout << "Protect: " << lpBuffer.Protect << endl; //???????????????????????????? ?????????????? ???????????? (PAGE_*) ???????? ?????????????? ??????????????, ?????????????? ?????????? ???? ???? ???????????????? ????????????, ?????????????????? ?? ??????, ?????? ?? ????????????????, ??????????????-???????????? ???? ????????????, ???????????????????? ?? ?????????????????? lpAddress
        
        if( lpBuffer.State == MEM_COMMIT){
            cout << "Ficksation Page " << endl;
        }
        else if( lpBuffer.State == MEM_FREE ){
            cout << "Free Page .... " << endl;
        }
        else if( lpBuffer.State == MEM_RESERVE ){
            cout << "Rezerv Page" << endl;
        }
        
        cout << "Memory type: ";
        
        switch(lpBuffer.Type) {
            case MEM_PRIVATE:
                cout << "MEM_PRIVATE" << endl;
                break;
            case MEM_IMAGE:
                cout << "MEM_IMAGE" << endl;
                break;
            case MEM_MAPPED:
                cout << "MEM_MAPPED" << endl;
                break;
            default:
                cout << "NO TYPE" << endl;
                break;
        }
    }
    else{
        cout << "Error!" << endl;
    }

}

void reserveMemory() {
    PVOID address;
    SIZE_T dwSize=10;
    PVOID p;
    MEMORY_BASIC_INFORMATION lpBuffer;
    BOOL Free;
    
    cout << "Input Size: " << endl;
    cin >> dwSize;
    
    cout << "Input address ( 0 - Auto ), 0X000... : " << endl;
    cin >> address;
    
    p = VirtualAlloc(address, dwSize, MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if( p != 0 ) {
        cout << "Memory highlighted, adress: " << p << endl; // ???????????? ????????????????
    }
    else{
        cout << "Memory no highlighted" << endl;
        return;
    }
    
    dwSize = sizeof(lpBuffer);
    dwSize = VirtualQuery(p, &lpBuffer, dwSize);
    
    if( lpBuffer.State == MEM_COMMIT){
        cout << "Ficksation Page " << endl;
    }
    else if( lpBuffer.State == MEM_FREE ){
        cout << "Free Page .... " << endl;
    }
    else if( lpBuffer.State == MEM_RESERVE ){
        cout << "Rezerv Page" << endl;
    }
    
    system("pause");
    
    Free = VirtualFree(p, 0, MEM_RELEASE);
    if (Free != 0 ){
        cout << "Memory Cleaned!" << endl;
    }
    else{
        cout << "No Cleaned Memory!" << endl;
    }
}

void WriteDataCell() {
    PVOID address;
    SIZE_T dwSize=10;
    PVOID p;
    BOOL Free;
    
    cout << "Input address ( 0 - Auto ), 0X000... :  "<< endl;
    cin >> address;
    p = VirtualAlloc(address, dwSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if( p == 0 ) {
        cout << " Memory no highlighted! " << endl;
        return;
    }

    cout << "Adress: " << (int)(*((char *)p)) << endl;
    *((char *)p) = 'h';
    cout << "Adress: " << (int)(*((char *)p)) << endl;
    system("pause");
    
    Free = VirtualFree(p, 0, MEM_RELEASE);
    if (Free != 0 ){
        cout << "Memory Cleaned!" << endl;
    }
    else{
        cout << "No Cleaned Memory!" << endl;
    }
    
}

void Protect(){
    PVOID address;
    SIZE_T dwSize=10;
    PVOID p;
    BOOL Free;
    int choice;
    DWORD flag;
    DWORD old;
    
    cout << "Input address ( 0 - Auto ), 0X000... :  "<< endl;
    cin >> address;
    p = VirtualAlloc(address, dwSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if( p == 0 ) {
        cout << " Memory no highlighted! " << endl;
        return;
    }
    cout << "Adress: " << p << endl;
    
    cout << "Value: " << (int)(*((char *)p)) << endl;
    
    cout << "Input Choice Flag ( 1 - Root ): " << endl;
    cin >> choice;
    
    switch(choice){
        case 1:
            flag = PAGE_READONLY;
            break;
        default:
            flag = PAGE_EXECUTE_READWRITE;
            break;
    }
    
    if( VirtualProtect(p, dwSize, flag, &old) ) {
        cout << "Access Changed!" << endl;
    }
    else{
        cout << "Access not Changed!" << endl;
    }
    
    if( flag == PAGE_READONLY ){
        cout << "No data, No Root!" << endl;
    }
    else{
        *((char *)p) = 100;
        cout << "Value: " << (int)(*((char *)p)) << endl;
    }
    
    Free = VirtualFree(p, 0, MEM_RELEASE);
    if (Free != 0 ){
        cout << "Memory Cleaned!" << endl;
    }
    else{
        cout << "No Cleaned Memory!" << endl;
    }
}

int main()
{
    system("chcp 1251");
    int input;
    int adr = 0; 
    void* word;
    DWORD size;
    string address;
    
    do {
        system("cls");
        Menu();
        cin >> input;
        switch (input) {
        case 1:
            SystemInformation(); 
            break;
        case 2:
            MemoryInformation();   
            break;
        case 3:
            InformationMemorySpec();
            break;
        case 4:
            reserveMemory();
            break;
        case 5:
            WriteDataCell();
            break;
        case 6:
            Protect();
            break;
        case 0:
            cout << "Exit Program" << endl;
            break;
        default:
            break;
        }
        system("pause");
    } while (input != 0);
    return 0;
}