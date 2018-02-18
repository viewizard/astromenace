/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright (c) 2006-2018 Mikhail Kurinnoi, Viewizard


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


	Web Site: http://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#ifndef VFS_H
#define VFS_H

#include "../base.h"

#define VFS_VER "v1.6"

/*

 Game data VFS v1.6 structure.

  4b - 'VFS_'
  4b - 'v1.6'
  4b - VFS build number
  4b - file table offset
  4b - file table size
  ?b - data (file data one by one)

  - File table structure
  2b - file name size
  ?b - file name
  4b - file position offset in VFS file
  4b - file size

*/

struct eVFS {
	char		*FileName;
	SDL_RWops	*File;

	int	HeaderLengthVFS;
	int	HeaderOffsetVFS;
	int	DataStartOffsetVFS;
	bool	Writable;

	eVFS	*Prev;
	eVFS	*Next;
};

struct eVFS_Entry {
	DWORD	NameSize;
	char	*Name;
	int	Offset;
	int	Size;

	eVFS	*Parent;

	eVFS_Entry	*Prev;
	eVFS_Entry	*Next;
};

/* Get first entry from VFS linked list */
eVFS_Entry *vw_GetStartVFSArray();
/* Create VFS file */
eVFS	*vw_CreateVFS(const char *Name, unsigned int BuildNumber);
/* Write data from file into VFS file */
int	vw_WriteIntoVFSfromFile(eVFS *WritableVFS, const char *SrcName, const char *DstName);
/* Write data from memory into VFS file */
int	vw_WriteIntoVFSfromMemory(eVFS *WritableVFS, const char *Name, const BYTE *buffer, int size);
/* Open VFS file */
int	vw_OpenVFS(const char *Name, unsigned int BuildNumber);
/* Close all VFS */
void	vw_CloseVFS();
/* Shutdown VFS (all eFILE files will be closed) */
void	vw_ShutdownVFS();

struct eFILE {
	char	*Name;
	int	VFS_Offset;
	int	Size;
	long	Pos;
	BYTE	*Data;

	int	fread(void *buffer, size_t size, size_t count);
	int	fseek(long offset, int origin);
	long	ftell();

	eFILE	*Prev;
	eFILE	*Next;
};

eFILE	*vw_fopen(const char *FileName);
int	vw_fclose(eFILE *stream);

#endif // VFS_H
