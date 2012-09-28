/////////////////////////////////////////////////
// VFSCreator
/////////////////////////////////////////////////
// File: main_createVFS.cpp
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
#include <io.h>







//------------------------------------------------------------------------------------
// рекурсивная функция проверки подкаталогов
// CurrentSubDirName - текущий подкадалог чтения-поиска
// ReadPath - начальный путь чтения-поиска (не включая CurrentSubDirName)
// WritePath - начальный путь записи в VFS (не включая CurrentSubDirName)
//------------------------------------------------------------------------------------
void CheckSubDir(char * CurrentSubDirName, char *ReadPath, char *WritePath)
{
	// путь записи файлов в VFS
	char pszWritePath[MAX_PATH];
	ZeroMemory(pszWritePath, MAX_PATH); 
	strcpy_s(pszWritePath, MAX_PATH, WritePath);
	strcat_s(pszWritePath, MAX_PATH, CurrentSubDirName);
	strcat_s(pszWritePath, MAX_PATH, "/");

	// путь чтения-поиска файлов
	char pszReadPath[MAX_PATH];
	ZeroMemory(pszReadPath, MAX_PATH); 
	strcpy_s(pszReadPath, MAX_PATH, ReadPath);
	strcat_s(pszReadPath, MAX_PATH, CurrentSubDirName);
	strcat_s(pszReadPath, MAX_PATH, "/*.*");
	struct _finddata_t c_file;
	intptr_t h_file = _findfirst(pszReadPath, &c_file);

	if(h_file != -1L )
	{
		do{
			if ( !((c_file.attrib & _A_HIDDEN) == _A_HIDDEN) &&
			!((c_file.attrib & _A_SYSTEM) == _A_SYSTEM) &&
			strcmp(c_file.name,".") &&
			strcmp(c_file.name,"..") )
			{
				if (((c_file.attrib & _A_SUBDIR) == _A_SUBDIR))
				{
					char pszFilePathR[MAX_PATH];
					ZeroMemory(pszFilePathR, MAX_PATH); 
					strcpy_s(pszFilePathR, MAX_PATH, ReadPath);
					strcat_s(pszFilePathR, MAX_PATH, CurrentSubDirName);
					strcat_s(pszFilePathR, MAX_PATH, "/");
					CheckSubDir(c_file.name, pszFilePathR, pszWritePath);
				}
				else
				{
					char pszFilePathR[MAX_PATH];
					ZeroMemory(pszFilePathR, MAX_PATH); 
					strcpy_s(pszFilePathR, MAX_PATH, ReadPath);
					strcat_s(pszFilePathR, MAX_PATH, CurrentSubDirName);
					strcat_s(pszFilePathR, MAX_PATH, "/");
					strcat_s(pszFilePathR, MAX_PATH, c_file.name);

					char pszFilePathW[MAX_PATH];
					ZeroMemory(pszFilePathW, MAX_PATH); 
					strcpy_s(pszFilePathW, MAX_PATH, WritePath);
					strcat_s(pszFilePathW, MAX_PATH, CurrentSubDirName);
					strcat_s(pszFilePathW, MAX_PATH, "/");
					strcat_s(pszFilePathW, MAX_PATH, c_file.name);

					vw_CopyFStoVFS(pszFilePathR, pszFilePathW);
				}
			}

		}while(_findnext(h_file, &c_file) != -1);

		_findclose( h_file );
	}

}






//------------------------------------------------------------------------------------
// собственно процедура генерации VFS файла
//------------------------------------------------------------------------------------
void CreateVFS(char *ReadPath, char *WritePath, char *VFSfile, char *ArhKey, int Ver)
{
	vw_CreateVFS(VFSfile, ArhKey, false, Ver);

	// текущий подкоталог, полный путь чтения, путь записи
	CheckSubDir("", ReadPath, WritePath);

	vw_CloseVFS();
}





