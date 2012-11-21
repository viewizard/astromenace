/********************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2012 Michael Kurinnoy, Viewizard


	AstroMenace is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	AstroMenace is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with AstroMenace. If not, see <http://www.gnu.org/licenses/>.


	Web Site:		http://www.viewizard.com/
	E-mail:			viewizard@viewizard.com
	Postal address:	kvartal Zhukova, 10/79, Lugansk city, 91051, Ukraine


*********************************************************************************/


#ifndef VFS_H
#define VFS_H


#include "../Base.h"



// File present in the VFS
#define VFS_FILE_VFS							0x0031
// File present in the File System
#define VFS_FILE_FS								0x0032





// Current VFS version
#define VFS_VER									"v1.5"
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

// Create VFS file
int		vw_CreateVFS(const char *Name, unsigned int BuildNumber);
// Write data from memory into VFS file
int		vw_WriteIntoVFSfromMemory(const char *Name, const BYTE * buffer, int size);
// Open VFS file
int		vw_OpenVFS(const char *Name, unsigned int BuildNumber);
// Close VFS file
void	vw_CloseVFS(void);
// Shutdown VFS (all eFILE files will be closed)
void	vw_ShutdownVFS(void);
// Get file location status (FS or VFS)
int 	FileDetect(const char *FileName);
// Create file "symlink"
bool	vw_CreateEntryLinkVFS(const char *FileName, const char *FileNameLink);
// Delete file "symlink"
bool	vw_DeleteEntryLinkVFS(const char *FileNameLink);
// Delete all "symlinks" in VFS
bool	vw_DeleteAllLinksVFS();

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
VFS v1.5

  4b - 'VFS_'
  4b - 'v1.5'
  4b - VFS build number
  4b - file table offset
  4b - file table size
  ?b - data (file data one by one)

  - File table structure
  1b - 00, if no encoding key
	or
  1b - encoding key size in byte
  ?b - encoding key
  2b - file name size
  ?b - file name
  4b - file position offset in VFS file
  4b - file size in VFS file
  4b - original file size
*/



#endif // VFS_H

