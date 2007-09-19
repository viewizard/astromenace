/////////////////////////////////////////////////
// Viewizard Game Engine
/////////////////////////////////////////////////
// File: vfs_var.h
/////////////////////////////////////////////////
// Copyright (c) 2001-2006, Michael Kurinnoy
// All Rights Reserved.
/////////////////////////////////////////////////
// Ver : 2.0
/////////////////////////////////////////////////




#ifndef vfs_var_H
#define vfs_var_H



//------------------------------------------------------------------------------------
// include и lib...
//------------------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>













//------------------------------------------------------------------------------------
// VFS
//
// Virtual File System
// Now supported VFS: v1.1, v1.2, v1.3
//------------------------------------------------------------------------------------

// Current VFS version
#define VFS_VER									"v1.3" 
// Data compression
#define VFS_DATA_ARH_NONE						'0'
#define VFS_DATA_ARH_RLE						'1'
#define VFS_DATA_ARH_HAFF						'2'



struct eFILE
{
	char*	Name;		// File name
  	int		VFS_Offset;	// File offset in the VFS file
	int		PackLength;	// File size in the VFS file
	int		RealLength;	// File size in the memory (unpacked file size)
	int		Pos;		// Current position in the file
	bool	Flush;		// File will flushed on close.
	BYTE*	Data;		// Pointer to the file data (unpacked) in the memory

	eFILE*	Prev;		// Pointer to the previous opened file
	eFILE*	Next;		// Pointer to the next opened file
};



// VFS functions

// Create VFS file
int	vw_CreateVFS(const char *Name, const char *nArhKey, bool WriteArhKey, int Ver = 2);//2=1.2
// Close VFS file
void	vw_CloseVFS(void);
// Copy file from File System into VFS
int	vw_CopyFStoVFS(const char *SrcFileName, const char *DstFileName);
// Copy memory file into VFS
int	vw_WriteIntoVFS(const char *Name, const byte * buffer, int size);

//------------------------------------------------------------------------------------








//------------------------------------------------------------------------------------
// переменные
//------------------------------------------------------------------------------------


// File present in the VFS
#define VFS_FILE_VFS							0x0031
// File present in the File System
#define VFS_FILE_FS								0x0032


// Структура записи в системе VFS (для v1.3)
typedef struct
{
	DWORD	NameLen;	// Кол-во байт в имени...
	char	*Name;		// Имя записи (имя файла)
	int		Offset;		// Смещение начала файла относительно начала файловой системы
	int		Length;		// Длина файла в системе
	int		RealLength;	// Длина файла после распаковки
	// для 1.3
	byte	ArhKeyLen;	// Кол-во байт ключа упаковки
	char	*ArhKey;	// Ключ-упаковки
} eVFS_Entry;   

extern char *VFSFileName;		// Имя файла VFS
extern FILE *VFSFile;			// Указатель на файл виртуальной системы
extern byte *dstVFS;			// данные для архивир/разарх
extern byte *srcVFS;			// данные для архивир/разарх
extern int  dsizeVFS;			// данные для архивир/разарх
extern int  ssizeVFS;			// данные для архивир/разарх
extern int  DataStartOffsetVFS;// смещение данных в структуре VFS (без 8 байт смещ табл)
extern int  HeaderOffsetVFS;	// смещение таблицы данных в структуре VFS
extern int  HeaderLengthVFS;	// Длина таблицы данный структуры VFS
extern int  NumberOfFilesVFS;	// количество файлов в VFS
extern bool VFSFileOpen;	// Датчик открытой VFS
extern bool VFSFileWritable;
extern eVFS_Entry *VFSdirArray;// Массив записей VFS
extern bool SetUpperVFS;	// установка букв в названии файла в верхний регистр (проверка без учета регистра)
extern char *ArhKeyVFS;		// Ключ-упаковки
extern eFILE *StartFileVFS;	// Указатель на первый файл в списке...
extern eFILE *EndFileVFS;		// Указатель на последний файл в списке...

extern int VFSversion;

int vw_DATAtoRLE(void);
int vw_DATAtoHAFF(void);
int vw_RLEtoDATA(int size);
int vw_HAFFtoDATA(int size);




/* 
Описание структуры VFS v1.2

  4б - 'VFS_'
  4б - 'v1.2'

  ~~
   1б - 00, если не передается последовательность раскодирования
  или
   1б - кол-во байт последовательности раскодирования
   Nб - собственно последовательность
  ~~

  4б - смещение таблицы файлов
  4б - длина таблицы файлов
  ?б - данные(тела файлов один за другим)
 
   Далее таблица файлов...
  --структура записи таблицы файлов
  2б - кол-во символов в имени файла
  Nб - имя файла
  4б - смещение начала файла в системе
  4б - длина файла
  4б - длина файла в распакованном виде
*/

/* 
Описание структуры VFS v1.3

  4б - 'VFS_'
  4б - 'v1.3'
  4б - смещение таблицы файлов
  4б - длина таблицы файлов
  ?б - данные(тела файлов один за другим)
 
   Далее таблица файлов...
  --структура записи таблицы файлов
  1б - 00, если не передается последовательность раскодирования
     или
  1б - кол-во байт последовательности раскодирования
  Nб - собственно последовательность
  2б - кол-во символов в имени файла
  Nб - имя файла
  4б - смещение начала файла в системе
  4б - длина файла
  4б - длина файла в распакованном виде
*/



#endif

