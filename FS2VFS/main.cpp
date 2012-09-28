/////////////////////////////////////////////////
// VFSCreator
/////////////////////////////////////////////////
// File: main.cpp
/////////////////////////////////////////////////
// Copyright (c) 2004, Michael Kurinnoy
// All Rights Reserved.
/////////////////////////////////////////////////
// Ver : 1.0
/////////////////////////////////////////////////




//------------------------------------------------------------------------------------
// include и lib...
//------------------------------------------------------------------------------------
#include "vfs_var.h"
#include "resource.h"

extern void CreateVFS(char *ReadPath, char *WritePath, char *VFSfile, char *ArhKey, int Ver);




//------------------------------------------------------------------------------------
// переменные... глобальные
//------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------
// переменные... локальные
//------------------------------------------------------------------------------------
char ScrDir[MAX_PATH];
char DstVFSFile[MAX_PATH];
char VFSInitFolder[MAX_PATH];
char VFSArhKey[MAX_PATH];
char VFSversionDetection[MAX_PATH];
int ver = 2;








//------------------------------------------------------------------------------------
// обработчик диалога ...
//------------------------------------------------------------------------------------
INT_PTR CALLBACK DLG( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{

    switch( msg )
    {
        case WM_INITDIALOG:
			{
// получаем текущий абсолюный путь
				char ProgrammDir[MAX_PATH];

				ZeroMemory(ProgrammDir, sizeof(ProgrammDir));
				GetModuleFileName(NULL, ProgrammDir, MAX_PATH);
				char* s = strrchr(ProgrammDir,'\\');
				if (s) s[0]=0x0;
				const char *Fi = "\\";
				strcat( ProgrammDir, Fi );

				char VFSFileNamePath[MAX_PATH];
				char DatFileName[MAX_PATH];

				strcpy(DatFileName, ProgrammDir);
				strcat(DatFileName, "DATA");

				strcpy(VFSFileNamePath, ProgrammDir);
				strcat(VFSFileNamePath, "gamedata.vfs");

				SetDlgItemText(hDlg,IDC_EDIT1 ,DatFileName);
				SetDlgItemText(hDlg,IDC_EDIT2 ,VFSFileNamePath);
				SetDlgItemText(hDlg,IDC_EDIT3 ,"DATA");
				SetDlgItemText(hDlg,IDC_EDIT4 ,"1");
				SetDlgItemText(hDlg,IDC_EDIT5 ,"3");
			}
            break;

        case WM_COMMAND:
			switch( LOWORD(wParam) )
            {

                case IDCANCEL:
                    EndDialog(hDlg, IDCANCEL);
                    break;


  				case IDOK:
					GetDlgItemText(hDlg, IDC_EDIT1, ScrDir, MAX_PATH);
					GetDlgItemText(hDlg, IDC_EDIT2, DstVFSFile, MAX_PATH);
					GetDlgItemText(hDlg, IDC_EDIT3, VFSInitFolder, MAX_PATH);
					GetDlgItemText(hDlg, IDC_EDIT4, VFSArhKey, MAX_PATH);
					GetDlgItemText(hDlg, IDC_EDIT5, VFSversionDetection, MAX_PATH);
					if (VFSversionDetection[0] == '1') ver = 1;
					if (VFSversionDetection[0] == '2') ver = 2;
					if (VFSversionDetection[0] == '3') ver = 3;
					CreateVFS(ScrDir, VFSInitFolder, DstVFSFile, VFSArhKey, ver);
					
					MessageBox(hDlg,"VFS has been created!","VFSCreater Status:",MB_TOPMOST|MB_OK);
                    //EndDialog(hDlg, IDOK);
                    break;



                default:
                    return FALSE;
            }
            break;
        

        default:
            return FALSE; 
    }
    return TRUE; 
}












//------------------------------------------------------------------------------------
// основная процедура...
//------------------------------------------------------------------------------------
int WINAPI WinMain(	HINSTANCE	hInstance,		
					HINSTANCE	hPrevInstance,	 
					LPSTR		lpCmdLine,		
					int			nCmdShow)			
{
	// запускаем прорисовку диалога...
	DialogBoxParam(hInstance,
		   MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DLG, DS_MODALFRAME);


	// уходим из программы...
	return 0;
}
