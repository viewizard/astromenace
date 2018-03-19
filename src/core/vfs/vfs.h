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

enum class eFileLocation {
	Unknown,
	VFS,	// File present in the VFS.
	FS	// File present in the File System.
};


// Create VFS file.
int vw_CreateVFS(const std::string &Name, unsigned int BuildNumber,
		 const std::string &RawDataDir, const std::string &ModelsPack,
		 const std::string GameData[], unsigned int GameDataCount);
// Open VFS file.
int vw_OpenVFS(const std::string &Name, unsigned int BuildNumber);
// Get internal VFS state for data extraction.
int vw_GetInternalDataStateInVFS(const std::string &FileName, SDL_RWops **VFSFile,
				 uint32_t &DataOffset, uint32_t &DataSize);
// Shutdown VFS.
void vw_ShutdownVFS();
// Detect file location.
eFileLocation vw_DetectFileLocation(const std::string &FileName);

// Get stored in VFS data to buffer (or fallback to FS).
template <typename T>
inline int vw_VFStoBuffer(const std::string &FileName, T &Buffer)
{
	if (FileName.empty())
		return ERR_PARAMETERS;

	SDL_RWops *tmpFile{nullptr};
	uint32_t DataOffset{0}, DataSize{0};
	int err{0};

	// since C++11
	// The array should be null-terminated, data() and c_str() perform the same function.
	bool isString = std::is_same<T, std::string>::value;

	switch (vw_DetectFileLocation(FileName)) {
	case eFileLocation::VFS:
		err = vw_GetInternalDataStateInVFS(FileName, &tmpFile, DataOffset, DataSize);
		if (err)
			return err;

		Buffer.resize(DataSize + (isString ? 1 : 0));
		SDL_RWseek(tmpFile, DataOffset, SEEK_SET);
		// don't remove .data() usage, we need .data() and .resize()
		// member functions in order to detect allowed container's
		// type for template
		// TODO remove (void*) in future, (since C++17) "CharT* data();" also added.
		SDL_RWread(tmpFile, (void *)Buffer.data(), DataSize, 1);

		if (isString)
			Buffer[DataSize] = '\0';
		break;

	case eFileLocation::FS:
		tmpFile = SDL_RWFromFile(FileName.c_str(), "rb");
		if (!tmpFile)
			return ERR_FILE_NOT_FOUND;

		SDL_RWseek(tmpFile, 0, SEEK_END);
		DataSize = SDL_RWtell(tmpFile);
		SDL_RWseek(tmpFile, 0, SEEK_SET);

		Buffer.resize(DataSize + (isString ? 1 : 0));
		// don't remove .data() usage, we need .data() and .resize()
		// member functions in order to detect allowed container's
		// type for template
		// TODO remove (void*) in future, (since C++17) "CharT* data();" also added.
		SDL_RWread(tmpFile, (void *)Buffer.data(), DataSize, 1);
		SDL_RWclose(tmpFile);

		if (isString)
			Buffer[DataSize] = '\0';
		break;

	case eFileLocation::Unknown:
		std::cerr << __func__ << "(): " << "Can't find file " << FileName << "\n";
		return ERR_FILE_NOT_FOUND;
		break;
	}

	return 0;
}

struct sFILE {
	uint32_t Size{0};
	// std::unique_ptr, we need only memory allocation without container's features
	// don't use std::vector here, since it allocates AND value-initializes
	std::unique_ptr<uint8_t[]> Data{};

	int fread(void *buffer, size_t size, size_t count);
	int fseek(long offset, int origin);
	long ftell();

	sFILE(unsigned int _Size, long _Pos) :
		Size{_Size},
		Pos{_Pos}
	{}

private:
	long Pos{0};
};

// Return std::unique_ptr, provide smart pointer connected to caller's scope.
std::unique_ptr<sFILE> vw_fopen(const std::string &FileName);
// You could call vw_fclose() if you should release memory in particular
// part of code. Otherwise, it will be released automatically (see. unique_ptr).
int vw_fclose(std::unique_ptr<sFILE> &stream);

#endif // VFS_H
