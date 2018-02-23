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

/*
 The main VFS concept:
 1. store all game data in one file;
 2. provide unified access to game data (libSDL2 used as backend);
 3. on request, copy game data to memory buffer and care about it.

 On VFS file open, VFS entries list generated with all available in this VFS
 files data. Could be opened multiple VFS files, in this case VFS entries list
 will contain all available in all opened VFS files data.
 On eFILE open, all requested data will be copied into memory buffer (eFILE->Data).
 Opened eFILE is not connected to VFS file or VFS entries list in any way.

 Caller should hold eFILE open as long, as it need memory buffer (eFILE->Data).
 In order to code simplicity, read and write direct access to eFILE data allowed.
 Caller could reset() memory buffer with different size (eFILE->Data), but should
 care about eFILE->Size and eFILE->Pos field (access by fseek()).

 Game data VFS v1.6 structure.

  4b - 'VFS_'
  4b - 'v1.6' (VFS_VER)
  4b - VFS build number (GAME_BUILD)
  4b - file table offset
  4b - file table size
  ?b - data (file data one by one)

  - File table structure
  2b - file name size
  ?b - file name
  4b - file position offset in VFS file
  4b - file size

*/

#include "vfs.h"

enum file_location {
	FILE_NOT_FOUND	= -1,
	FILE_IN_VFS	= 1,	/* File present in the VFS. */
	FILE_IN_FS	= 2 	/* File present in the File System. */
};

struct eVFS {
	std::string	FileName;
	SDL_RWops	*File{nullptr};

	eVFS(const std::string &_FileName) :
		FileName(_FileName)
	{}
	~eVFS()
	{
		/* not sure, if libSDL close SDL_RWops on "out of scope" */
		if (File != nullptr)
			SDL_RWclose(File);
	}
};

struct eVFS_Entry {
	DWORD		NameSize{0};
	std::string	Name;
	int		Offset{0};
	int		Size{0};
	eVFS		*Parent{nullptr};

	eVFS_Entry(eVFS *_Parent) :
		Parent(_Parent)
	{}
	/* trick for forward_list<unique_ptr<T>> work with iterator */
	bool CheckName(const std::string &FileName)
	{
		return (FileName == Name);
	}
};

namespace {
/* List with connected VFS. */
std::forward_list<std::unique_ptr<eVFS>> VFS_List;
/* List with file's entries in VFS. */
std::forward_list<std::unique_ptr<eVFS_Entry>> VFSEntries_List;
}


/*
 * Write data from memory into VFS file.
 */
static int WriteIntoVFSfromMemory(eVFS *WritableVFS, const std::string &Name, const std::unique_ptr<BYTE[]> &buffer,
				  int size, int *HeaderLengthVFS, int *HeaderOffsetVFS, int *DataStartOffsetVFS,
				  std::forward_list<std::unique_ptr<eVFS_Entry>> &WritableVFSEntries_List)
{
	if ((WritableVFS == nullptr) || Name.empty() || (buffer.get() == nullptr) ||
	    (size <= 0) || (HeaderLengthVFS == nullptr) ||
	    (HeaderOffsetVFS == nullptr) || (DataStartOffsetVFS == nullptr))
		return -1;

	/* push VFS entry to list */
	WritableVFSEntries_List.push_front(std::unique_ptr<eVFS_Entry>(new eVFS_Entry(WritableVFS)));

	WritableVFSEntries_List.front()->NameSize = (DWORD)Name.size();
	WritableVFSEntries_List.front()->Name = Name;

	/* add new data to VFS file, in this case we could use
	 * HeaderOffsetVFS, since this is the end of data part
	 */
	WritableVFSEntries_List.front()->Offset = *HeaderOffsetVFS;
	WritableVFSEntries_List.front()->Size = size;
	SDL_RWseek(WritableVFS->File, WritableVFSEntries_List.front()->Offset, SEEK_SET);
	SDL_RWwrite(WritableVFS->File, buffer.get(), WritableVFSEntries_List.front()->Size, 1);

	/* write all entries belong to this VFS file */
	for (auto&& Tmp : WritableVFSEntries_List) {
		if (Tmp->Parent == WritableVFS) {
			SDL_RWwrite(WritableVFS->File, &Tmp->NameSize, 2, 1);
			SDL_RWwrite(WritableVFS->File, Tmp->Name.data(), Tmp->NameSize, 1);
			SDL_RWwrite(WritableVFS->File, &Tmp->Offset, 4, 1);
			SDL_RWwrite(WritableVFS->File, &Tmp->Size, 4, 1);
		}
	}

	/* correct table offset and size */
	*HeaderOffsetVFS += WritableVFSEntries_List.front()->Size;
	*HeaderLengthVFS += 1 + 2 + WritableVFSEntries_List.front()->NameSize + 4 + 4 + 4;

	/* rewrite table offset and size */
	SDL_RWseek(WritableVFS->File, *DataStartOffsetVFS, SEEK_SET);
	SDL_RWwrite(WritableVFS->File, HeaderOffsetVFS, 4, 1);
	SDL_RWwrite(WritableVFS->File, HeaderLengthVFS, 4, 1);

	printf("%s file added to VFS.\n", Name.c_str());
	return 0;
}

/*
 * Write data from file into VFS file.
 */
static int WriteIntoVFSfromFile(eVFS *WritableVFS, const std::string &SrcName, const std::string &DstName,
				int *HeaderLengthVFS, int *HeaderOffsetVFS, int *DataStartOffsetVFS,
				std::forward_list<std::unique_ptr<eVFS_Entry>> &WritableVFSEntries_List)
{
	if ((WritableVFS == nullptr) || SrcName.empty() || DstName.empty() ||
	    (HeaderLengthVFS == nullptr) || (HeaderOffsetVFS == nullptr) ||
	    (DataStartOffsetVFS == nullptr))
		return -1;

	SDL_RWops *Ftmp = SDL_RWFromFile(SrcName.c_str(), "rb");
	if (Ftmp == nullptr) {
		fprintf(stderr, "Can't find file %s\n", SrcName.c_str());
		return -1;
	}

	SDL_RWseek(Ftmp, 0, SEEK_END);
	int tmpSize = SDL_RWtell(Ftmp);
	SDL_RWseek(Ftmp, 0, SEEK_SET);

	/* copy data to tmp buffer */
	std::unique_ptr<BYTE[]> tmpBuffer(new BYTE[tmpSize]);
	SDL_RWread(Ftmp, tmpBuffer.get(), tmpSize, 1);
	SDL_RWclose(Ftmp);

	/* write into VFS */
	int rc = WriteIntoVFSfromMemory(WritableVFS, DstName, tmpBuffer, tmpSize,
					HeaderLengthVFS, HeaderOffsetVFS, DataStartOffsetVFS,
					WritableVFSEntries_List);
	if (rc != 0)
		fprintf(stderr, "Can't write into VFS from memory %s\n", DstName.c_str());

	return rc;
}

/*
 * Create VFS file.
 */
int vw_CreateVFS(const std::string &Name, unsigned int BuildNumber,
		 const std::string &RawDataDir, const std::string &ModelsPack,
		 const std::string GameData[], unsigned int GameDataCount)
{
	if (Name.empty())
		return false;

	std::unique_ptr<eVFS> TempVFS(new eVFS(Name));

	TempVFS->File = SDL_RWFromFile(Name.c_str(), "wb");
	if (TempVFS->File == nullptr) {
		fprintf(stderr, "Can't open VFS file for write %s\n", Name.c_str());
		return -1;
	}

	/* write VFS sign "VFS_", version and build number */
	char Sign[4]{'V','F','S','_'};
	SDL_RWwrite(TempVFS->File, Sign, 4, 1);
	SDL_RWwrite(TempVFS->File, VFS_VER, 4, 1);
	SDL_RWwrite(TempVFS->File, &BuildNumber, 4, 1);

	/* write new table offset and size */
	int HeaderOffsetVFS{4 + 4 + 4 + 4 + 4}; /* VFS_ + ver + build + offset + size */
	int HeaderLengthVFS{0};
	int DataStartOffsetVFS{4 + 4 + 4}; /* VFS_ + ver + build */
	SDL_RWwrite(TempVFS->File, &HeaderOffsetVFS, 4, 1);
	SDL_RWwrite(TempVFS->File, &HeaderLengthVFS, 4, 1);

	/* we need separate VFS entries list */
	std::forward_list<std::unique_ptr<eVFS_Entry>> WritableVFSEntries_List;

	/* add model pack files into VFS */
	if (!ModelsPack.empty()) {
		/* close opened VFS, we need empty VFS related lists */
		if (!VFS_List.empty()) {
			fprintf(stderr, "Detected open VFS, close it now.\n");
			vw_ShutdownVFS();
		}

		if (vw_OpenVFS(RawDataDir + ModelsPack, 0) != 0) {
			fprintf(stderr, "%s file not found or corrupted.\n", (RawDataDir + ModelsPack).c_str());
			return -1;
		}

		/* copy all files from pack into new VFS,
		 * a bit tricky, since we can't work with iterator for
		 * forward_list<unique_ptr<T>> in usual way
		 */
		for (auto&& TmpVFSEntry : VFSEntries_List) {
			std::unique_ptr<eFILE> tmpFile = vw_fopen(TmpVFSEntry->Name);
			if (tmpFile == nullptr)
				return -1;
			if (0 != WriteIntoVFSfromMemory(TempVFS.get(), TmpVFSEntry->Name, tmpFile->Data, tmpFile->Size,
							&HeaderLengthVFS, &HeaderOffsetVFS, &DataStartOffsetVFS,
							WritableVFSEntries_List)) {
				fprintf(stderr, "VFS compilation process aborted!\n");
				return -1;
			}
			vw_fclose(tmpFile);
		}

		/* close pack VFS, we don't need it any more */
		vw_ShutdownVFS();
	}

	/* add real files into VFS */
	if (GameDataCount > 0) {
		for (unsigned int i = 0; i < GameDataCount; i++) {
			if (0 != WriteIntoVFSfromFile(TempVFS.get(), RawDataDir + GameData[i], GameData[i],
						      &HeaderLengthVFS, &HeaderOffsetVFS, &DataStartOffsetVFS,
						      WritableVFSEntries_List)) {
				fprintf(stderr, "VFS compilation process aborted!\n");
				return -1;
			}
		}
	}

	printf("VFS file was created %s\n", Name.c_str());
	return 0;
}

/*
 * Open VFS file.
 */
int vw_OpenVFS(const std::string &Name, unsigned int BuildNumber)
{
	/* small trick for replace 'goto' statement usage (for release
	 * memory on errors) to Lambda expression
	 */
	auto errPrintWithVFSListPop = [&Name] (const std::string &Text)
	{
		fprintf(stderr, (Text + " " + Name + "\n").c_str());
		VFS_List.pop_front();
		return -1;
	};

	/* since we can't move unique_ptr into std::forward_list with push_front(),
	 * forced to explicitly create the std::unique_ptr in the list
	 */
	VFS_List.push_front(std::unique_ptr<eVFS>(new eVFS(Name)));

	VFS_List.front()->File = SDL_RWFromFile(Name.c_str(), "rb");
	if (VFS_List.front()->File == nullptr)
		return errPrintWithVFSListPop("Can't find VFS file");

	SDL_RWseek(VFS_List.front()->File,0,SEEK_END);
	int VFS_FileSize = SDL_RWtell(VFS_List.front()->File);
	SDL_RWseek(VFS_List.front()->File,0,SEEK_SET);

	/* check VFS file sign "VFS_" */
	char Sign[4];
	if(SDL_RWread(VFS_List.front()->File, &Sign, 4, 1) == 0)
		return errPrintWithVFSListPop("VFS file size error");
	/* Sign don't contain null-terminated string, strncmp() should be used */
	if (strncmp(Sign, "VFS_", 4) != 0)
		return errPrintWithVFSListPop("VFS file header error");

	/* check VFS file version */
	char Version[4];
	if(SDL_RWread(VFS_List.front()->File, &Version, 4, 1) == 0)
		return errPrintWithVFSListPop("VFS file corrupted:");
	/* Version don't contain null-terminated string, strncmp() should be used */
	if (strncmp(Version, VFS_VER, 4) != 0)
		return errPrintWithVFSListPop("VFS file has wrong version:");

	/* check VFS file build number */
	unsigned int vfsBuildNumber;
	if(SDL_RWread(VFS_List.front()->File, &vfsBuildNumber, 4, 1) == 0)
		return errPrintWithVFSListPop("VFS file corrupted:");
	if (BuildNumber != 0) {
		if (vfsBuildNumber == 0) {
			printf("VFS file build number was not set (0).\n");
		} else {
			if (BuildNumber != vfsBuildNumber)
				return errPrintWithVFSListPop("VFS file has wrong build number");
		}
	}

	int HeaderOffsetVFS;
	int HeaderLengthVFS;
	SDL_RWread(VFS_List.front()->File, &HeaderOffsetVFS, sizeof(HeaderOffsetVFS), 1);
	SDL_RWread(VFS_List.front()->File, &HeaderLengthVFS, sizeof(HeaderLengthVFS), 1);
	SDL_RWseek(VFS_List.front()->File, HeaderOffsetVFS, SEEK_SET);

	/* add entries from new connected VFS file */
	while (VFS_FileSize != SDL_RWtell(VFS_List.front()->File)) {
		/* push VFS entry to list */
		VFSEntries_List.push_front(std::unique_ptr<eVFS_Entry>(new eVFS_Entry(VFS_List.front().get())));

		SDL_RWread(VFS_List.front()->File, &VFSEntries_List.front()->NameSize, 2, 1);
		VFSEntries_List.front()->Name.resize(VFSEntries_List.front()->NameSize);
		SDL_RWread(VFS_List.front()->File, &(VFSEntries_List.front()->Name[0]), VFSEntries_List.front()->NameSize, 1);
		/* just to be sure, that we have null-terminated string */
		VFSEntries_List.front()->Name[VFSEntries_List.front()->NameSize] = '\0';
		SDL_RWread(VFS_List.front()->File, &(VFSEntries_List.front()->Offset), sizeof(VFSEntries_List.front()->Offset), 1);
		SDL_RWread(VFS_List.front()->File, &(VFSEntries_List.front()->Size), sizeof(VFSEntries_List.front()->Size), 1);
	}

	printf("VFS file was opened %s\n", Name.c_str());
	return 0;
}

/*
 * Shutdown VFS.
 */
void vw_ShutdownVFS()
{
	/* release all VFS entries */
	VFSEntries_List.clear();

	/* release all VFS */
	VFS_List.clear();

	printf("All VFS files closed.\n");
}

/*
 * Detect file location.
 */
static file_location DetectFileLocation(const std::string &FileName)
{
	if (FileName.empty())
		return FILE_NOT_FOUND;

	/* trying to find file in VFS by name,
	 * a bit tricky, since we can't work with iterator for
	 * forward_list<unique_ptr<T>> in usual way
	 */
	for (auto&& Tmp : VFSEntries_List) {
		if (Tmp->CheckName(FileName))
			return FILE_IN_VFS;
	}

	/* trying to open real file in file system */
	SDL_RWops *File = SDL_RWFromFile(FileName.c_str(), "rb");
	if (File != nullptr) {
		SDL_RWclose(File);
		return FILE_IN_FS;
	}

	return FILE_NOT_FOUND;
}

/*
 * Open the eFILE.
 */
std::unique_ptr<eFILE> vw_fopen(const std::string &FileName)
{
	if (FileName.empty())
		return nullptr;

	file_location Location = DetectFileLocation(FileName);

	if (Location == FILE_NOT_FOUND) {
		fprintf(stderr, "Can't find the file %s\n", FileName.c_str());
		return nullptr;
	}

	/* initial memory allocation and setup */
	std::unique_ptr<eFILE> File(new eFILE(0, 0));

	if (Location == FILE_IN_VFS) {
		/* find file in VFS by name */
		for (auto&& FileInVFS : VFSEntries_List) {
			/* a bit tricky, since we can't work with iterator for
			 * forward_list<unique_ptr<T>> in usual way
			 */
			if (FileInVFS->CheckName(FileName)) {
				File->Data.reset(new BYTE[FileInVFS->Size]);
				File->Size = FileInVFS->Size;

				SDL_RWseek(FileInVFS->Parent->File, FileInVFS->Offset, SEEK_SET);
				SDL_RWread(FileInVFS->Parent->File, File->Data.get(), FileInVFS->Size, 1);

				break;
			}
		}
	} else if (Location == FILE_IN_FS) {
		/* open real file in file system */
		SDL_RWops * fTEMP = SDL_RWFromFile(FileName.c_str(), "rb");

		SDL_RWseek(fTEMP, 0, SEEK_END);
		File->Size = SDL_RWtell(fTEMP);
		SDL_RWseek(fTEMP, 0, SEEK_SET);

		File->Data.reset(new BYTE[File->Size]);

		SDL_RWread(fTEMP, File->Data.get(), File->Size, 1);
		SDL_RWclose(fTEMP);
	}

	return File;
}

/*
 * We don't really close stream, but release memory.
 */
int vw_fclose(std::unique_ptr<eFILE> &stream)
{
	if (stream.get() == nullptr)
		return -1;

	/* release allocated memory */
	stream.reset();

	return 0;
}

/*
 * Reads an array of count elements, each one with a size of size bytes,
 * from the stream and stores them in the block of memory specified by buffer.
 */
int eFILE::fread(void *buffer, size_t size, size_t count)
{
	int CopyCount{0};

	if ((buffer == nullptr) || (Data == nullptr))
		return -1;

	/* read data */
	for (size_t i = 0; i < count; i++) {
		if (size <= (unsigned int)(Size - Pos)) {
			memcpy((BYTE*)buffer + CopyCount*size, Data.get() + Pos, size);
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
