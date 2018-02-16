/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright (c) 2006-2016 Mikhail Kurinnoi, Viewizard


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


#include "Texture.h"




//------------------------------------------------------------------------------------
// загрузка данных из TGA файла в DIB массив
//------------------------------------------------------------------------------------
#define TGA_RGB		 2		// This tells us it's a normal RGB (really BGR) file
#define TGA_A		 3		// This tells us it's a ALPHA file
#define TGA_RLE		10		// This tells us that the targa is Run-Length Encoded (RLE)
int ReadTGA(BYTE **DIB, eFILE *pFile, int *DWidth, int *DHeight, int *DChanels)
{

	BYTE length = 0;					// The length in bytes to the pixels
	BYTE imageType = 0;					// The image type (RLE, RGB, Alpha...)
	BYTE bits = 0;						// The bits per pixel for the image (16, 24, 32)
	int channels = 0;					// The channels of the image (3 = RGA : 4 = RGBA)
	size_t stride = 0;						// The stride (channels * width)
	int i = 0;							// A counter



	// Read in the length in bytes from the header to the pixel data
	pFile->fread(&length, sizeof(BYTE), 1);

	// Jump over one byte
	pFile->fseek(1, SEEK_CUR);

	// Read in the imageType (RLE, RGB, etc...)
	pFile->fread(&imageType, sizeof(BYTE), 1);


	// Skip past general information we don't care about
	pFile->fseek(9, SEEK_CUR);

	// Read the width, height and bits per pixel (16, 24 or 32)
	WORD TmpReadData;
	pFile->fread(&TmpReadData,  sizeof(WORD), 1);
	*DWidth = TmpReadData;
	pFile->fread(&TmpReadData, sizeof(WORD), 1);
	*DHeight = TmpReadData;
	pFile->fread(&bits,   sizeof(BYTE), 1);

	// Now we move the file pointer to the pixel data
	pFile->fseek(length + 1, SEEK_CUR);

	// Check if the image is RLE compressed or not
	if(imageType != TGA_RLE)
	{
		// Check if the image is a 24 or 32-bit image
		if(bits == 24 || bits == 32)
		{
			// Calculate the channels (3 or 4) - (use bits >> 3 for more speed).
			// Next, we calculate the stride and allocate enough memory for the pixels.
			channels = bits / 8;
			stride = channels * (*DWidth);
			*DIB = 0;
			*DIB = new BYTE[stride * (*DHeight)];
			if (*DIB == 0) return 0;

			// Load in all the pixel data line by line
			for(int y = 0; y < (*DHeight); y++)
			{
				// Store a pointer to the current line of pixels
				BYTE *pLine = (*DIB+stride * y);

				// Read in the current line of pixels
				pFile->fread(pLine, stride, 1);
			}
		}
		// Check if the image is a 16 bit image (RGB stored in 1 unsigned short)
		else if(bits == 16)
		{
			unsigned short pixels = 0;
			int r=0, g=0, b=0;

			// Since we convert 16-bit images to 24 bit, we hardcode the channels to 3.
			// We then calculate the stride and allocate memory for the pixels.
			channels = 3;
			stride = channels * (*DWidth);
			*DIB = 0;
			*DIB = new BYTE[stride * (*DHeight)];
			if (*DIB == 0) return 0;

			// Load in all the pixel data pixel by pixel
			for(i = 0; i < (*DWidth)*(*DHeight); i++)
			{
				// Read in the current pixel
				pFile->fread(&pixels, sizeof(unsigned short), 1);

				// To convert a 16-bit pixel into an R, G, B, we need to
				// do some masking and such to isolate each color value.
				// 0x1f = 11111 in binary, so since 5 bits are reserved in
				// each unsigned short for the R, G and B, we bit shift and mask
				// to find each value.  We then bit shift up by 3 to get the full color.
				b = (pixels & 0x1f) << 3;
				g = ((pixels >> 5) & 0x1f) << 3;
				r = ((pixels >> 10) & 0x1f) << 3;

				// This essentially assigns the color to our array and swaps the
				// B and R values at the same time.
				memcpy(*DIB+i*3+2, &r, sizeof(BYTE));
				memcpy(*DIB+i*3+1, &g, sizeof(BYTE));
				memcpy(*DIB+i*3+0, &b, sizeof(BYTE));
			}
		}
		// Else return a NULL for a bad or unsupported pixel format
		else
			return 0;
	}
	// Else, it must be Run-Length Encoded (RLE)
	else
	{
		// Create some variables to hold the rleID, current colors read, channels, & stride.
		BYTE rleID = 0;
		int colorsRead = 0;
		channels = bits / 8;
		stride = channels * (*DWidth);

		// Next we want to allocate the memory for the pixels and create an array,
		// depending on the channel count, to read in for each pixel.
		*DIB = 0;
		*DIB = new BYTE[stride * (*DHeight)];
		if (*DIB == 0) return 0;
		BYTE *pColors = 0;
		pColors = new BYTE[channels];
		if (pColors == 0) return 0;


		// Load in all the pixel data
		while(i < (*DWidth)*(*DHeight))
		{
			// Read in the current color count + 1
			pFile->fread(&rleID, sizeof(BYTE), 1);

			// Check if we don't have an encoded string of colors
			if(rleID < 128)
			{
				// Increase the count by 1
				rleID++;

				// Go through and read all the unique colors found
				while(rleID)
				{
					// Read in the current color
					pFile->fread(pColors, sizeof(BYTE) * channels, 1);

					// Store the current pixel in our image array
					memcpy(*DIB+colorsRead+0, &pColors[0], sizeof(BYTE));
					memcpy(*DIB+colorsRead+1, &pColors[1], sizeof(BYTE));
					memcpy(*DIB+colorsRead+2, &pColors[2], sizeof(BYTE));

					// If we have a 4 channel 32-bit image, assign one more for the alpha
					if(bits == 32)
						memcpy(*DIB+colorsRead+3, &pColors[3], sizeof(BYTE));

					// Increase the current pixels read, decrease the amount
					// of pixels left, and increase the starting index for the next pixel.
					i++;
					rleID--;
					colorsRead += channels;
				}
			}
			// Else, let's read in a string of the same character
			else
			{
				// Minus the 128 ID + 1 (127) to get the color count that needs to be read
				rleID -= 127;

				// Read in the current color, which is the same for a while
				pFile->fread(pColors, sizeof(BYTE) * channels, 1);

				// Go and read as many pixels as are the same
				while(rleID)
				{
					// Assign the current pixel to the current index in our pixel array
					memcpy(*DIB+colorsRead+0, &pColors[0], sizeof(BYTE));
					memcpy(*DIB+colorsRead+1, &pColors[1], sizeof(BYTE));
					memcpy(*DIB+colorsRead+2, &pColors[2], sizeof(BYTE));


					// If we have a 4 channel 32-bit image, assign one more for the alpha
					if(bits == 32)
						*DIB[colorsRead + 3] = pColors[3];

					// Increase the current pixels read, decrease the amount
					// of pixels left, and increase the starting index for the next pixel.
					i++;
					rleID--;
					colorsRead += channels;
				}
			}
		}
	}



	*DChanels = channels;

	// меняем местами цвета...

	BYTE TmpColorSwap;
	int k=0;
	for (i=0; i<(*DHeight); i++)
	for (int j=0; j<(*DWidth); j++)
	{
		memcpy(&TmpColorSwap, *DIB+k, 1);
		memcpy(*DIB+k, *DIB+k+2, 1);
		memcpy(*DIB+k+2, &TmpColorSwap, 1);
		k+=*DChanels;
	}

	return 1;
}

