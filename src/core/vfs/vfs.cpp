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


	Website: https://viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

// NOTE in future, use make_unique() to make unique_ptr-s (since C++14)

/*
 The main VFS concept:
 1. store all game data in one file;
 2. provide unified access to game data (libSDL2 used as backend);
 3. on request, copy game data to memory buffer and care about it.

 On VFS file open, VFS entries list generated with all available in this VFS
 files data. Could be opened multiple VFS files, in this case VFS entries list
 will contain all available in all opened VFS files data.
 On cFILE open, all requested data will be copied into memory buffer (cFILE->Data_).
 Opened cFILE is not connected to VFS file or VFS entries list in any way.

 Caller should hold cFILE open as long, as it need memory buffer (cFILE->Data_).
 In order to code simplicity, read and write direct access to cFILE data allowed.
 Caller could reset() memory buffer with different size (cFILE->Data), but should
 care about cFILE->Size_ and cFILE->Pos_ field (access by fseek()).

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

Important!
Don't use std::make_shared here, since std::make_shared performs one heap-allocation,
whereas calling the std::shared_ptr constructor performs two.
https://en.cppreference.com/w/cpp/memory/shared_ptr (see "Implementation notes")
https://stackoverflow.com/questions/20895648/difference-in-make-shared-and-normal-shared-ptr-in-c

*/

#include "vfs.h"
#include <limits> // need this one for UINT16_MAX only

namespace viewizard {

struct sVFS {
	std::string FileName;
	std::fstream File{};

	explicit sVFS(const std::string &_FileName) :
		FileName{_FileName}
	{}
};

struct sVFS_Entry {
	uint32_t Offset{0};
	uint32_t Size{0}; // NOTE cFILE::Size_ is long (32/64 depending from platform)
	std::weak_ptr<sVFS> Parent{};
};

namespace {

// List with all opened VFS.
std::forward_list<std::shared_ptr<sVFS>> VFSList;
// Map with file's entries in all opened VFS.
std::unordered_map<std::string, sVFS_Entry> VFSEntriesMap;

constexpr unsigned int FixedHeaderPartSize = 4 + 4 + 4; /*VFS_ + ver + build*/

} // unnamed namespace


/*
 * Write data from memory into VFS file.
 */
static int WriteIntoVFSfromMemory(const std::shared_ptr<sVFS> &WritableVFS, const std::string &Name,
				  const uint8_t *DataBuffer, uint32_t DataSize, uint32_t &FileTableOffset,
				  std::unordered_map<std::string, sVFS_Entry> &WritableVFSEntriesMap)
{
	if (!WritableVFS || Name.empty() || !DataBuffer ||
	    (DataSize <= 0) || (Name.size() > UINT16_MAX)) // UINT16_MAX - we should store string size in uint16_t variable
		return ERR_PARAMETERS;

	// add new data to VFS file, in this case we could use
	// FileTableOffset, since this is the end of data part
	WritableVFSEntriesMap[Name].Offset = FileTableOffset;
	WritableVFSEntriesMap[Name].Size = DataSize;
	WritableVFS->File.seekp(WritableVFSEntriesMap[Name].Offset, std::ios::beg);
	WritableVFS->File.write(reinterpret_cast<const char*>(DataBuffer), WritableVFSEntriesMap[Name].Size);
	WritableVFSEntriesMap[Name].Parent = WritableVFS;

	// write all entries belong to this VFS file
	for (const auto &tmpEntry : WritableVFSEntriesMap) {
		auto sharedParent = tmpEntry.second.Parent.lock();
		if (sharedParent == WritableVFS) {
			uint16_t tmpNameSize{(uint16_t)(tmpEntry.first.size())};
			WritableVFS->File.write(reinterpret_cast<char*>(&tmpNameSize), sizeof(tmpNameSize));
			WritableVFS->File.write(tmpEntry.first.c_str(), tmpEntry.first.size());
			WritableVFS->File.write(reinterpret_cast<const char*>(&tmpEntry.second.Offset),
						sizeof(tmpEntry.second.Offset));
			WritableVFS->File.write(reinterpret_cast<const char*>(&tmpEntry.second.Size),
						sizeof(tmpEntry.second.Size));
		}
	}

	WritableVFS->File.seekp(FixedHeaderPartSize, std::ios::beg);
	FileTableOffset += DataSize;
	WritableVFS->File.write(reinterpret_cast<char*>(&FileTableOffset), sizeof(FileTableOffset));

	std::cout << Name << " file added to VFS.\n";
	return 0;
}

/*
 * Write data from file into VFS file.
 */
static int WriteIntoVFSfromFile(const std::shared_ptr<sVFS> &WritableVFS, const std::string &SrcName,
				const std::string &DstName, uint32_t &FileTableOffset,
				std::unordered_map<std::string, sVFS_Entry> &WritableVFSEntriesMap)
{
	if (!WritableVFS || SrcName.empty() || DstName.empty())
		return ERR_PARAMETERS;

	std::ifstream File{SrcName, std::ios::binary};
	if (File.fail()) {
		std::cerr << __func__ << "(): " << "Can't find file " << SrcName << "\n";
		return ERR_FILE_NOT_FOUND;
	}

	File.seekg(0, std::ios::end);
	auto tmpSize = File.tellg();
	if (tmpSize == std::ios::pos_type(-1))
		return ERR_PARAMETERS;
	File.seekg(0, std::ios::beg);
	uint32_t tmpFileSize = static_cast<uint32_t>(tmpSize);

	// std::unique_ptr, we need only memory allocation without container's features
	// don't use std::vector here, since it allocates AND value-initializes
	std::unique_ptr<uint8_t[]> tmpBuffer(new uint8_t[tmpFileSize]);
	File.read(reinterpret_cast<char*>(tmpBuffer.get()), tmpFileSize);

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

	std::shared_ptr<sVFS> TempVFS{new sVFS{Name}};

	TempVFS->File.open(Name, std::ios::binary | std::ios::out);
	if (TempVFS->File.fail()) {
		std::cerr << __func__ << "(): " << "Can't open VFS file for write " << Name << "\n";
		return ERR_FILE_NOT_FOUND;
	}

	// write VFS sign "VFS_", version and build number
	constexpr char Sign[4]{'V','F','S','_'};
	TempVFS->File.write(reinterpret_cast<const char*>(Sign), 4 /*fixed 4 bytes size*/);
	TempVFS->File.write(reinterpret_cast<const char*>(VFS_VER), 4 /*fixed 4 bytes size*/);
	TempVFS->File.write(reinterpret_cast<char*>(&BuildNumber), 4 /*fixed 4 bytes size*/);

	// new file table offset
	uint32_t FileTableOffset{FixedHeaderPartSize + sizeof(FileTableOffset)};
	TempVFS->File.write(reinterpret_cast<char*>(&FileTableOffset), sizeof(FileTableOffset));

	// we need separate VFS entries map
	std::unordered_map<std::string, sVFS_Entry> WritableVFSEntriesMap;

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
			std::unique_ptr<cFILE> tmpFile = vw_fopen(tmpVFSEntry.first);
			if (!tmpFile)
				return ERR_FILE_NOT_FOUND;
			err = WriteIntoVFSfromMemory(TempVFS, tmpVFSEntry.first, tmpFile->GetData(),
						     static_cast<uint32_t>(tmpFile->GetSize()),
						     FileTableOffset, WritableVFSEntriesMap);
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
			int err = WriteIntoVFSfromFile(TempVFS, RawDataDir + GameData[i], GameData[i],
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

	VFSList.emplace_front(new sVFS{Name});

	VFSList.front()->File.open(Name, std::ios::binary | std::ios::in);
	if (VFSList.front()->File.fail())
		return errPrintWithVFSListPop("Can't find VFS file", ERR_FILE_NOT_FOUND);

	VFSList.front()->File.seekg(0, std::ios::end);
	auto VFS_FileSize = VFSList.front()->File.tellg();
	if (VFS_FileSize == std::ios::pos_type(-1))
		return errPrintWithVFSListPop("VFS file size error", ERR_FILE_IO);
	VFSList.front()->File.seekg(0, std::ios::beg);

	// check VFS file sign "VFS_"
	char Sign[4];
	VFSList.front()->File.read(reinterpret_cast<char*>(&Sign), 4 /*fixed 4 bytes size*/);
	if (VFSList.front()->File.fail())
		return errPrintWithVFSListPop("VFS file size error", ERR_FILE_IO);
	// Sign don't contain null-terminated string, strncmp() should be used
	if (strncmp(Sign, "VFS_", 4) != 0)
		return errPrintWithVFSListPop("VFS file header error", ERR_FILE_IO);

	// check VFS file version
	char Version[4];
	VFSList.front()->File.read(reinterpret_cast<char*>(&Version), 4 /*fixed 4 bytes size*/);
	if (VFSList.front()->File.fail())
		return errPrintWithVFSListPop("VFS file corrupted:", ERR_FILE_IO);
	// Version don't contain null-terminated string, strncmp() should be used
	if (strncmp(Version, VFS_VER, 4) != 0)
		return errPrintWithVFSListPop("VFS file has wrong version:", ERR_FILE_IO);

	// check VFS file build number
	unsigned int vfsBuildNumber;
	VFSList.front()->File.read(reinterpret_cast<char*>(&vfsBuildNumber), 4 /*fixed 4 bytes size*/);
	if (VFSList.front()->File.fail())
		return errPrintWithVFSListPop("VFS file corrupted:", ERR_FILE_IO);
	if (BuildNumber) {
		if (vfsBuildNumber) {
			if (BuildNumber != vfsBuildNumber)
				return errPrintWithVFSListPop("VFS file has wrong build number", ERR_VFS_BUILD);
		} else
			return errPrintWithVFSListPop("VFS file build number was not set", ERR_VFS_BUILD);
	}

	uint32_t FileTableOffset;
	VFSList.front()->File.read(reinterpret_cast<char*>(&FileTableOffset), sizeof(FileTableOffset));
	VFSList.front()->File.seekg(FileTableOffset, std::ios::beg);

	// add entries from new connected VFS file
	while (VFSList.front()->File.good() &&
	       (VFS_FileSize != VFSList.front()->File.tellg())) {
		uint16_t tmpNameSize;
		VFSList.front()->File.read(reinterpret_cast<char*>(&tmpNameSize), sizeof(tmpNameSize));

		std::string tmpName;
		tmpName.resize(tmpNameSize);
		// NOTE remove const_cast in future, (since C++17) "CharT* data();" also added.
		VFSList.front()->File.read(const_cast<char*>(tmpName.data()), tmpNameSize);

		VFSList.front()->File.read(reinterpret_cast<char*>(&VFSEntriesMap[tmpName].Offset),
					   sizeof(VFSEntriesMap[tmpName].Offset));
		VFSList.front()->File.read(reinterpret_cast<char*>(&VFSEntriesMap[tmpName].Size),
					   sizeof(VFSEntriesMap[tmpName].Size));
		VFSEntriesMap[tmpName].Parent = VFSList.front();
	}

	// unconditional rehash, at this line we have not rehashed map
	VFSEntriesMap.rehash(0);

	std::cout << "VFS file was opened " << Name << "\n";
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
 * Open the sFILE.
 * Return std::unique_ptr, provide smart pointer connected to caller's scope.
 */
std::unique_ptr<cFILE> vw_fopen(const std::string &FileName)
{
	if (FileName.empty())
		return nullptr;

	auto FileInVFS = VFSEntriesMap.find(FileName);
	if (FileInVFS != VFSEntriesMap.end()) {
		auto sharedParent = FileInVFS->second.Parent.lock();
		if (!sharedParent) {
			VFSEntriesMap.erase(FileName); // this is "dead" entry not connected to any VFS, remove it
			return nullptr;
		}

		std::unique_ptr<cFILE> File(new cFILE(0, 0));

		File->Size_ = static_cast<long>(FileInVFS->second.Size);
		sharedParent->File.seekg(FileInVFS->second.Offset, std::ios::beg);
		File->Data_.reset(new uint8_t[File->Size_]);
		sharedParent->File.read(reinterpret_cast<char*>(File->Data_.get()), File->Size_);

		return File;
	}

	std::ifstream fsFile(FileName, std::ios::binary);
	if (fsFile.good()) {
		fsFile.seekg(0, std::ios::end);
		auto tmpSize = fsFile.tellg();
		if (tmpSize == std::ios::pos_type(-1))
			return nullptr;
		fsFile.seekg(0, std::ios::beg);

		std::unique_ptr<cFILE> File(new cFILE(0, 0));

		File->Size_ = static_cast<long>(tmpSize);
		File->Data_.reset(new uint8_t[File->Size_]);
		fsFile.read(reinterpret_cast<char*>(File->Data_.get()), File->Size_);

		return File;
	}

	return nullptr;
}

/*
 * You could call vw fclose() if you should release memory in particular
 * part of code. Otherwise, it will be released automatically (see. unique_ptr).
 */
int vw_fclose(std::unique_ptr<cFILE> &stream)
{
	if (!stream.get())
		return ERR_PARAMETERS;

	// release allocated memory
	stream.reset();

	return 0;
}

/*
 * Reads an array of 'count' elements, each one with a size of 'size' bytes,
 * from the stream and stores them in the block of memory specified by 'buffer'.
 */
size_t cFILE::fread(void *buffer, size_t size, size_t count)
{
	if (!buffer || !Data_)
		return ERR_PARAMETERS;

	size_t CopyCount{0};
	for (; (CopyCount < count) && (Size_ >= static_cast<long>(Pos_ + size)); CopyCount++) {
		memcpy(static_cast<uint8_t *>(buffer) + CopyCount * size, Data_.get() + Pos_, size);
		Pos_ += size;
	}

	return CopyCount;
}

/*
 * Sets the position indicator associated with the stream to a new position.
 */
int cFILE::fseek(long offset, int origin)
{
	switch (origin) {
	case SEEK_CUR:
		if ((Pos_ + offset > Size_) || (Pos_ + offset < 0))
			return ERR_PARAMETERS;
		Pos_ += offset;
		break;

	case SEEK_END:
		if ((offset > 0) || (Size_ + offset < 0))
			return ERR_PARAMETERS;
		Pos_ = Size_ + offset;
		break;

	case SEEK_SET:
		if ((offset < 0) || (offset > Size_))
			return ERR_PARAMETERS;
		Pos_ = offset;
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
long cFILE::ftell()
{
	return Pos_;
}

} // viewizard namespace
