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
 On sFILE open, all requested data will be copied into memory buffer (sFILE->Data).
 Opened sFILE is not connected to VFS file or VFS entries list in any way.

 Caller should hold sFILE open as long, as it need memory buffer (sFILE->Data).
 In order to code simplicity, read and write direct access to sFILE data allowed.
 Caller could reset() memory buffer with different size (sFILE->Data), but should
 care about sFILE->Size and sFILE->Pos field (access by fseek()).

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
#include <limits> // need this one for UINT16_MAX only

enum eFileLocation {
	FILE_NOT_FOUND	= -1,
	FILE_IN_VFS	=  1,	// File present in the VFS.
	FILE_IN_FS	=  2	// File present in the File System.
};

struct sVFS {
	std::string FileName;
	SDL_RWops *File{nullptr};

	sVFS(const std::string &_FileName) :
		FileName{_FileName}
	{}
	~sVFS()
	{
		// not sure, if libSDL close SDL_RWops on "out of scope"
		if (File != nullptr)
			SDL_RWclose(File);
	}
};

struct sVFS_Entry {
	uint32_t Offset{0};
	uint32_t Size{0};
	sVFS *Parent{nullptr};

	sVFS_Entry(sVFS *_Parent) :
		Parent{_Parent}
	{}
};

namespace {
// List with connected VFS.
std::forward_list<std::unique_ptr<sVFS>> VFS_List;
// Map with file's entries in VFS.
std::unordered_map<std::string, std::unique_ptr<sVFS_Entry>> VFSEntries_Map;
}


/*
 * Write data from memory into VFS file.
 */
static int WriteIntoVFSfromMemory(sVFS *WritableVFS, const std::string &Name, const std::unique_ptr<uint8_t[]> &Data,
				  uint32_t DataSize, uint32_t *HeaderLengthVFS, uint32_t *HeaderOffsetVFS, uint32_t *DataStartOffsetVFS,
				  std::unordered_map<std::string, std::unique_ptr<sVFS_Entry>> &WritableVFSEntries_Map)
{
	if ((WritableVFS == nullptr) || Name.empty() || (Data.get() == nullptr) ||
	    (DataSize <= 0) || (HeaderLengthVFS == nullptr) ||
	    (HeaderOffsetVFS == nullptr) || (DataStartOffsetVFS == nullptr) ||
	    (Name.size() > UINT16_MAX)) // we should store string size in uint16_t variable
		return ERR_PARAMETERS;

	// push VFS entry to map
	WritableVFSEntries_Map[Name] = std::unique_ptr<sVFS_Entry>(new sVFS_Entry(WritableVFS));
	auto tmpVFSEntry = WritableVFSEntries_Map[Name].get();

	// add new data to VFS file, in this case we could use
	// HeaderOffsetVFS, since this is the end of data part
	tmpVFSEntry->Offset = *HeaderOffsetVFS;
	tmpVFSEntry->Size = DataSize;
	SDL_RWseek(WritableVFS->File, tmpVFSEntry->Offset, SEEK_SET);
	SDL_RWwrite(WritableVFS->File, Data.get(), tmpVFSEntry->Size, 1);

	// write all entries belong to this VFS file
	for (const auto &Tmp : WritableVFSEntries_Map) {
		if (Tmp.second->Parent == WritableVFS) {
			uint16_t tmpNameSize{(uint16_t)(Tmp.first.size())};
			SDL_RWwrite(WritableVFS->File, &tmpNameSize, 2, 1);
			SDL_RWwrite(WritableVFS->File, Tmp.first.c_str(), Tmp.first.size(), 1);
			SDL_RWwrite(WritableVFS->File, &Tmp.second->Offset, 4, 1);
			SDL_RWwrite(WritableVFS->File, &Tmp.second->Size, 4, 1);
		}
	}

	// correct table offset and size
	*HeaderOffsetVFS += DataSize;
	*HeaderLengthVFS += 1 + 2 + Name.size() + 4 + 4 + 4;

	// rewrite table offset and size
	SDL_RWseek(WritableVFS->File, *DataStartOffsetVFS, SEEK_SET);
	SDL_RWwrite(WritableVFS->File, HeaderOffsetVFS, 4, 1);
	SDL_RWwrite(WritableVFS->File, HeaderLengthVFS, 4, 1);

	printf("%s file added to VFS.\n", Name.c_str());
	return 0;
}

/*
 * Write data from file into VFS file.
 */
static int WriteIntoVFSfromFile(sVFS *WritableVFS, const std::string &SrcName, const std::string &DstName,
				uint32_t *HeaderLengthVFS, uint32_t *HeaderOffsetVFS, uint32_t *DataStartOffsetVFS,
				std::unordered_map<std::string, std::unique_ptr<sVFS_Entry>> &WritableVFSEntries_Map)
{
	if ((WritableVFS == nullptr) || SrcName.empty() || DstName.empty() ||
	    (HeaderLengthVFS == nullptr) || (HeaderOffsetVFS == nullptr) ||
	    (DataStartOffsetVFS == nullptr))
		return ERR_PARAMETERS;

	SDL_RWops *Ftmp = SDL_RWFromFile(SrcName.c_str(), "rb");
	if (Ftmp == nullptr) {
		fprintf(stderr, "Can't find file %s\n", SrcName.c_str());
		return ERR_FILE_NOT_FOUND;
	}

	SDL_RWseek(Ftmp, 0, SEEK_END);
	int tmpSize = SDL_RWtell(Ftmp);
	SDL_RWseek(Ftmp, 0, SEEK_SET);

	// copy data to tmp buffer
	std::unique_ptr<uint8_t[]> tmpBuffer(new uint8_t[tmpSize]);
	SDL_RWread(Ftmp, tmpBuffer.get(), tmpSize, 1);
	SDL_RWclose(Ftmp);

	// write into VFS
	int rc = WriteIntoVFSfromMemory(WritableVFS, DstName, tmpBuffer, tmpSize,
					HeaderLengthVFS, HeaderOffsetVFS, DataStartOffsetVFS,
					WritableVFSEntries_Map);
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
	int rc;

	if (Name.empty())
		return ERR_PARAMETERS;

	std::unique_ptr<sVFS> TempVFS(new sVFS(Name));

	TempVFS->File = SDL_RWFromFile(Name.c_str(), "wb");
	if (TempVFS->File == nullptr) {
		fprintf(stderr, "Can't open VFS file for write %s\n", Name.c_str());
		return ERR_FILE_NOT_FOUND;
	}

	// write VFS sign "VFS_", version and build number
	char Sign[4]{'V','F','S','_'};
	SDL_RWwrite(TempVFS->File, Sign, 4, 1);
	SDL_RWwrite(TempVFS->File, VFS_VER, 4, 1);
	SDL_RWwrite(TempVFS->File, &BuildNumber, 4, 1);

	// write new table offset and size
	uint32_t HeaderOffsetVFS{4 + 4 + 4 + 4 + 4}; // VFS_ + ver + build + offset + size
	uint32_t HeaderLengthVFS{0};
	uint32_t DataStartOffsetVFS{4 + 4 + 4}; // VFS_ + ver + build
	SDL_RWwrite(TempVFS->File, &HeaderOffsetVFS, 4, 1);
	SDL_RWwrite(TempVFS->File, &HeaderLengthVFS, 4, 1);

	// we need separate VFS entries map
	std::unordered_map<std::string, std::unique_ptr<sVFS_Entry>> WritableVFSEntries_Map;

	// add model pack files into VFS
	if (!ModelsPack.empty()) {
		// close opened VFS, we need empty VFS related lists
		if (!VFS_List.empty()) {
			fprintf(stderr, "Detected open VFS, close it now.\n");
			vw_ShutdownVFS();
		}

		rc = vw_OpenVFS(RawDataDir + ModelsPack, 0);
		if (rc != 0) {
			fprintf(stderr, "%s file not found or corrupted.\n", (RawDataDir + ModelsPack).c_str());
			return rc;
		}

		// copy all files from pack into new VFS
		for (const auto &TmpVFSEntry : VFSEntries_Map) {
			std::unique_ptr<sFILE> tmpFile = vw_fopen(TmpVFSEntry.first);
			if (tmpFile == nullptr)
				return ERR_FILE_NOT_FOUND;
			rc = WriteIntoVFSfromMemory(TempVFS.get(), TmpVFSEntry.first, tmpFile->Data, tmpFile->Size,
						    &HeaderLengthVFS, &HeaderOffsetVFS, &DataStartOffsetVFS,
						    WritableVFSEntries_Map);
			if (rc != 0) {
				fprintf(stderr, "VFS compilation process aborted!\n");
				return rc;
			}
			vw_fclose(tmpFile);
		}

		// close pack VFS, we don't need it any more
		vw_ShutdownVFS();
	}

	// add real files into VFS
	if (GameDataCount > 0) {
		for (unsigned int i = 0; i < GameDataCount; i++) {
			rc = WriteIntoVFSfromFile(TempVFS.get(), RawDataDir + GameData[i], GameData[i],
						  &HeaderLengthVFS, &HeaderOffsetVFS, &DataStartOffsetVFS,
						  WritableVFSEntries_Map);
			if (rc != 0) {
				fprintf(stderr, "VFS compilation process aborted!\n");
				return rc;
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
	// small trick for replace 'goto' statement usage (for release
	// memory on errors) to named Lambda function
	auto errPrintWithVFSListPop = [&Name] (const std::string &Text, int rc)
	{
		fprintf(stderr, "%s %s\n", Text.c_str(), Name.c_str());
		VFS_List.pop_front();
		return rc;
	};

	// since we can't move unique_ptr into std::forward_list with push_front(),
	// forced to explicitly create the std::unique_ptr in the list
	VFS_List.push_front(std::unique_ptr<sVFS>(new sVFS(Name)));

	VFS_List.front()->File = SDL_RWFromFile(Name.c_str(), "rb");
	if (VFS_List.front()->File == nullptr)
		return errPrintWithVFSListPop("Can't find VFS file", ERR_FILE_NOT_FOUND);

	SDL_RWseek(VFS_List.front()->File,0,SEEK_END);
	int VFS_FileSize = SDL_RWtell(VFS_List.front()->File);
	SDL_RWseek(VFS_List.front()->File,0,SEEK_SET);

	// check VFS file sign "VFS_"
	char Sign[4];
	if(SDL_RWread(VFS_List.front()->File, &Sign, 4, 1) == 0)
		return errPrintWithVFSListPop("VFS file size error", ERR_FILE_IO);
	// Sign don't contain null-terminated string, strncmp() should be used
	if (strncmp(Sign, "VFS_", 4) != 0)
		return errPrintWithVFSListPop("VFS file header error", ERR_FILE_IO);

	// check VFS file version
	char Version[4];
	if(SDL_RWread(VFS_List.front()->File, &Version, 4, 1) == 0)
		return errPrintWithVFSListPop("VFS file corrupted:", ERR_FILE_IO);
	// Version don't contain null-terminated string, strncmp() should be used
	if (strncmp(Version, VFS_VER, 4) != 0)
		return errPrintWithVFSListPop("VFS file has wrong version:", ERR_FILE_IO);

	// check VFS file build number
	unsigned int vfsBuildNumber;
	if(SDL_RWread(VFS_List.front()->File, &vfsBuildNumber, 4, 1) == 0)
		return errPrintWithVFSListPop("VFS file corrupted:", ERR_FILE_IO);
	if (BuildNumber != 0) {
		if (vfsBuildNumber == 0) {
			return errPrintWithVFSListPop("VFS file build number was not set", ERR_VFS_BUILD);
		} else {
			if (BuildNumber != vfsBuildNumber)
				return errPrintWithVFSListPop("VFS file has wrong build number", ERR_VFS_BUILD);
		}
	}

	uint32_t HeaderOffsetVFS;
	uint32_t HeaderLengthVFS;
	SDL_RWread(VFS_List.front()->File, &HeaderOffsetVFS, sizeof(HeaderOffsetVFS), 1);
	SDL_RWread(VFS_List.front()->File, &HeaderLengthVFS, sizeof(HeaderLengthVFS), 1);
	SDL_RWseek(VFS_List.front()->File, HeaderOffsetVFS, SEEK_SET);

	// add entries from new connected VFS file
	uint16_t tmpNameSize{0};
	std::string tmpName;
	while (VFS_FileSize != SDL_RWtell(VFS_List.front()->File)) {
		SDL_RWread(VFS_List.front()->File, &tmpNameSize, 2, 1);
		tmpName.resize(tmpNameSize); // make sure, that we have enough allocated memory for string before SDL_RWread
		SDL_RWread(VFS_List.front()->File, &tmpName[0], tmpNameSize, 1);

		// push VFS entry to map
		VFSEntries_Map[tmpName] = std::unique_ptr<sVFS_Entry>(new sVFS_Entry(VFS_List.front().get()));
		auto tmpVFSEntry = VFSEntries_Map[tmpName].get();

		SDL_RWread(VFS_List.front()->File, &(tmpVFSEntry->Offset), sizeof(tmpVFSEntry->Offset), 1);
		SDL_RWread(VFS_List.front()->File, &(tmpVFSEntry->Size), sizeof(tmpVFSEntry->Size), 1);
	}

	printf("VFS file was opened %s\n", Name.c_str());
	return 0;
}

/*
 * Shutdown VFS.
 */
void vw_ShutdownVFS()
{
	// release all VFS entries
	VFSEntries_Map.clear();

	// release all VFS
	VFS_List.clear();

	printf("All VFS files closed.\n");
}

/*
 * Detect file location.
 */
static eFileLocation DetectFileLocation(const std::string &FileName)
{
	if (FileName.empty())
		return FILE_NOT_FOUND;

	if (VFSEntries_Map.find(FileName) != VFSEntries_Map.end())
		return FILE_IN_VFS;

	// trying to open real file in file system
	SDL_RWops *File = SDL_RWFromFile(FileName.c_str(), "rb");
	if (File != nullptr) {
		SDL_RWclose(File);
		return FILE_IN_FS;
	}

	return FILE_NOT_FOUND;
}

/*
 * Open the sFILE.
 */
std::unique_ptr<sFILE> vw_fopen(const std::string &FileName)
{
	if (FileName.empty())
		return nullptr;

	eFileLocation Location = DetectFileLocation(FileName);

	if (Location == FILE_NOT_FOUND) {
		fprintf(stderr, "Can't find the file %s\n", FileName.c_str());
		return nullptr;
	}

	// initial memory allocation and setup
	std::unique_ptr<sFILE> File(new sFILE(0, 0));

	if (Location == FILE_IN_VFS) {
		// find file in VFS by name
		auto FileInVFS = VFSEntries_Map.find(FileName);

		File->Data.reset(new uint8_t[FileInVFS->second->Size]);
		File->Size = FileInVFS->second->Size;

		SDL_RWseek(FileInVFS->second->Parent->File, FileInVFS->second->Offset, SEEK_SET);
		SDL_RWread(FileInVFS->second->Parent->File, File->Data.get(), FileInVFS->second->Size, 1);
	} else if (Location == FILE_IN_FS) {
		// open real file in file system
		SDL_RWops *fTEMP = SDL_RWFromFile(FileName.c_str(), "rb");

		SDL_RWseek(fTEMP, 0, SEEK_END);
		File->Size = SDL_RWtell(fTEMP);
		SDL_RWseek(fTEMP, 0, SEEK_SET);

		File->Data.reset(new uint8_t[File->Size]);

		SDL_RWread(fTEMP, File->Data.get(), File->Size, 1);
		SDL_RWclose(fTEMP);
	}

	return File;
}

/*
 * You could call vw fclose() if you should release memory in particular
 * part of code. Otherwise, it will be deleted automatically (see. unique_ptr).
 */
int vw_fclose(std::unique_ptr<sFILE> &stream)
{
	if (stream.get() == nullptr)
		return ERR_PARAMETERS;

	// release allocated memory
	stream.reset();

	return 0;
}

/*
 * Reads an array of count elements, each one with a size of size bytes,
 * from the stream and stores them in the block of memory specified by buffer.
 */
int sFILE::fread(void *buffer, size_t size, size_t count)
{
	int CopyCount{0};

	if ((buffer == nullptr) || (Data == nullptr))
		return ERR_PARAMETERS;

	// read data
	for (size_t i = 0; i < count; i++) {
		if (size <= (unsigned int)(Size - Pos)) {
			memcpy((uint8_t *)buffer + CopyCount * size, Data.get() + Pos, size);
			Pos += size;
			CopyCount++;
		} else
			break; // end of sFILE
	}

	return CopyCount;
}

/*
 * Sets the position indicator associated with the stream to a new position.
 */
int sFILE::fseek(long offset, int origin)
{
	switch (origin) {
	case SEEK_CUR:
		if (Pos + offset > Size)
			return ERR_PARAMETERS;
		Pos += offset;
		break;

	case SEEK_END:
		if (Size - offset < 0)
			return ERR_PARAMETERS;
		Pos = Size - offset;
		break;

	case SEEK_SET:
		if (offset < 0 || offset > Size)
			return ERR_PARAMETERS;
		Pos = offset;
		break;

	default:
		fprintf(stderr, "Error in fseek function call, wrong origin.\n");
		return ERR_PARAMETERS;
		break;
	}

	return 0;
}

/*
 * Returns the current value of the position indicator of the stream.
 */
long sFILE::ftell()
{
	return Pos;
}
