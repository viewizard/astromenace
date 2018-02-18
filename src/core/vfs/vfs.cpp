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

#include "vfs.h"

/* Doubly-linked list with connected VFS. */
eVFS *StartVFS = NULL;
eVFS *EndVFS = NULL;

/* Doubly-linked list with files in VFS. */
eVFS_Entry *StartVFSArray = NULL;
eVFS_Entry *EndVFSArray = NULL;

/* Doubly-linked list with opened files. */
eFILE *StartFileVFS = NULL;
eFILE *EndFileVFS = NULL;

enum file_location {
FILE_NOT_FOUND = -1,
FILE_IN_VFS = 1, /* File present in the VFS. */
FILE_IN_FS = 2 /* File present in the File System. */
};


/*
 * Get first entry from VFS linked list.
 */
eVFS_Entry *vw_GetStartVFSArray()
{
	return StartVFSArray;
}

/*
 * Create VFS file.
 */
eVFS *vw_CreateVFS(const char *Name, unsigned int BuildNumber)
{
	if (Name == NULL)
		return NULL;

	eVFS *TempVFS = new eVFS;

	TempVFS->HeaderLengthVFS = 0;
	TempVFS->HeaderOffsetVFS = 0;
	TempVFS->DataStartOffsetVFS = 0;
	TempVFS->Writable = true;

	TempVFS->File = SDL_RWFromFile(Name, "wb");
	if (TempVFS->File == NULL) {
		fprintf(stderr, "Can't open VFS file for write %s\n", Name);
		delete TempVFS;
		return NULL;
	}

	TempVFS->FileName = new char[strlen(Name)+1];
	strcpy(TempVFS->FileName, Name);

	/* attach VFS to linked list */
	if (EndVFS == NULL) {
		TempVFS->Prev = NULL;
		TempVFS->Next = NULL;
		StartVFS = TempVFS;
		EndVFS = TempVFS;
	}
	else {
		TempVFS->Prev = EndVFS;
		TempVFS->Next = NULL;
		EndVFS->Next = TempVFS;
		EndVFS = TempVFS;
	}

	/* write VFS sign, version and build number */
	char tmp1[5] = "VFS_";
	SDL_RWwrite(TempVFS->File, tmp1, 4, 1);
	SDL_RWwrite(TempVFS->File, VFS_VER, 4, 1);
	SDL_RWwrite(TempVFS->File, &BuildNumber, 4, 1);

	/* write new table offset and size */
	TempVFS->HeaderOffsetVFS = 4+4+4+4+4; /* VFS_ + ver + build + offset + size */
	TempVFS->DataStartOffsetVFS = 4+4+4; /* VFS_ + ver + build */
	SDL_RWwrite(TempVFS->File, &TempVFS->HeaderOffsetVFS, 4, 1);
	SDL_RWwrite(TempVFS->File, &TempVFS->HeaderLengthVFS, 4, 1);

	printf("VFS file was created %s\n", Name);
	return TempVFS;
}

/*
 * Write data from file into VFS file.
 */
int vw_WriteIntoVFSfromFile(eVFS *WritableVFS, const char *SrcName, const char *DstName)
{
	if ((SrcName == 0) || (DstName == 0))
		return -1;

	// читаем данные файла в буфер
	SDL_RWops *Ftmp = SDL_RWFromFile(SrcName, "rb");
	// Если файл не найден - выходим
	if (Ftmp == NULL) {
		fprintf(stderr, "Can't find file %s\n", SrcName);
		return -1;
	}

	SDL_RWseek(Ftmp, 0, SEEK_END);
	int tmpSize = SDL_RWtell(Ftmp);
	SDL_RWseek(Ftmp, 0, SEEK_SET);

	/* copy data to tmp buffer */
	BYTE *tmp = new BYTE[tmpSize];
	SDL_RWread(Ftmp, tmp, tmpSize, 1);
	SDL_RWclose(Ftmp);

	/* write into VFS */
	int rc = vw_WriteIntoVFSfromMemory(WritableVFS, DstName, tmp, tmpSize);
	if (rc != 0)
		fprintf(stderr, "Can't write into VFS from memory %s\n", DstName);
	/* release tmp buffer */
	delete [] tmp;

	return rc;
}

/*
 * Write data from memory into VFS file.
 */
int vw_WriteIntoVFSfromMemory(eVFS *WritableVFS, const char *Name, const BYTE *buffer, int size)
{
	if ((WritableVFS == NULL) || (Name == NULL) ||
	    (buffer == NULL) || (size <= 0))
		return -1;

	if (!WritableVFS->Writable) {
		fprintf(stderr, "Can't write into VFS, VFS not writable %s\n", Name);
		return -1;
	}

	eVFS_Entry *NewVFS_Entry = new eVFS_Entry;

	/* attach VFS to linked list */
	if (EndVFSArray == NULL) {
		NewVFS_Entry->Prev = NULL;
		NewVFS_Entry->Next = NULL;
		StartVFSArray = NewVFS_Entry;
		EndVFSArray = NewVFS_Entry;
	}
	else {
		NewVFS_Entry->Prev = EndVFSArray;
		NewVFS_Entry->Next = NULL;
		EndVFSArray->Next = NewVFS_Entry;
		EndVFSArray = NewVFS_Entry;
	}

	NewVFS_Entry->Parent = WritableVFS;
	NewVFS_Entry->NameSize = (DWORD)strlen(Name);
	NewVFS_Entry->Name = new char[NewVFS_Entry->NameSize+1];
	strcpy(NewVFS_Entry->Name, Name);

	/* add new data to VFS file, in this case we could use
	 * HeaderOffsetVFS, since this is the end of data part
	 */
	NewVFS_Entry->Offset = WritableVFS->HeaderOffsetVFS;
	NewVFS_Entry->Size = size;
	SDL_RWseek(WritableVFS->File, NewVFS_Entry->Offset, SEEK_SET);
	SDL_RWwrite(WritableVFS->File, buffer, NewVFS_Entry->Size, 1);

	/* write all entries belong to this VFS file */
	eVFS_Entry *Tmp = StartVFSArray;
	while (Tmp != NULL) {
		eVFS_Entry *Tmp1 = Tmp->Next;

		if (NewVFS_Entry->Parent == WritableVFS) {
			SDL_RWwrite(WritableVFS->File, &Tmp->NameSize, 2, 1);
			SDL_RWwrite(WritableVFS->File, Tmp->Name, Tmp->NameSize, 1);
			SDL_RWwrite(WritableVFS->File, &Tmp->Offset, 4, 1);
			SDL_RWwrite(WritableVFS->File, &Tmp->Size, 4, 1);
		}

		Tmp = Tmp1;
	}

	/* correct table offset and size */
	WritableVFS->HeaderOffsetVFS += NewVFS_Entry->Size;
	WritableVFS->HeaderLengthVFS += 1+2+NewVFS_Entry->NameSize+4+4+4;

	/* rewrite table offset and size */
	SDL_RWseek(WritableVFS->File, WritableVFS->DataStartOffsetVFS, SEEK_SET);
	SDL_RWwrite(WritableVFS->File, &WritableVFS->HeaderOffsetVFS, 4, 1);
	SDL_RWwrite(WritableVFS->File, &WritableVFS->HeaderLengthVFS, 4, 1);

	printf("%s file added to VFS.\n", Name);
	return 0;
}

/*
 * Open VFS file.
 */
int vw_OpenVFS(const char *Name, unsigned int BuildNumber)
{
	unsigned int vfsBuildNumber = 0;
	int HeaderOffsetVFS;
	int HeaderLengthVFS;
	int VFS_FileSize;
	eVFS *TempVFS;

	TempVFS = new eVFS;
	TempVFS->Writable = false;

	TempVFS->File = SDL_RWFromFile(Name, "rb");
	if (TempVFS->File == NULL)
	{
		fprintf(stderr, "Can't find VFS file %s\n", Name);
		goto error;
	}

	SDL_RWseek(TempVFS->File,0,SEEK_END);
	VFS_FileSize = SDL_RWtell(TempVFS->File);
	SDL_RWseek(TempVFS->File,0,SEEK_SET);

	TempVFS->FileName = new char[strlen(Name)+1];
	strcpy(TempVFS->FileName, Name);

	/* check VFS file sign "VFS_" */
	char Sign[5];
	Sign[4] = '\0'; /* just to be sure, that we have null-terminated string */

	if(SDL_RWread(TempVFS->File, &Sign, 4, 1) == 0) {
		fprintf(stderr, "VFS file size error %s\n", Name);
		goto error;
	}
	if (strncmp(Sign, "VFS_", 4) != 0) {
		fprintf(stderr, "VFS file header error %s\n", Name);
		goto error;
	}

	/* check VFS file version */
	char Version[5];
	Version[4] = '\0'; /* just to be sure, that we have null-terminated string */

	if(SDL_RWread(TempVFS->File, &Version, 4, 1) == 0) {
		fprintf(stderr, "VFS file corrupted: %s\n", Name);
		goto error;
	}
	if (strncmp(Version, VFS_VER, 4) != 0) {
		fprintf(stderr,
			"VFS file has wrong version, version %s not supported: %s\n",
			Version, Name);
		goto error;
	}

	/* check VFS file build number */
	if(SDL_RWread(TempVFS->File, &vfsBuildNumber, 4, 1) == 0) {
		fprintf(stderr, "VFS file corrupted: %s\n", Name);
		goto error;
	}
	if (BuildNumber != 0) {
		if (vfsBuildNumber == 0) {
			printf("VFS file build number was not set (0).\n");
		}
		else {
			if (BuildNumber != vfsBuildNumber) {
				fprintf(stderr,
					"VFS file has wrong build number (%u), you need VFS with build number %u\n",
					vfsBuildNumber, BuildNumber);
				goto error;
			}
		}
	}

	/* all looks legit, attach VFS to linked list */
	if (EndVFS == NULL) {
		TempVFS->Prev = NULL;
		TempVFS->Next = NULL;
		StartVFS = TempVFS;
		EndVFS = TempVFS;
	}
	else {
		TempVFS->Prev = EndVFS;
		TempVFS->Next = NULL;
		EndVFS->Next = TempVFS;
		EndVFS = TempVFS;
	}

	SDL_RWread(TempVFS->File, &HeaderOffsetVFS, sizeof(HeaderOffsetVFS), 1);
	SDL_RWread(TempVFS->File, &HeaderLengthVFS, sizeof(HeaderLengthVFS), 1);
	SDL_RWseek(TempVFS->File, HeaderOffsetVFS, SEEK_SET);

	/* add entries from new connected VFS file */
	while (VFS_FileSize != SDL_RWtell(TempVFS->File)) {
		eVFS_Entry *Temp = new eVFS_Entry;

		if (EndVFSArray == NULL) {
			Temp->Prev = NULL;
			Temp->Next = NULL;
			StartVFSArray = Temp;
			EndVFSArray = Temp;
		}
		else {
			Temp->Prev = EndVFSArray;
			Temp->Next = NULL;
			EndVFSArray->Next = Temp;
			EndVFSArray = Temp;
		}

		Temp->Parent = TempVFS;

		SDL_RWread(TempVFS->File, &Temp->NameSize, 2, 1);
		Temp->Name = new char[Temp->NameSize+1];
		SDL_RWread(TempVFS->File, Temp->Name, Temp->NameSize, 1);
		/* just to be sure, that we have null-terminated string */
		Temp->Name[Temp->NameSize] = '\0';
		SDL_RWread(TempVFS->File, &(Temp->Offset), sizeof(Temp->Offset), 1);
		SDL_RWread(TempVFS->File, &(Temp->Size), sizeof(Temp->Size), 1);
	}

	printf("VFS file was opened %s\n", Name);
	return 0;

error:
	if (TempVFS->File != NULL)
		SDL_RWclose(TempVFS->File);
	if (TempVFS->FileName != NULL)
		delete [] TempVFS->FileName;
	delete TempVFS;

	return -1;
}

/*
 * Close all VFS.
 */
void vw_CloseVFS()
{
	/* release all VFS entry */
	eVFS_Entry *Tmp = StartVFSArray;
	while (Tmp != NULL)
	{
		eVFS_Entry *Tmp1 = Tmp->Next;

		if (Tmp->Name != NULL)
			delete [] Tmp->Name;

		if (StartVFSArray == Tmp)
			StartVFSArray = Tmp->Next;
		if (EndVFSArray == Tmp)
			EndVFSArray = Tmp->Prev;

		if (Tmp->Next != NULL)
			Tmp->Next->Prev = Tmp->Prev;
		else if (Tmp->Prev != NULL)
			Tmp->Prev->Next = NULL;

		if (Tmp->Prev != NULL)
			Tmp->Prev->Next = Tmp->Next;
		else if (Tmp->Next != NULL)
			Tmp->Next->Prev = NULL;

		delete Tmp;
		Tmp = Tmp1;
	}

	/* reset linked list */
	StartVFSArray = NULL;
	EndVFSArray = NULL;

	/* release all VFS */
	eVFS *TempVFS = StartVFS;
	while (TempVFS != NULL) {
		eVFS *TempVFS1 = TempVFS->Next;

		if (TempVFS->File != NULL)
			SDL_RWclose(TempVFS->File);

		if (TempVFS->FileName != NULL) {
			printf("VFS file was closed %s\n", TempVFS->FileName);
			delete [] TempVFS->FileName;
		}

		delete TempVFS;
		TempVFS = TempVFS1;
	}

	/* reset linked list */
	StartVFS = NULL;
	EndVFS = NULL;
}

/*
 * Shutdown VFS (all eFILE files will be closed).
 */
void vw_ShutdownVFS()
{
	/* close all files */
	if (StartFileVFS != NULL) {
		eFILE *Tmp = StartFileVFS;

		while (Tmp != NULL) {
			eFILE *Tmp1 = Tmp->Next;

			vw_fclose(Tmp);
			Tmp = Tmp1;
		}
	}

	/* reset linked list */
	StartFileVFS = NULL;
	EndFileVFS = NULL;

	vw_CloseVFS();
}

/*
 * Detect file location.
 */
static file_location DetectFileLocation(const char *FileName)
{
	if (FileName == NULL)
		return FILE_NOT_FOUND;

	/* trying to find file in VFS by name */
	eVFS_Entry *Tmp1 = StartVFSArray;
	while (Tmp1 != NULL) {
		eVFS_Entry *Tmp2 = Tmp1->Next;

		if(strcmp(FileName, Tmp1->Name) == 0)
			return FILE_IN_VFS;

		Tmp1 = Tmp2;
	}

	/* trying to open real file in file system */
	SDL_RWops *File = SDL_RWFromFile(FileName, "rb");
	if (File != NULL) {
		SDL_RWclose(File);
		return FILE_IN_FS;
	}

	return FILE_NOT_FOUND;
}

/*
 * Opens the eFILE.
 */
eFILE *vw_fopen(const char *FileName)
{
	file_location Location;
	eFILE *File;

	if (FileName == NULL)
		return NULL;

	Location = DetectFileLocation(FileName);

	if (Location == FILE_NOT_FOUND) {
		fprintf(stderr, "Can't find the file %s\n", FileName);
		return NULL;
	}

	/* initial memory allocation and setup */
	File = new eFILE;
	File->Pos = 0;
	File->Size = 0;
	File->VFS_Offset = 0;
	File->Name = new char[strlen(FileName)+1];
	strcpy(File->Name, FileName);

	if (Location == FILE_IN_VFS) {
		/* find file in VFS by name */
		eVFS_Entry *FileInVFS = NULL;
		eVFS_Entry *Tmp = StartVFSArray;

		while (Tmp != NULL) {
			eVFS_Entry *Tmp1 = Tmp->Next;

			if(strcmp(File->Name, Tmp->Name) == 0) {
				FileInVFS = Tmp;
				break;
			}
			Tmp = Tmp1;
		}

		File->VFS_Offset = FileInVFS->Offset;

		File->Data = new BYTE[FileInVFS->Size];

		SDL_RWseek(FileInVFS->Parent->File, File->VFS_Offset, SEEK_SET);
		SDL_RWread(FileInVFS->Parent->File, File->Data, FileInVFS->Size, 1);
		File->Size = FileInVFS->Size;
	} else if (Location == FILE_IN_FS) {
		/* open real file in file system */
		SDL_RWops * fTEMP = SDL_RWFromFile(FileName, "rb");

		SDL_RWseek(fTEMP, 0, SEEK_END);
		File->Size = SDL_RWtell(fTEMP);
		SDL_RWseek(fTEMP, 0, SEEK_SET);

		File->Data = new BYTE[File->Size];

		SDL_RWread(fTEMP, File->Data, File->Size, 1);
		SDL_RWclose(fTEMP);
	}

	/* add to linked lists */
	if (EndFileVFS == NULL) {
		File->Prev = NULL;
		File->Next = NULL;
		StartFileVFS = File;
		EndFileVFS = File;
	}
	else {
		File->Prev = EndFileVFS;
		File->Next = NULL;
		EndFileVFS->Next = File;
		EndFileVFS = File;
	}

	return File;
}

/*
 * Closes the eFILE associated with the stream and disassociates it.
 */
int vw_fclose(eFILE *stream)
{
	if (stream == NULL)
		return -1;

	/* remove from linked lists */
	if (StartFileVFS == stream)
		StartFileVFS = stream->Next;
	if (EndFileVFS == stream)
		EndFileVFS = stream->Prev;

	if (stream->Next != NULL)
		stream->Next->Prev = stream->Prev;
	else if (stream->Prev != NULL)
		stream->Prev->Next = NULL;

	if (stream->Prev != NULL)
		stream->Prev->Next = stream->Next;
	else if (stream->Next != NULL)
		stream->Next->Prev = NULL;

	/* release alocated memory */
	if (stream->Data != NULL) {
		delete [] stream->Data;
		stream->Data = NULL;
	}
	if (stream->Name != NULL) {
		delete [] stream->Name;
		stream->Name = NULL;
	}
	delete stream;

	return 0;
}

/*
 * Reads an array of count elements, each one with a size of size bytes,
 * from the stream and stores them in the block of memory specified by buffer.
 */
int eFILE::fread(void *buffer, size_t size, size_t count)
{
	int CopyCount = 0;

	if ((buffer == NULL) || (Data == 0))
		return -1;

	/* read data */
	for (size_t i=0; i<count; i++) {
		if (size <= (unsigned int)(Size - Pos)) {
			memcpy((BYTE*)buffer + CopyCount*size, Data + Pos, size);
			Pos += size;
			CopyCount++;
		}
		else break; /* end of eFILE */
	}

	return CopyCount;
}

/*
 * Sets the position indicator associated with the stream to a new position.
 */
int eFILE::fseek(long offset, int origin)
{
	switch (origin) {
		case SEEK_CUR:
			if (Pos + offset > Size)
				return 1;
			Pos += offset;
			break;

		case SEEK_END:
			if (Size - offset < 0)
				return 1;
			Pos = Size - offset;
			break;

		case SEEK_SET:
			if (offset < 0 || offset > Size)
				return 1;
			Pos = offset;
			break;

		default:
			fprintf(stderr, "Error in fseek function call, wrong origin.\n");
			break;
	}

	return 0;
}

/*
 * Returns the current value of the position indicator of the stream.
 */
long eFILE::ftell()
{
	return Pos;
}
