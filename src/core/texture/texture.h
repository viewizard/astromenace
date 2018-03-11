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

#ifndef Texture_H
#define Texture_H

#include "../base.h"

enum class eLoadTextureAs {
	AUTO,	// Detect by file extension
	VW2D,	// VW2D file
	TGA	// TGA file
};

enum class eAlphaCreateMode {
	NONE,	// Fill alpha channel with 255
	GREYSC,	// Create alpha channel by image greyscale color
	EQUAL	// Create alpha channel by equal Alpha color
};

struct sTexture {
	uint8_t ARed;		// Alpha channel red color
	uint8_t AGreen;		// Alpha channel green color
	uint8_t ABlue;		// Alpha channel blue color

	int Width;		// Texture width
	int Height;		// Texture height
	int SrcWidth;		// Source image width
	int SrcHeight;		// Source image height

	int Bytes;		// Bytes per pixel (not bits!)

	unsigned TextureID;	// Texture ID (OpenGL related)
};

// Load texture from file.
// Note, in case of resize, we should provide width and height (but not just one of them).
sTexture *vw_LoadTexture(const std::string &TextureName, int CompressionType,
			 eLoadTextureAs LoadAs = eLoadTextureAs::AUTO,
			 int NeedResizeW = 0, int NeedResizeH = 0);
// Create texture from memory.
sTexture *vw_CreateTextureFromMemory(const std::string &TextureName, std::vector<uint8_t> &DIB, int DIBWidth,
				     int DIBHeight, int DIBChanels, int CompressionType, int NeedResizeW = 0,
				     int NeedResizeH = 0, bool NeedDuplicateCheck = true);
// Release texture.
void vw_ReleaseTexture(sTexture* Texture);
// Release all textures.
void vw_ReleaseAllTextures();
// Set textures properties.
void vw_SetTextureProp(int nFiltering, int nAddress_Mode, bool nAlpha = false,
		       eAlphaCreateMode nAFlag = eAlphaCreateMode::EQUAL, bool nMipMap = true);
// Set textures alpha color.
void vw_SetTextureAlpha(uint8_t nARed, uint8_t nAGreen, uint8_t nABlue);
// Find texture by name.
sTexture *vw_FindTextureByName(const std::string &Name);
// Convert supported image file format to VW2D format.
void vw_ConvertImageToVW2D(const std::string &SrcName, const std::string &DestName);

#endif // Texture_H
