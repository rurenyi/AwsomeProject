#include "ViewControl.h"
#include "framework.h"

int OpenTargetFile(OPENFILENAME* stOpenFile, HWND hDlg) {
	TCHAR szFileExt[100] = TEXT("*.exe;*.dll;*.scr;*.drv;*.sys");
	TCHAR szFileName[256];
	memset(szFileName, 0, MAX_PATH);
	memset(stOpenFile, 0, sizeof(OPENFILENAME));
	stOpenFile->lStructSize = sizeof(OPENFILENAME);
	stOpenFile->Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	stOpenFile->hwndOwner = hDlg;
	stOpenFile->lpstrFilter = szFileExt;
	stOpenFile->lpstrFile = szFileName;
	stOpenFile->nMaxFile = MAX_PATH;
	GetOpenFileName(stOpenFile);
	if (*szFileName == NULL) {
		MessageBox(0, TEXT("Please Select Something!!!"), 0, 0);
		return 0;
	}

	AddIAT(szFileName, hDlg);
	return 1;
}

int OpenDllFile(OPENFILENAME* stOpenFile, HWND hDlg, WCHAR* ProcName) {
	TCHAR szFileExt[100] = TEXT("*.exe;*.dll;*.scr;*.drv;*.sys");
	TCHAR szFileName[256];
	memset(szFileName, 0, MAX_PATH);
	memset(stOpenFile, 0, sizeof(OPENFILENAME));
	stOpenFile->lStructSize = sizeof(OPENFILENAME);
	stOpenFile->Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	stOpenFile->hwndOwner = hDlg;
	stOpenFile->lpstrFilter = szFileExt;
	stOpenFile->lpstrFile = szFileName;
	stOpenFile->nMaxFile = MAX_PATH;
	GetOpenFileName(stOpenFile);
	if (*szFileName == NULL) {
		MessageBox(0, TEXT("Please Select Something!!!"), 0, 0);
		return 0;
	}
	wcscpy(ProcName, szFileName);
	return 1;
}

HWND InitList(HWND hwnd) {
	LV_COLUMN lv;
	HWND hListProcess;
	memset(&lv, 0, sizeof(LV_COLUMN));
	hListProcess = GetDlgItem(hwnd, IDC_LIST_FunName);
	SendMessage(hListProcess, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	// 1st column	
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = TEXT("DLL Name");
	lv.cx = 100;
	lv.iSubItem = 0;	
	SendMessage(hListProcess, LVM_INSERTCOLUMN, 0, (LPARAM)&lv);

	// 2nd column
	lv.pszText = TEXT("Function Name");
	lv.cx = 100;
	lv.iSubItem = 1;
	ListView_InsertColumn(hListProcess, 1, &lv);	
	
	// 3rd column
	lv.pszText = TEXT("Function Number");
	lv.cx = 100;
	lv.iSubItem = 2;
	ListView_InsertColumn(hListProcess, 2, &lv);

	// 4th column	
	lv.pszText = TEXT("Image Base");
	lv.cx = 100;
	lv.iSubItem = 3;
	ListView_InsertColumn(hListProcess, 3, &lv);

	// 5th column
	lv.pszText = TEXT("Entry Point");
	lv.cx = 100;
	lv.iSubItem = 4;
	ListView_InsertColumn(hListProcess, 4, &lv);

	return hListProcess;
}

void EnumProcess(HWND hListProcess) {
	LV_ITEM vitem;

	// initialing
	memset(&vitem, 0, sizeof(LV_ITEM));
	vitem.mask = LVIF_TEXT;

	vitem.pszText = TEXT("csrss.exe");
	vitem.iItem = 0;
	vitem.iSubItem = 0;
	SendMessage(hListProcess, LVM_INSERTITEM, 0, (DWORD)&vitem);

	vitem.pszText = TEXT("448");
	vitem.iItem = 0;
	vitem.iSubItem = 1;
	ListView_SetItem(hListProcess, &vitem);

	vitem.pszText = TEXT("56590000");
	vitem.iItem = 0;
	vitem.iSubItem = 2;
	ListView_SetItem(hListProcess, &vitem);


	vitem.pszText = TEXT("winlogon.exe");
	vitem.iItem = 1;
	vitem.iSubItem = 0;
	SendMessage(hListProcess, LVM_INSERTITEM, 0, (DWORD)&vitem);

	vitem.pszText = TEXT("456");
	vitem.iSubItem = 1;
	ListView_SetItem(hListProcess, &vitem);

	vitem.pszText = TEXT("10000000");
	vitem.iSubItem = 2;
	ListView_SetItem(hListProcess, &vitem);


}

void AddIAT(TCHAR FileName[], HWND hListProcess) {
	char* EXEContent = ReadFile(FileName);
	char* NewFile = FileExtension(EXEContent);
	ReadIAT(NewFile, hListProcess);
	free(EXEContent);
	free(NewFile);
}