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

// NOTE in future, use make_unique() to make unique_ptr-s (since C++14)

#include "../vfs/vfs.h"
#include <cstring>

namespace viewizard {

int ReadTGA(std::unique_ptr<uint8_t[]> &PixelsArray, cFILE *pFile, int &DWidth, int &DHeight, int &DChanels)
{
    constexpr uint8_t TGA_RGB{2};   // normal RGB (BGR) file
    constexpr uint8_t TGA_RLE{10};  // RLE file

    uint8_t tmpTGAHeaderLength{0};
    uint8_t tmpTGAImageType{0};     // RLE, RGB
    uint8_t tmpBits{0};             // 16, 24, 32

    if (pFile->fread(&tmpTGAHeaderLength, sizeof(tmpTGAHeaderLength), 1) != 1) {
        return ERR_FILE_IO;
    }
    // jump over one byte
    int Status = RES_OK;
    IfFailRet(pFile->fseek(1, SEEK_CUR));
    // image type (RLE, RGB, ...)
    if (pFile->fread(&tmpTGAImageType, sizeof(tmpTGAImageType), 1) != 1) {
        return ERR_FILE_IO;
    }
    // skip past general information
    IfFailRet(pFile->fseek(9, SEEK_CUR));
    // read the width, height and bpp
    uint16_t TmpReadData;
    if (pFile->fread(&TmpReadData, sizeof(TmpReadData), 1) != 1) {
        return ERR_FILE_IO;
    }
    DWidth = TmpReadData;
    if (pFile->fread(&TmpReadData, sizeof(TmpReadData), 1) != 1) {
        return ERR_FILE_IO;
    }
    DHeight = TmpReadData;
    if (pFile->fread(&tmpBits, sizeof(tmpBits), 1) != 1) {
        return ERR_FILE_IO;
    }
    // move to the pixel data
    IfFailRet(pFile->fseek(tmpTGAHeaderLength + 1, SEEK_CUR));

    if (tmpTGAImageType == TGA_RGB) {
        if (tmpBits == 24 || tmpBits == 32) {
            DChanels = tmpBits / 8;
            size_t tmpStride = DChanels * DWidth;
            PixelsArray.reset(new uint8_t[tmpStride * DHeight]);

            // load line by line
            for (int y = 0; y < DHeight; y++) {
                uint8_t *pLine = PixelsArray.get() + tmpStride * y;
                if (pFile->fread(pLine, tmpStride, 1) != 1) {
                    return ERR_FILE_IO;
                }
            }
        } else {
            std::cerr << __func__ << "(): " << "16 bits TGA images are not supported.\n";
            return ERR_NOT_SUPPORTED;
        }
    } else if (tmpTGAImageType == TGA_RLE) {
        uint8_t rleID = 0;
        int colorsRead = 0;
        DChanels = tmpBits / 8;

        PixelsArray.reset(new uint8_t[DWidth * DHeight * DChanels]);
        std::vector<uint8_t> pColors(DChanels);

        int i = 0;
        while (i < DWidth * DHeight) {
            if (pFile->fread(&rleID, sizeof(rleID), 1) != 1) {
                return ERR_FILE_IO;
            }

            if (rleID < 128) {
                rleID++;

                while (rleID && (i < DWidth * DHeight)) {
                    if (pFile->fread(pColors.data(), sizeof(pColors[0]) * DChanels, 1) != 1) {
                        return ERR_FILE_IO;
                    }

                    memcpy(PixelsArray.get() + colorsRead, pColors.data(), sizeof(pColors[0]) * 3);
                    if (tmpBits == 32) {
                        PixelsArray[colorsRead + 3] = pColors[3];
                    }

                    i++;
                    rleID--;
                    colorsRead += DChanels;
                }
            } else {
                // static_cast should be here to suppress conversion warnings
                // they are really do nothing (same ASM result as for "rleID -= 127;")
                rleID = static_cast<uint8_t>(static_cast<int8_t>(rleID) - 127);

                if (pFile->fread(pColors.data(), sizeof(pColors[0]) * DChanels, 1) != 1) {
                    return ERR_FILE_IO;
                }

                while (rleID && (i < DWidth * DHeight)) {
                    memcpy(PixelsArray.get() + colorsRead, pColors.data(), sizeof(pColors[0]) * 3);
                    if (tmpBits == 32) {
                        PixelsArray[colorsRead + 3] = pColors[3];
                    }

                    i++;
                    rleID--;
                    colorsRead += DChanels;
                }
            }
        }
    } else {
        std::cerr << __func__ << "(): " << "Not supported TGA images type.\n";
        return ERR_NOT_SUPPORTED;
    }

    // swap colors (BGR -> RGB)
    for (int i = 0; i < DWidth * DHeight * DChanels; i += DChanels) {
        uint8_t tmpColorSwap{PixelsArray[i]};
        PixelsArray[i] = PixelsArray[i + 2];
        PixelsArray[i + 2] = tmpColorSwap;
    }

    return 0;
}

} // viewizard namespace
