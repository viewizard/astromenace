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

#include "../vfs/vfs.h"

#define TGA_RGB		 2	/* normal RGB (BGR) file */
#define TGA_A		 3	/* ALPHA file */
#define TGA_RLE		10	/* RLE file */

int ReadTGA(uint8_t **DIB, sFILE *pFile, int *DWidth, int *DHeight, int *DChanels)
{

	uint8_t length = 0;
	uint8_t imageType = 0;	/* RLE, RGB, Alpha */
	uint8_t bits = 0;		/* 16, 24, 32 */
	int channels = 0;	/* 3 = RGA : 4 = RGBA */
	size_t stride = 0;	/* (channels * width) */
	int i = 0;

	pFile->fread(&length, sizeof(length), 1);
	/* jump over one byte */
	pFile->fseek(1, SEEK_CUR);
	/* image type (RLE, RGB, ...) */
	pFile->fread(&imageType, sizeof(imageType), 1);
	/* skip past general information */
	pFile->fseek(9, SEEK_CUR);
	/* read the width, height and bpp */
	uint16_t TmpReadData;
	pFile->fread(&TmpReadData,  sizeof(TmpReadData), 1);
	*DWidth = TmpReadData;
	pFile->fread(&TmpReadData, sizeof(TmpReadData), 1);
	*DHeight = TmpReadData;
	pFile->fread(&bits, sizeof(bits), 1);
	/* move to the pixel data */
	pFile->fseek(length + 1, SEEK_CUR);

	if(imageType != TGA_RLE) {
		if((bits == 24) || (bits == 32)) {
			channels = bits / 8;
			stride = channels * (*DWidth);
			*DIB = new uint8_t[stride * (*DHeight)];

			/* load line by line */
			for(int y = 0; y < (*DHeight); y++) {
				uint8_t *pLine = (*DIB+stride * y);
				pFile->fread(pLine, stride, 1);
			}
		} else if(bits == 16) {
			unsigned short pixels = 0;
			int r = 0, g = 0, b = 0;

			/* convert 16-bit images to 24 bit */
			channels = 3;
			stride = channels * (*DWidth);
			*DIB = new uint8_t[stride * (*DHeight)];

			for(i = 0; i < (*DWidth)*(*DHeight); i++) {
				pFile->fread(&pixels, sizeof(pixels), 1);

				b = (pixels & 0x1f) << 3;
				g = ((pixels >> 5) & 0x1f) << 3;
				r = ((pixels >> 10) & 0x1f) << 3;
				memcpy(*DIB+i*3+2, &r, sizeof(uint8_t));
				memcpy(*DIB+i*3+1, &g, sizeof(uint8_t));
				memcpy(*DIB+i*3+0, &b, sizeof(uint8_t));
			}
		} else {
			return 0;
		}
	} else { /* RLE*/
		uint8_t rleID = 0;
		int colorsRead = 0;
		channels = bits / 8;
		stride = channels * (*DWidth);

		*DIB = new uint8_t[stride * (*DHeight)];
		uint8_t *pColors = new uint8_t[channels];

		while(i < (*DWidth)*(*DHeight)) {
			pFile->fread(&rleID, sizeof(rleID), 1);

			if(rleID < 128) {
				rleID++;

				while(rleID) {
					pFile->fread(pColors, sizeof(pColors[0]) * channels, 1);

					memcpy(*DIB+colorsRead+0, &pColors[0], sizeof(pColors[0]));
					memcpy(*DIB+colorsRead+1, &pColors[1], sizeof(pColors[0]));
					memcpy(*DIB+colorsRead+2, &pColors[2], sizeof(pColors[0]));

					if(bits == 32)
						memcpy(*DIB+colorsRead+3, &pColors[3], sizeof(pColors[0]));

					i++;
					rleID--;
					colorsRead += channels;
				}
			} else {
				rleID -= 127;

				pFile->fread(pColors, sizeof(pColors[0]) * channels, 1);

				while(rleID) {
					memcpy(*DIB+colorsRead+0, &pColors[0], sizeof(pColors[0]));
					memcpy(*DIB+colorsRead+1, &pColors[1], sizeof(pColors[0]));
					memcpy(*DIB+colorsRead+2, &pColors[2], sizeof(pColors[0]));

					if(bits == 32)
						*DIB[colorsRead + 3] = pColors[3];

					i++;
					rleID--;
					colorsRead += channels;
				}
			}
		}
	}

	*DChanels = channels;

	/* swap colors */
	uint8_t TmpColorSwap;
	int k=0;
	for (i=0; i<(*DHeight); i++)
		for (int j=0; j<(*DWidth); j++) {
			memcpy(&TmpColorSwap, *DIB+k, 1);
			memcpy(*DIB+k, *DIB+k+2, 1);
			memcpy(*DIB+k+2, &TmpColorSwap, 1);
			k+=*DChanels;
		}

	return 1;
}

