/****************************************************************************

    AstroMenace
    Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
    Copyright (C) 2006-2025 Mikhail Kurinnoi, Viewizard


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

*****************************************************************************/

#ifndef CORE_VFS_VFS_H
#define CORE_VFS_VFS_H

#include "../base.h"

namespace viewizard {

constexpr char VFS_VER[]{"v1.6"};

// Create VFS file.
int vw_CreateVFS(const std::string &Name, unsigned int BuildNumber,
                 const std::string &RawDataDir, const std::string &ModelsPack,
                 const std::string GameData[], unsigned int GameDataCount);
// Open VFS file.
int vw_OpenVFS(const std::string &Name, unsigned int BuildNumber);
// Shutdown VFS.
void vw_ShutdownVFS();

class cFILE {
    friend std::unique_ptr<cFILE> vw_fopen(const std::string &FileName);

public:
    explicit cFILE(long Size, long Pos) :
        Size_{Size},
        Pos_{Pos}
    {}

    long GetSize()
    {
        return Size_;
    }

    uint8_t *GetData()
    {
        return Data_.get();
    }

    size_t fread(void *buffer, size_t size, size_t count);
    int fseek(long offset, int origin);
    long ftell();

private:
    long Size_{0};
    long Pos_{0};

    // std::unique_ptr, we need only memory allocation without container's features
    // don't use std::vector here, since it allocates AND value-initializes
    std::unique_ptr<uint8_t[]> Data_{};
};

// Return std::unique_ptr, provide smart pointer connected to caller's scope.
std::unique_ptr<cFILE> vw_fopen(const std::string &FileName);
// You could call vw_fclose() if you should release memory in particular
// part of code. Otherwise, it will be released automatically (see. unique_ptr).
int vw_fclose(std::unique_ptr<cFILE> &stream);

} // viewizard namespace

#endif // CORE_VFS_VFS_H
