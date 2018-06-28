/************************************************************************************

	AstroMenace
	Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
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
	along with AstroMenace. If not, see <https://www.gnu.org/licenses/>.


	Website: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

// TODO SDL2 operate with 64bit offsets (long long, since C++11), see SDL_RWtell()
//      move to 64bit and remove all related static_cast-s

// NOTE in future, use make_unique() to make unique_ptr-s (since C++14)

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
  ?b - data (file data one by one)

  - File table structure
  2b - file name size
  ?b - file name
  4b - file position offset in VFS file
  4b - file size
*/

/*
Warning!

VFS code could be used before SDL_Init() and SDL_InitSubSystem(),
make sure, that only file I/O SDL subsystem are used in VFS code.

From SDL wiki (https://wiki.libsdl.org/SDL_Init):
The file I/O and threading subsystems are initialized by default.
You must specifically initialize other subsystems if you use them in your application.
Logging works without initialization, too.
*/

#include "vfs.h"
#include <limits> // need this one for UINT16_MAX only

namespace viewizard {

struct sVFS {
	std::string FileName;
	SDL_RWops *File{nullptr};

	explicit sVFS(const std::string &_FileName) :
		FileName{_FileName}
	{}
	~sVFS()
	{
		// not sure, if libSDL close SDL_RWops on "out of scope"
		if (File)
			SDL_RWclose(File);
	}
};

struct sVFS_Entry {
	uint32_t Offset{0};
	uint32_t Size{0};
	sVFS *Parent{nullptr};

	explicit sVFS_Entry(sVFS *_Parent) :
		Parent{_Parent}
	{}
};

namespace {

// List with all opened VFS.
std::forward_list<std::unique_ptr<sVFS>> VFSList;
// Map with file's entries in all opened VFS.
std::unordered_map<std::string, std::unique_ptr<sVFS_Entry>> VFSEntriesMap;

constexpr unsigned int FixedHeaderPartSize = 4 + 4 + 4; /*VFS_ + ver + build*/

} // unnamed namespace


/*
 * Write data from memory into VFS file.
 */
static int WriteIntoVFSfromMemory(sVFS *WritableVFS, const std::string &Name, const uint8_t *DataBuffer,
				  uint32_t DataSize, uint32_t &FileTableOffset, std::unordered_map<std::string,
				  std::unique_ptr<sVFS_Entry>> &WritableVFSEntriesMap)
{
	if (!WritableVFS || Name.empty() || !DataBuffer ||
	    (DataSize <= 0) || (Name.size() > UINT16_MAX)) // UINT16_MAX - we should store string size in uint16_t variable
		return ERR_PARAMETERS;

	// push VFS entry into the map
	WritableVFSEntriesMap[Name] = std::unique_ptr<sVFS_Entry>(new sVFS_Entry(WritableVFS));

	// add new data to VFS file, in this case we could use
	// FileTableOffset, since this is the end of data part
	WritableVFSEntriesMap[Name]->Offset = FileTableOffset;
	WritableVFSEntriesMap[Name]->Size = DataSize;
	SDL_RWseek(WritableVFS->File, WritableVFSEntriesMap[Name]->Offset, SEEK_SET);
	SDL_RWwrite(WritableVFS->File, DataBuffer, WritableVFSEntriesMap[Name]->Size, 1);

	// write all entries belong to this VFS file
	for (const auto &tmpEntry : WritableVFSEntriesMap) {
		if (tmpEntry.second->Parent == WritableVFS) {
			uint16_t tmpNameSize{(uint16_t)(tmpEntry.first.size())};
			SDL_RWwrite(WritableVFS->File, &tmpNameSize, sizeof(tmpNameSize), 1);
			SDL_RWwrite(WritableVFS->File, tmpEntry.first.c_str(), tmpEntry.first.size(), 1);
			SDL_RWwrite(WritableVFS->File, &tmpEntry.second->Offset, sizeof(tmpEntry.second->Offset), 1);
			SDL_RWwrite(WritableVFS->File, &tmpEntry.second->Size, sizeof(tmpEntry.second->Size), 1);
		}
	}

	SDL_RWseek(WritableVFS->File, FixedHeaderPartSize, SEEK_SET);
	FileTableOffset += DataSize;
	SDL_RWwrite(WritableVFS->File, &FileTableOffset, sizeof(FileTableOffset), 1);

	std::cout << Name << " file added to VFS.\n";
	return 0;
}

/*
 * Write data from file into VFS file.
 */
static int WriteIntoVFSfromFile(sVFS *WritableVFS, const std::string &SrcName, const std::string &DstName,
				uint32_t &FileTableOffset, std::unordered_map<std::string,
				std::unique_ptr<sVFS_Entry>> &WritableVFSEntriesMap)
{
	if (!WritableVFS || SrcName.empty() || DstName.empty())
		return ERR_PARAMETERS;

	SDL_RWops *tmpFile = SDL_RWFromFile(SrcName.c_str(), "rb");
	if (!tmpFile) {
		std::cerr << __func__ << "(): " << "Can't find file " << SrcName << "\n";
		return ERR_FILE_NOT_FOUND;
	}

	SDL_RWseek(tmpFile, 0, SEEK_END);
	// we don't use >2GB VFS files, so, we ok here with static_cast
	int tmpFileSize = static_cast<int>(SDL_RWtell(tmpFile));
	SDL_RWseek(tmpFile, 0, SEEK_SET);

	// std::unique_ptr, we need only memory allocation without container's features
	// don't use std::vector here, since it allocates AND value-initializes
	std::unique_ptr<uint8_t[]> tmpBuffer(new uint8_t[tmpFileSize]);
	SDL_RWread(tmpFile, tmpBuffer.get(), tmpFileSize, 1);
	SDL_RWclose(tmpFile);

	int err = WriteIntoVFSfromMemory(WritableVFS, DstName, tmpBuffer.get(), tmpFileSize,
					 FileTableOffset, WritableVFSEntriesMap);
	if (err)
		std::cerr << __func__ << "(): " << "Can't write into VFS from memory " << DstName << "\n";

	return err;
}

/*
 * Create VFS file.
 */
int vw_CreateVFS(const std::string &Name, unsigned int BuildNumber,
		 const std::string &RawDataDir, const std::string &ModelsPack,
		 const std::string GameData[], unsigned int GameDataCount)
{
	if (Name.empty())
		return ERR_PARAMETERS;

	std::unique_ptr<sVFS> TempVFS(new sVFS(Name));

	TempVFS->File = SDL_RWFromFile(Name.c_str(), "wb");
	if (!TempVFS->File) {
		std::cerr << __func__ << "(): " << "Can't open VFS file for write " << Name << "\n";
		return ERR_FILE_NOT_FOUND;
	}

	// write VFS sign "VFS_", version and build number
	constexpr char Sign[4]{'V','F','S','_'};
	SDL_RWwrite(TempVFS->File, Sign, 4 /*fixed 4 bites size*/, 1);
	SDL_RWwrite(TempVFS->File, VFS_VER, 4 /*fixed 4 bites size*/, 1);
	SDL_RWwrite(TempVFS->File, &BuildNumber, 4 /*fixed 4 bites size*/, 1);

	// new file table offset
	uint32_t FileTableOffset{FixedHeaderPartSize + sizeof(FileTableOffset)};
	SDL_RWwrite(TempVFS->File, &FileTableOffset, sizeof(FileTableOffset), 1);

	// we need separate VFS entries map
	std::unordered_map<std::string, std::unique_ptr<sVFS_Entry>> WritableVFSEntriesMap;

	// add model pack files into VFS
	if (!ModelsPack.empty()) {
		// close opened VFS, we need empty VFS related lists
		if (!VFSList.empty()) {
			std::cerr << __func__ << "(): " << "Detected open VFS, close it now.\n";
			vw_ShutdownVFS();
		}

		int err = vw_OpenVFS(RawDataDir + ModelsPack, 0);
		if (err) {
			std::cerr << __func__ << "(): " << RawDataDir + ModelsPack << " file not found or corrupted.\n";
			return err;
		}

		// copy all files from pack into new VFS
		for (const auto &tmpVFSEntry : VFSEntriesMap) {
			std::unique_ptr<sFILE> tmpFile = vw_fopen(tmpVFSEntry.first);
			if (!tmpFile)
				return ERR_FILE_NOT_FOUND;
			err = WriteIntoVFSfromMemory(TempVFS.get(), tmpVFSEntry.first, tmpFile->Data.get(),
						     tmpFile->Size, FileTableOffset, WritableVFSEntriesMap);
			if (err) {
				std::cerr << __func__ << "(): " << "VFS compilation process aborted!\n";
				return err;
			}
			vw_fclose(tmpFile);
		}

		// close pack VFS, we don't need it any more
		vw_ShutdownVFS();
	}

	// unconditional rehash, at this line we have not rehashed map
	WritableVFSEntriesMap.rehash(0);

	// add real files into VFS
	if (GameDataCount > 0) {
		for (unsigned int i = 0; i < GameDataCount; i++) {
			int err = WriteIntoVFSfromFile(TempVFS.get(), RawDataDir + GameData[i], GameData[i],
						       FileTableOffset, WritableVFSEntriesMap);
			if (err) {
				std::cerr << __func__ << "(): " << "VFS compilation process aborted!\n";
				return err;
			}
		}
	}

	std::cout << "VFS file was created " << Name << "\n";
	return 0;
}

/*
 * Open VFS file.
 */
int vw_OpenVFS(const std::string &Name, unsigned int BuildNumber)
{
	// small trick for replace 'goto' statement usage (for release
	// memory on errors) to named Lambda function
	auto errPrintWithVFSListPop = [&Name] (const std::string &Text, int err)
	{
		std::cerr << "vw_OpenVFS(): " << Text << " " << Name << "\n";
		VFSList.pop_front();
		return err;
	};

	// since we can't move unique_ptr into std::forward_list with push_front(),
	// forced to explicitly create the std::unique_ptr in the list
	VFSList.push_front(std::unique_ptr<sVFS>(new sVFS(Name)));

	VFSList.front()->File = SDL_RWFromFile(Name.c_str(), "rb");
	if (!VFSList.front()->File)
		return errPrintWithVFSListPop("Can't find VFS file", ERR_FILE_NOT_FOUND);

	SDL_RWseek(VFSList.front()->File,0,SEEK_END);
	// we don't use >2GB VFS files, so, we ok here with static_cast
	int VFS_FileSize = static_cast<int>(SDL_RWtell(VFSList.front()->File));
	SDL_RWseek(VFSList.front()->File,0,SEEK_SET);

	// check VFS file sign "VFS_"
	char Sign[4];
	if(!SDL_RWread(VFSList.front()->File, &Sign, 4 /*fixed 4 bites size*/, 1))
		return errPrintWithVFSListPop("VFS file size error", ERR_FILE_IO);
	// Sign don't contain null-terminated string, strncmp() should be used
	if (strncmp(Sign, "VFS_", 4) != 0)
		return errPrintWithVFSListPop("VFS file header error", ERR_FILE_IO);

	// check VFS file version
	char Version[4];
	if(!SDL_RWread(VFSList.front()->File, &Version, 4 /*fixed 4 bites size*/, 1))
		return errPrintWithVFSListPop("VFS file corrupted:", ERR_FILE_IO);
	// Version don't contain null-terminated string, strncmp() should be used
	if (strncmp(Version, VFS_VER, 4) != 0)
		return errPrintWithVFSListPop("VFS file has wrong version:", ERR_FILE_IO);

	// check VFS file build number
	unsigned int vfsBuildNumber;
	if(!SDL_RWread(VFSList.front()->File, &vfsBuildNumber, 4 /*fixed 4 bites size*/, 1))
		return errPrintWithVFSListPop("VFS file corrupted:", ERR_FILE_IO);
	if (BuildNumber) {
		if (vfsBuildNumber) {
			if (BuildNumber != vfsBuildNumber)
				return errPrintWithVFSListPop("VFS file has wrong build number", ERR_VFS_BUILD);
		} else
			return errPrintWithVFSListPop("VFS file build number was not set", ERR_VFS_BUILD);
	}

	uint32_t FileTableOffset;
	SDL_RWread(VFSList.front()->File, &FileTableOffset, sizeof(FileTableOffset), 1);
	SDL_RWseek(VFSList.front()->File, FileTableOffset, SEEK_SET);

	// add entries from new connected VFS file
	while (VFS_FileSize != SDL_RWtell(VFSList.front()->File)) {
		uint16_t tmpNameSize;
		SDL_RWread(VFSList.front()->File, &tmpNameSize, sizeof(tmpNameSize), 1);

		std::string tmpName;
		tmpName.resize(tmpNameSize); // make sure, that we have enough allocated memory for string before SDL_RWread
		SDL_RWread(VFSList.front()->File, &tmpName[0], tmpNameSize, 1);

		VFSEntriesMap[tmpName] = std::unique_ptr<sVFS_Entry>(new sVFS_Entry(VFSList.front().get()));

		SDL_RWread(VFSList.front()->File, &(VFSEntriesMap[tmpName]->Offset), sizeof(VFSEntriesMap[tmpName]->Offset), 1);
		SDL_RWread(VFSList.front()->File, &(VFSEntriesMap[tmpName]->Size), sizeof(VFSEntriesMap[tmpName]->Size), 1);
	}

	// unconditional rehash, at this line we have not rehashed map
	VFSEntriesMap.rehash(0);

	std::cout << "VFS file was opened " << Name << "\n";
	return 0;
}

/*
 * Get internal VFS state for data extraction.
 */
int vw_GetInternalDataStateInVFS(const std::string &FileName, SDL_RWops *&VFSFile,
				 uint32_t &DataOffset, uint32_t &DataSize)
{
	if (FileName.empty())
		return ERR_PARAMETERS;

	auto FileInVFS = VFSEntriesMap.find(FileName);

	if (FileInVFS == VFSEntriesMap.end())
		return ERR_FILE_NOT_FOUND;

	VFSFile = FileInVFS->second->Parent->File;
	DataOffset = FileInVFS->second->Offset;
	DataSize = FileInVFS->second->Size;

	return 0;
}

/*
 * Shutdown VFS.
 */
void vw_ShutdownVFS()
{
	// release all VFS entries
	VFSEntriesMap.clear();

	// release all VFS
	VFSList.clear();

	std::cout << "All VFS files closed.\n";
}

/*
 * Detect file location.
 */
eFileLocation vw_DetectFileLocation(const std::string &FileName)
{
	if (FileName.empty())
		return eFileLocation::Unknown;

	if (VFSEntriesMap.find(FileName) != VFSEntriesMap.end())
		return eFileLocation::VFS;

	// trying to open real file in file system
	SDL_RWops *File = SDL_RWFromFile(FileName.c_str(), "rb");
	if (File) {
		SDL_RWclose(File);
		return eFileLocation::FS;
	}

	return eFileLocation::Unknown;
}

/*
 * Open the sFILE.
 * Return std::unique_ptr, provide smart pointer connected to caller's scope.
 */
std::unique_ptr<sFILE> vw_fopen(const std::string &FileName)
{
	if (FileName.empty())
		return nullptr;

	// initial memory allocation and setup
	SDL_RWops *tmpFile{nullptr};
	uint32_t DataOffset{0}, DataSize{0};
	std::unique_ptr<sFILE> File(new sFILE(0, 0));

	switch (vw_DetectFileLocation(FileName)) {
	case eFileLocation::VFS:
		if (vw_GetInternalDataStateInVFS(FileName, tmpFile, DataOffset, DataSize))
			return nullptr;

		File->Size = DataSize;
		File->Data.reset(new uint8_t[File->Size]);
		SDL_RWseek(tmpFile, DataOffset, SEEK_SET);
		SDL_RWread(tmpFile, File->Data.get(), File->Size, 1);
		break;

	case eFileLocation::FS:
		tmpFile = SDL_RWFromFile(FileName.c_str(), "rb");
		if (!tmpFile)
			return nullptr;

		SDL_RWseek(tmpFile, 0, SEEK_END);
		// we don't use >2GB VFS files, so, we ok here with static_cast
		if (SDL_RWtell(tmpFile) > 0)
			File->Size = static_cast<uint32_t>(SDL_RWtell(tmpFile));
		else
			return nullptr;
		SDL_RWseek(tmpFile, 0, SEEK_SET);

		File->Data.reset(new uint8_t[File->Size]);

		SDL_RWread(tmpFile, File->Data.get(), File->Size, 1);
		SDL_RWclose(tmpFile);
		break;

	case eFileLocation::Unknown:
		std::cerr << __func__ << "(): " << "Can't find file " << FileName << "\n";
		return nullptr;
		break;
	}

	return File;
}

/*
 * You could call vw fclose() if you should release memory in particular
 * part of code. Otherwise, it will be released automatically (see. unique_ptr).
 */
int vw_fclose(std::unique_ptr<sFILE> &stream)
{
	if (!stream.get())
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

	if (!buffer || !Data)
		return ERR_PARAMETERS;

	// read data
	for (size_t i = 0; i < count; i++) {
		if (size <= static_cast<unsigned>(Size - Pos)) {
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
		if ((Pos + offset) > Size)
			return ERR_PARAMETERS;
		Pos += offset;
		break;

	case SEEK_END:
		if (offset > Size)
			return ERR_PARAMETERS;
		Pos = Size - offset;
		break;

	case SEEK_SET:
		if ((offset < 0) || (offset > Size))
			return ERR_PARAMETERS;
		Pos = offset;
		break;

	default:
		std::cerr << __func__ << "(): " << "Error in fseek function call, wrong origin.\n";
		return ERR_PARAMETERS;
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

} // viewizard namespace
