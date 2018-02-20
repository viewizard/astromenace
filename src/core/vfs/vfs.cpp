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

/* List with connected VFS. */
std::forward_list<eVFS*> VFS_List;
/* List with file's entries in VFS. */
std::forward_list<eVFS_Entry*> VFSEntries_List;
/* List with opened files. */
std::forward_list<eFILE*> VFSFiles_List;

enum file_location {
	FILE_NOT_FOUND = -1,
	FILE_IN_VFS = 1, /* File present in the VFS. */
	FILE_IN_FS = 2 /* File present in the File System. */
};


/*
 * Get VFS entries list.
 */
std::forward_list<eVFS_Entry*> *vw_GetVFSEntriesListHead()
{
	return &VFSEntries_List;
}

/*
 * Create VFS file.
 */
eVFS *vw_CreateVFS(const char *Name, unsigned int BuildNumber)
{
	if (Name == nullptr)
		return nullptr;

	eVFS *TempVFS = new eVFS;

	TempVFS->HeaderLengthVFS = 0;
	TempVFS->HeaderOffsetVFS = 0;
	TempVFS->DataStartOffsetVFS = 0;
	TempVFS->Writable = true;

	TempVFS->File = SDL_RWFromFile(Name, "wb");
	if (TempVFS->File == nullptr) {
		fprintf(stderr, "Can't open VFS file for write %s\n", Name);
		delete TempVFS;
		return nullptr;
	}

	TempVFS->FileName = new char[strlen(Name)+1];
	strcpy(TempVFS->FileName, Name);

	/* push VFS to list */
	VFS_List.push_front(TempVFS);

	/* write VFS sign, version and build number */
	char tmp1[5] = "VFS_";
	SDL_RWwrite(TempVFS->File, tmp1, 4, 1);
	SDL_RWwrite(TempVFS->File, VFS_VER, 4, 1);
	SDL_RWwrite(TempVFS->File, &BuildNumber, 4, 1);

	/* write new table offset and size */
	TempVFS->HeaderOffsetVFS = 4 + 4 + 4 + 4 + 4; /* VFS_ + ver + build + offset + size */
	TempVFS->DataStartOffsetVFS = 4 + 4 + 4; /* VFS_ + ver + build */
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
	if ((SrcName == nullptr) || (DstName == nullptr))
		return -1;

	// читаем данные файла в буфер
	SDL_RWops *Ftmp = SDL_RWFromFile(SrcName, "rb");
	// Если файл не найден - выходим
	if (Ftmp == nullptr) {
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
	if ((WritableVFS == nullptr) || (Name == nullptr) ||
	    (buffer == nullptr) || (size <= 0))
		return -1;

	if (!WritableVFS->Writable) {
		fprintf(stderr, "Can't write into VFS, VFS not writable %s\n", Name);
		return -1;
	}

	eVFS_Entry *NewVFS_Entry = new eVFS_Entry;

	/* push VFS entry to list */
	VFSEntries_List.push_front(NewVFS_Entry);

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
	for (eVFS_Entry *Tmp : VFSEntries_List) {
		if (Tmp->Parent == WritableVFS) {
			SDL_RWwrite(WritableVFS->File, &Tmp->NameSize, 2, 1);
			SDL_RWwrite(WritableVFS->File, Tmp->Name, Tmp->NameSize, 1);
			SDL_RWwrite(WritableVFS->File, &Tmp->Offset, 4, 1);
			SDL_RWwrite(WritableVFS->File, &Tmp->Size, 4, 1);
		}
	}

	/* correct table offset and size */
	WritableVFS->HeaderOffsetVFS += NewVFS_Entry->Size;
	WritableVFS->HeaderLengthVFS += 1 + 2 + NewVFS_Entry->NameSize + 4 + 4 + 4;

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
	char Sign[5];
	char Version[5];

	TempVFS = new eVFS;
	TempVFS->Writable = false;

	TempVFS->File = SDL_RWFromFile(Name, "rb");
	if (TempVFS->File == nullptr) {
		fprintf(stderr, "Can't find VFS file %s\n", Name);
		goto error;
	}

	SDL_RWseek(TempVFS->File,0,SEEK_END);
	VFS_FileSize = SDL_RWtell(TempVFS->File);
	SDL_RWseek(TempVFS->File,0,SEEK_SET);

	TempVFS->FileName = new char[strlen(Name)+1];
	strcpy(TempVFS->FileName, Name);

	/* check VFS file sign "VFS_" */
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
		} else {
			if (BuildNumber != vfsBuildNumber) {
				fprintf(stderr,
					"VFS file has wrong build number (%u), you need VFS with build number %u\n",
					vfsBuildNumber, BuildNumber);
				goto error;
			}
		}
	}

	/* all looks legit, push VFS to list */
	VFS_List.push_front(TempVFS);

	SDL_RWread(TempVFS->File, &HeaderOffsetVFS, sizeof(HeaderOffsetVFS), 1);
	SDL_RWread(TempVFS->File, &HeaderLengthVFS, sizeof(HeaderLengthVFS), 1);
	SDL_RWseek(TempVFS->File, HeaderOffsetVFS, SEEK_SET);

	/* add entries from new connected VFS file */
	while (VFS_FileSize != SDL_RWtell(TempVFS->File)) {
		eVFS_Entry *Temp = new eVFS_Entry;

		/* push VFS entry to list */
		VFSEntries_List.push_front(Temp);

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
	if (TempVFS->File != nullptr)
		SDL_RWclose(TempVFS->File);
	if (TempVFS->FileName != nullptr)
		delete [] TempVFS->FileName;
	delete TempVFS;

	return -1;
}

/*
 * Close all VFS.
 */
void vw_CloseVFS()
{
	/* release all VFS entries */
	while (!VFSEntries_List.empty()) {

		if (VFSEntries_List.front()->Name != nullptr)
			delete [] VFSEntries_List.front()->Name;

		delete VFSEntries_List.front();
		VFSEntries_List.pop_front();
	}
	/* reset list */
	VFSEntries_List.clear();

	/* release all VFS */
	while (!VFS_List.empty()) {

		if (VFS_List.front()->File != nullptr)
			SDL_RWclose(VFS_List.front()->File);

		if (VFS_List.front()->FileName != nullptr) {
			printf("VFS file was closed %s\n", VFS_List.front()->FileName);
			delete [] VFS_List.front()->FileName;
		}

		delete VFS_List.front();
		VFS_List.pop_front();
	}
	/* reset list */
	VFS_List.clear();
}

/*
 * Shutdown VFS, close all opened files.
 */
void vw_ShutdownVFS()
{
	/* close all files */
	while (!VFSFiles_List.empty()) {
		vw_fclose(VFSFiles_List.front());
	}
	/* reset list */
	VFSFiles_List.clear();

	vw_CloseVFS();
}

/*
 * Detect file location.
 */
static file_location DetectFileLocation(const char *FileName)
{
	if (FileName == nullptr)
		return FILE_NOT_FOUND;

	/* trying to find file in VFS by name */
	for (eVFS_Entry *Tmp : VFSEntries_List) {
		if(strcmp(FileName, Tmp->Name) == 0)
			return FILE_IN_VFS;
	}

	/* trying to open real file in file system */
	SDL_RWops *File = SDL_RWFromFile(FileName, "rb");
	if (File != nullptr) {
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
	if (FileName == nullptr)
		return nullptr;

	file_location Location = DetectFileLocation(FileName);

	if (Location == FILE_NOT_FOUND) {
		fprintf(stderr, "Can't find the file %s\n", FileName);
		return nullptr;
	}

	/* initial memory allocation and setup */
	eFILE *File = new eFILE;
	File->Pos = 0;
	File->Size = 0;
	File->VFS_Offset = 0;
	File->Name = new char[strlen(FileName)+1];
	strcpy(File->Name, FileName);

	if (Location == FILE_IN_VFS) {
		/* find file in VFS by name */
		eVFS_Entry *FileInVFS = nullptr;
		for (eVFS_Entry *Tmp : VFSEntries_List) {
			if(strcmp(File->Name, Tmp->Name) == 0) {
				FileInVFS = Tmp;
				break;
			}
		}
		/* no need to check FileInVFS, since we know for sure,
		 * file present in VFS
		 */
		File->VFS_Offset = FileInVFS->Offset;
		File->Data = new BYTE[FileInVFS->Size];
		File->Size = FileInVFS->Size;

		SDL_RWseek(FileInVFS->Parent->File, File->VFS_Offset, SEEK_SET);
		SDL_RWread(FileInVFS->Parent->File, File->Data, FileInVFS->Size, 1);
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

	/* add file to list */
	VFSFiles_List.push_front(File);

	return File;
}

/*
 * Closes the eFILE associated with the stream and disassociates it.
 */
int vw_fclose(eFILE *stream)
{
	if (stream == nullptr)
		return -1;

	/* remove from list */
	VFSFiles_List.remove(stream);

	/* release alocated memory */
	if (stream->Data != nullptr)
		delete [] stream->Data;
	if (stream->Name != nullptr)
		delete [] stream->Name;
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

	if ((buffer == nullptr) || (Data == nullptr))
		return -1;

	/* read data */
	for (size_t i=0; i<count; i++) {
		if (size <= (unsigned int)(Size - Pos)) {
			memcpy((BYTE*)buffer + CopyCount*size, Data + Pos, size);
			Pos += size;
			CopyCount++;
		} else
			break; /* end of eFILE */
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
