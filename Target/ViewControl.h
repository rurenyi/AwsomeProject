#pragma once
#include"framework.h"
#include<commdlg.h>

HWND InitList(HWND hwnd);

void EnumProcess(HWND hListProcess);

int OpenTargetFile(OPENFILENAME* stOpenFile, HWND hDlg);

void AddIAT(TCHAR FileName[], HWND hListProcess);