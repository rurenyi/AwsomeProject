// Target.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "Target.h"

// 全局变量:

HINSTANCE hInstance;
HWND hdlg;
HWND Dlg;
HWND hProcList;
WCHAR ProcName[MAX_PATH] = TEXT("not");

//CALLBACK
BOOL CALLBACK MainDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    OPENFILENAME stOpenFile;
    BOOL bret = FALSE;
    NMHDR* pNMHDR = (NMHDR*)lParam;
    /*WCHAR ProcName[MAX_PATH] = TEXT("not");*/

    switch (uMsg)
    {
    case WM_INITDIALOG:
        Dlg = InitList(hdlg);
        hProcList = InitProcessList(hdlg);
        return TRUE;

    case WM_NOTIFY:
        if (wParam == IDC_LIST_Process && pNMHDR->code == NM_CLICK) {
            GetSelect(GetDlgItem(hdlg, IDC_LIST_Process), wParam, lParam, ProcName);
        }
        break;

    case WM_CLOSE:
    {
        EndDialog(hdlg, 0);
        return 1;
    }
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
            case IDC_BUTTON_OpenFile:
            {
                int result = OpenTargetFile((OPENFILENAME*)&stOpenFile, Dlg);
                break;
            }
            case IDC_BUTTON_Hook:
            {
                // nothing is going to happen.....
                char* SharedMemory = StartMemoryShare();
                if (wcscmp(ProcName, TEXT("not"))==0) {
                    MessageBox(0, TEXT("Please select target progress!"), 0, 0);
                    break;
                }
                LoadDLL(ProcName);
                return true;
            }

            case IDC_BUTTON_Refresh:
            {
                SendMessage(hProcList, LVM_DELETEALLITEMS, 0, 0);
                RefreshProcList(hProcList);
                return true;
            }


            case IDCANCEL:
            {
                EndDialog(hdlg, 0);
                return true;
            }

        }
    }
    break;
    }
    return bret;
}


//MAIN
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_Test), NULL,(DLGPROC)MainDlgProc);

    return 1;
}
