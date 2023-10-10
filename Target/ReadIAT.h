#pragma once
#include"framework.h"

char* ReadFile(TCHAR FileName[]);

char* FileExtension(char* FilePtr);

void ReadIAT(char* FilePtr, HWND hListProcess);