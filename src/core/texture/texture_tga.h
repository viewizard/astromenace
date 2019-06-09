/****************************************************************************

    AstroMenace
    Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
    Copyright (c) 2006-2019 Mikhail Kurinnoi, Viewizard


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

#ifndef CORE_TEXTURE_TEXTURETGA_H
#define CORE_TEXTURE_TEXTURETGA_H

#include "../base.h"
#include "../vfs/vfs.h"

namespace viewizard {

int ReadTGA(std::unique_ptr<uint8_t[]> &PixelsArray, cFILE *pFile, int &DWidth, int &DHeight, int &DChanels);

} // viewizard namespace

#endif // CORE_TEXTURE_TEXTURETGA_H
