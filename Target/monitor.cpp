


#include "monitor.h"
#include "framework.h"

char* StartMemoryShare() {
	HANDLE hMapObject;
	HANDLE hMapView;

	//build FileMapping object	
	hMapObject = CreateFileMapping((HANDLE)0xFFFFFFFFFFFFFFFF, NULL, PAGE_READWRITE, 0, 0x10000, TEXT("shared"));
	if (!hMapObject)
	{
		//int error = GetLastError();
		MessageBox(NULL, TEXT("memory sharing failed"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	// open FileMapping	into my process
	hMapView = MapViewOfFile(hMapObject, FILE_MAP_WRITE, 0, 0, 0);
	if (!hMapView)
	{
		//int error = GetLastError();
		MessageBox(NULL, TEXT("memory mapping failed"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	strcpy((char*)hMapView, "Test Shared Memery");

	return (char*)hMapView;
}

HWND InitProcessList(HWND hwnd) {
	LV_COLUMN lv2;
	HWND hProcessListProcess;
	memset(&lv2, 0, sizeof(LV_COLUMN));
	hProcessListProcess = GetDlgItem(hwnd, IDC_LIST_Process);
	SendMessage(hProcessListProcess, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	// first line
	lv2.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv2.pszText = TEXT("Process Name");
	lv2.cx = 150;
	lv2.iSubItem = 0;
	//ListView_InsertColumn(hListProcess, 0, &lv);	
	SendMessage(hProcessListProcess, LVM_INSERTCOLUMN, 0, (LPARAM)&lv2);
	
	return hProcessListProcess;
	//EnumProcess(hListProcess);
}

int RefreshProcList(HWND hProcList) {
	WCHAR* ShitName = L"svchost.exe";

	LV_ITEM vitem;
	//iniitaling....
	memset(&vitem, 0, sizeof(LV_ITEM));
	vitem.mask = LVIF_TEXT;
	int LineNumber = 0;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot) {
		return NULL;
	}
	PROCESSENTRY32 pe = { sizeof(pe) };
	for (BOOL ret = Process32First(hSnapshot, &pe); ret; ret = Process32Next(hSnapshot, &pe)) {
		if (wcscmp(pe.szExeFile, ShitName))
		{
			vitem.pszText = pe.szExeFile;
			vitem.iItem = LineNumber;
			vitem.iSubItem = 0;

			SendMessage(hProcList, LVM_INSERTITEM, 0, (LPARAM)&vitem);
			LineNumber++;
		}
	}
	CloseHandle(hSnapshot);
	return 1;
}

void GetSelect(HWND hProcList, WPARAM wPara, LPARAM lPara , WCHAR* ProcName) {

	//WCHAR ProcName[MAX_PATH] = {0};
	DWORD RowID;
	LV_ITEM lv;
	memset(&lv, 0, sizeof(LV_ITEM));
	memset(ProcName, 0, MAX_PATH);
	RowID = SendMessage(hProcList, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
	if (RowID == -1)
	{
		MessageBox(0, TEXT("Please Select"), TEXT("Error"), MB_OK);
		return ;
	}
	lv.iSubItem = 0;
	lv.pszText = ProcName;
	lv.cchTextMax = MAX_PATH;
	SendMessage(hProcList, LVM_GETITEMTEXT, RowID, (DWORD64)&lv);
	//MessageBox(0, ProcName, TEXT("Process Name"), MB_OK);
}