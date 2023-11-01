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
        MessageBox(0,TEXT("Open Process Failed.\n"), 0, 0);
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
            HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
            if (handle == NULL)
            {
                int error;
                error = GetLastError();
                return 0;
            }
            return handle;
        }
    }
    CloseHandle(hSnapshot);
    MessageBox(0, TEXT("process not found."), 0, 0);

    return 0;
}

int Injection(HANDLE ProcessHandle, HANDLE CurrentHandle, PCHAR DllName) {

    PVOID RemoteTextMemory = VirtualAllocEx(ProcessHandle, NULL, strlen(DllName) + 4, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

    WriteProcessMemory(ProcessHandle, RemoteTextMemory, DllName, strlen(DllName) + 4, NULL);

    CreateRemoteThread(ProcessHandle, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryA, (PCHAR)RemoteTextMemory, NULL, NULL);
    int res = GetLastError();
    if (res != 0)
    {
        MessageBox(0, TEXT("Injection Error!"), 0, 0);
        return res;
    }
    return 0;
}

int LoadDLL(WCHAR* ProcName, WCHAR* DllName) {
    HANDLE ProcessHandle;
    HANDLE CurrentHandle;

    BOOL Privilege = EnableDebugPrivilege();

    if (Privilege == FALSE)
    {
        MessageBox(0,TEXT("Enable Debug privilege Failed.\n"), 0, 0);
    }
    ProcessHandle = GetHandleByName(ProcName);

    MessageBox(0, DllName, 0, 0);
    CurrentHandle = GetModuleHandle(NULL);
    CHAR InjectionDllName[MAX_PATH];
    wcstombs(InjectionDllName, DllName, MAX_PATH);
    MessageBoxA(0, InjectionDllName, 0, 0);

    int Res = Injection(ProcessHandle, CurrentHandle, InjectionDllName);

    if (Res == 0)
    {
        MessageBox(0, TEXT("Injection Success!"), 0, 0);
    }
    return 1;
}

void Hook(TCHAR FileName[], int index) {
	char* FilePtr = ReadFile(FileName);
	char* FilePtrEX = FileExtension(FilePtr);
    
	free(FilePtr);
	free(FilePtrEX);
}