#include "ReadIAT.h"
#include "framework.h"


char* FileExtension(char* FilePtr) {
	PIMAGE_DOS_HEADER Dos_header = (PIMAGE_DOS_HEADER)FilePtr;
	UINT32 NT = Dos_header->e_lfanew;
	PIMAGE_FILE_HEADER FIle_header = (PIMAGE_FILE_HEADER)((PUCHAR)FilePtr + NT + 0x04);
	PUCHAR ptr_of_lists = (PUCHAR)FilePtr + NT + 0x18 + FIle_header->SizeOfOptionalHeader;
	PIMAGE_OPTIONAL_HEADER32 Optional_header = (PIMAGE_OPTIONAL_HEADER32)((PUCHAR)FilePtr + NT + 0x18);
	char* NewBuffer = (char*)malloc(Optional_header->SizeOfImage);

	if (NewBuffer == NULL)
	{
		printf("Extened Failed when get buffer.\n");
		return NULL;
	}

	memset(NewBuffer, 0, Optional_header->SizeOfImage);
	memcpy(NewBuffer, FilePtr, Optional_header->SizeOfHeaders);
	PIMAGE_SECTION_HEADER Sections;
	for (size_t i = 0; i < FIle_header->NumberOfSections; i++)
	{
		Sections = (PIMAGE_SECTION_HEADER)(ptr_of_lists + i * 0x28);
		memcpy(NewBuffer + Sections->VirtualAddress, FilePtr + Sections->PointerToRawData, Sections->SizeOfRawData);
		printf("%X\n", Sections->VirtualAddress);
	}
	printf("Extension success.\n");
	return NewBuffer;
}

char* ReadFile(TCHAR FileName[]) {
	FILE* File = _wfopen(FileName, TEXT("rb"));
	if (File == NULL)
	{
		printf("File Open Failed\n");
		return 0;
	}
	fseek(File, 0, 2);
	long length = ftell(File);
	fseek(File, 0, 0);
	char* Content = (char*)malloc(length);
	if (Content == NULL)
	{
		printf("Get buffer Failed");
		return NULL;
	}
	memset(Content, 0, length);
	fread(Content, length, 1, File);
	fclose(File);
	return Content;
	printf("success\n");
}

void ReadIAT(char* FilePtr, HWND hListProcess) {
	PIMAGE_DOS_HEADER Dos_header = (PIMAGE_DOS_HEADER)FilePtr;
	UINT32 NT = Dos_header->e_lfanew;
	PIMAGE_FILE_HEADER FIle_header = (PIMAGE_FILE_HEADER)((PUCHAR)FilePtr + NT + 0x04);

	if (Dos_header->e_magic != 0x5a4d)
	{
		MessageBox(0, TEXT("It is not executable programme !!!"), 0, 0);
		return;
	}

	if (FIle_header->Machine != 0x014C)
	{
		MessageBox(0, TEXT("Currently not support x64 flatform programme !!!"), 0, 0);
		return;
	}
	PIMAGE_OPTIONAL_HEADER32 Optional_header = (PIMAGE_OPTIONAL_HEADER32)((PUCHAR)FilePtr + NT + 0x18);
	PIMAGE_IMPORT_DESCRIPTOR Import = (PIMAGE_IMPORT_DESCRIPTOR)(Optional_header->DataDirectory[1].VirtualAddress + FilePtr);
	UINT32 entry_Point = Optional_header->AddressOfEntryPoint;


	LV_ITEM vitem;

	//initialing
	memset(&vitem, 0, sizeof(LV_ITEM));
	vitem.mask = LVIF_TEXT;
	int LineNumber = 0;

	while (Import->Name != NULL)
	{
		int Count = 0;
		int Func = 0;

		TCHAR DLLName[MAX_PATH];
		memset(DLLName, 0, MAX_PATH);
		mbstowcs(DLLName, Import->Name + FilePtr, strlen(Import->Name + FilePtr));

		while (true)
		{

			Func = *(int*)(Import->OriginalFirstThunk + Count * 4 + FilePtr);
			if (Func == NULL)
			{
				break;
			}

			vitem.pszText = DLLName;
			vitem.iItem = LineNumber;
			vitem.iSubItem = 0;
			SendMessage(hListProcess, LVM_INSERTITEM, 0, (LPARAM)&vitem);

			TCHAR Index[256];
			memset(Index, 0, 256);
			_itow(entry_Point, Index, 16);

			vitem.pszText = Index;
			vitem.iItem = LineNumber;
			vitem.iSubItem = 4;
			ListView_SetItem(hListProcess, &vitem);


			INT32 image_base = Optional_header->ImageBase;
			memset(Index, 0, 256);
			_itow(image_base, Index, 16);

			vitem.pszText = Index;
			vitem.iItem = LineNumber;
			vitem.iSubItem = 3;
			ListView_SetItem(hListProcess, &vitem);


			Count++;
			// Function Imoprt by number
			if (Func >= 0x80000000)
			{
				Func = Func - 0x80000000;
				TCHAR Num[256];
				memset(Num, 0, 256);
				_itow(Func, Num, 16);
				vitem.pszText = Num;
				vitem.iItem = LineNumber;
				vitem.iSubItem = 2;
				ListView_SetItem(hListProcess, &vitem);

			}
			else {
				// Function import By Name
				PIMAGE_IMPORT_BY_NAME Import_Name = (PIMAGE_IMPORT_BY_NAME)(FilePtr + Func);
				TCHAR wFunName[MAX_PATH];
				memset(wFunName, 0, MAX_PATH);
				mbstowcs(wFunName, Import_Name->Name, strlen(Import_Name->Name));

				vitem.pszText = wFunName;
				vitem.iItem = LineNumber;
				vitem.iSubItem = 1;
				ListView_SetItem(hListProcess, &vitem);
			}
			LineNumber++;
		}
		Import++;
	}
}

