/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: VFS.h

	Copyright (c) 2001-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 3.0

******************************************************************************

	AstroMenace game source code available under "dual licensing" model.
	The licensing options available are:

	* Commercial Licensing. This is the appropriate option if you are
	  creating proprietary applications and you are not prepared to
	  distribute and share the source code of your application.
	  Contact us for pricing at viewizard@viewizard.com

	* Open Source Licensing. This is the appropriate option if you want
	  to share the source code of your application with everyone you
	  distribute it to, and you also want to give them the right to share
	  who uses it. You should have received a copy of the GNU General Public
	  License version 3 with this source codes.
	  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/


#ifndef VFS_H
#define VFS_H


#include "../Base.h"




// File present in the VFS
#define VFS_FILE_VFS							0x0031
// File present in the File System
#define VFS_FILE_FS								0x0032





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
	long	Pos;		// Current position in the file
	BYTE*	Data;		// Pointer to the file data (unpacked) in the memory

	// Read data
	int fread(void *buffer, size_t size, size_t count);
	// Set current position in the file
	int fseek(long offset, int origin);
	// Get current position in the file
	long ftell();
	// Check End-of-File indicator
	int feof();
	// Get string from stream
	char *fgets(char *str, int num);


	eFILE*	Prev;		// Pointer to the previous opened file
	eFILE*	Next;		// Pointer to the next opened file
};



// VFS functions

// Open VFS file
int		vw_OpenVFS(const char *Name);
// Close VFS file
void	vw_CloseVFS(void);
// Shutdown VFS (all eFILE files will be closed)
void	vw_ShutdownVFS(void);
// проверка принадлежности файла к vfs или fs
int 	FileDetect(const char *FileName);


// eFILE functions

// Open file
eFILE*	vw_fopen(const char *FileName);
// Close file
int		vw_fclose(eFILE *stream);




int vw_HAFFtoDATA(int size, BYTE **dstVFS, BYTE *srcVFS, int *dsizeVFS, int ssizeVFS);
int vw_RLEtoDATA(int size, BYTE **dstVFS, BYTE *srcVFS, int *dsizeVFS, int ssizeVFS);
int vw_DATAtoHAFF(BYTE **dstVFS, BYTE *srcVFS, int *dsizeVFS, int ssizeVFS);
int vw_DATAtoRLE(BYTE **dstVFS, BYTE *srcVFS, int *dsizeVFS, int ssizeVFS);


/*
Описание структуры VFS v1.3

  4б - 'VFS_'
  4б - 'v1.1'
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



#endif // VFS_H

