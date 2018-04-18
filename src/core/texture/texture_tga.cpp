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

// NOTE in future, use make_unique() to make unique_ptr-s (since C++14)

#include "../vfs/vfs.h"

namespace texture {

#define TGA_RGB		 2	// normal RGB (BGR) file
#define TGA_RLE		10	// RLE file

int ReadTGA(std::unique_ptr<uint8_t[]> &PixelsArray, sFILE *pFile, int &DWidth, int &DHeight, int &DChanels)
{
	uint8_t tmpTGAHeaderLength{0};
	uint8_t tmpTGAImageType{0};	// RLE, RGB
	uint8_t tmpBits{0};		// 16, 24, 32

	pFile->fread(&tmpTGAHeaderLength, sizeof(tmpTGAHeaderLength), 1);
	// jump over one byte
	pFile->fseek(1, SEEK_CUR);
	// image type (RLE, RGB, ...)
	pFile->fread(&tmpTGAImageType, sizeof(tmpTGAImageType), 1);
	// skip past general information
	pFile->fseek(9, SEEK_CUR);
	// read the width, height and bpp
	uint16_t TmpReadData;
	pFile->fread(&TmpReadData, sizeof(TmpReadData), 1);
	DWidth = TmpReadData;
	pFile->fread(&TmpReadData, sizeof(TmpReadData), 1);
	DHeight = TmpReadData;
	pFile->fread(&tmpBits, sizeof(tmpBits), 1);
	// move to the pixel data
	pFile->fseek(tmpTGAHeaderLength + 1, SEEK_CUR);

	if (tmpTGAImageType == TGA_RGB) {
		if((tmpBits == 24) || (tmpBits == 32)) {
			DChanels = tmpBits / 8;
			size_t tmpStride = DChanels * DWidth;
			PixelsArray.reset(new uint8_t[tmpStride * DHeight]);

			// load line by line
			for(int y = 0; y < DHeight; y++) {
				uint8_t *pLine = PixelsArray.get() + tmpStride * y;
				pFile->fread(pLine, tmpStride, 1);
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
		while(i < DWidth * DHeight) {
			pFile->fread(&rleID, sizeof(rleID), 1);

			if(rleID < 128) {
				rleID++;

				while(rleID && (i < DWidth * DHeight)) {
					pFile->fread(pColors.data(), sizeof(pColors[0]) * DChanels, 1);

					memcpy(PixelsArray.get() + colorsRead, pColors.data(), sizeof(pColors[0]) * 3);
					if(tmpBits == 32)
						PixelsArray[colorsRead + 3] = pColors[3];

					i++;
					rleID--;
					colorsRead += DChanels;
				}
			} else {
				rleID -= 127;

				pFile->fread(pColors.data(), sizeof(pColors[0]) * DChanels, 1);

				while(rleID && (i < DWidth * DHeight)) {
					memcpy(PixelsArray.get() + colorsRead, pColors.data(), sizeof(pColors[0]) * 3);
					if(tmpBits == 32)
						PixelsArray[colorsRead + 3] = pColors[3];

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

} // namespace texture
