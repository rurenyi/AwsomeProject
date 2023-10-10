#include "Hook.h"
#include"framework.h"


BOOL EnableDebugPrivilege()
{

    HANDLE hToken;

    BOOL fOk = FALSE;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))

    {

        TOKEN_PRIVILEGES tp;

        tp.PrivilegeCount = 1;

        LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);

        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;     // Start Debug

        AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);

        int res = GetLastError();

        fOk = (GetLastError() == ERROR_SUCCESS);

        CloseHandle(hToken);

    }

    return fOk;

}

HANDLE GetHandleByPID(DWORD PID) {
    HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
    if (handle == NULL)
    {
        int error;
        error = GetLastError();
        //printf("Open Process Failed\n");
        MessageBox(0,TEXT("Open Process Failed.\n"), 0, 0);
        //printf("Error Code Is : %X\n", error);
        return 0;
    }
    return handle;
}

HANDLE GetHandleByName(WCHAR* pName)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hSnapshot) {
        return NULL;
    }
    PROCESSENTRY32 pe = { sizeof(pe) };
    for (BOOL ret = Process32First(hSnapshot, &pe); ret; ret = Process32Next(hSnapshot, &pe)) {
        if (wcscmp(pe.szExeFile, pName) == 0) {
            CloseHandle(hSnapshot);
            //printf("PID found\n");
            HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
            if (handle == NULL)
            {
                //printf("Open Process Failed\n");
                int error;
                error = GetLastError();
                //printf("Error Code Is : %X\n", error);
                return 0;
            }
            return handle;
        }
    }
    CloseHandle(hSnapshot);
    //printf("process not found\n");
    MessageBox(0, TEXT("process not found."), 0, 0);

    return 0;
}

int Injection(HANDLE ProcessHandle, HANDLE CurrentHandle, PCHAR DllName) {
    //int res;
    PVOID RemoteTextMemory = VirtualAllocEx(ProcessHandle, NULL, strlen(DllName) + 4, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    //res = GetLastError();
    WriteProcessMemory(ProcessHandle, RemoteTextMemory, DllName, strlen(DllName) + 4, NULL);
    //res = GetLastError();
    CreateRemoteThread(ProcessHandle, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryA, (PCHAR)RemoteTextMemory, NULL, NULL);
    int res = GetLastError();
    if (res != 0)
    {
        MessageBox(0, TEXT("Injection Error!"), 0, 0);
        return res;
    }
    return 0;
}

int LoadDLL(WCHAR* ProcName) {
    HANDLE ProcessHandle;
    HANDLE CurrentHandle;

    BOOL Privilege = EnableDebugPrivilege();

    if (Privilege == FALSE)
    {
    // printf("Enable Debug privilege Failed.\n");
        MessageBox(0,TEXT("Enable Debug privilege Failed.\n"), 0, 0);
    }
    //ProcessHandle = GetHandleByName(TEXT("Target.exe"));
    //ProcessHandle = GetHandleByName(TEXT("Taskmgr.exe"));
    ProcessHandle = GetHandleByName(ProcName);

    //printf("%x", ProcessHandle);

    CurrentHandle = GetModuleHandle(NULL);

    CHAR InjectionDllName[] = "C:\\Users\\71559\\source\\repos\\InjectionDLL\\x64\\Debug\\InjectionDLL.dll";

    int Res = Injection(ProcessHandle, CurrentHandle, InjectionDllName);

    if (Res == 0)
    {
        MessageBox(0, TEXT("Injection Success!"), 0, 0);
    }
    else {
        MessageBox(0, TEXT("Injection Failed!"), 0, 0);
    }

    return 1;
}

void Hook(TCHAR FileName[], int index) {
	char* FilePtr = ReadFile(FileName);
	char* FilePtrEX = FileExtension(FilePtr);
    
	free(FilePtr);
	free(FilePtrEX);
}