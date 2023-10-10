#pragma once
#include"framework.h"

char* StartMemoryShare();

HWND InitProcessList(HWND hwnd);

int RefreshProcList(HWND hProcList);

void GetSelect(HWND hProcList, WPARAM wPara, LPARAM lPara , WCHAR* ProcName);