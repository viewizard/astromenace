/********************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2012 Michael Kurinnoy, Viewizard


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


	Web Site:		http://www.viewizard.com/
	E-mail:			viewizard@viewizard.com
	Postal address:	kvartal Zhukova, 10/79, Lugansk city, 91051, Ukraine


*********************************************************************************/


#ifndef Texture_H
#define Texture_H


#include "../Base.h"
#include "../VirtualFileSystem/VFS.h"



// File types (for LoadAs parametre)
#define AUTO_FILE			0	// Detect by file extension
#define VW2D_FILE			1	// VW2D file
#define TGA_FILE			2	// TGA file
#define JPG_FILE			4	// JPG file
#define PNG_FILE			8	// PNG file

// Create alpha channel by image greyscale color
#define TX_ALPHA_GREYSC		0x0021
// Create alpha channel by equal Alpha color
#define TX_ALPHA_EQUAL		0x0022
// Create alpha channel by great or equal than Alpha color
#define TX_ALPHA_GEQUAL		0x0023
// Create alpha channel by less or equal Alpha color
#define TX_ALPHA_LEQUAL		0x0024
// Create alpha channel by great than Alpha color
#define TX_ALPHA_GREAT		0x0025
// Create alpha channel by less Alpha color
#define TX_ALPHA_LESS		0x0026

struct eTexture
{
	char*	Name;			// File name

	BYTE	ARed;			// Alpha channel red color
	BYTE	AGreen;			// Alpha channel green color
	BYTE	ABlue;			// Alpha channel blue color

	int		Width;			// Texture width
	int		Height;			// Texture height
	int		SrcWidth;		// Source image width
	int		SrcHeight;		// Source image height

	int		Bytes;			// Bytes Per Pixel

    GLuint	TextureID;		// Номер текстуры

	eTexture*	Prev;		// Pointer to the previous loaded Texture in the memory
	eTexture*	Next;		// Pointer to the next loaded Texture in the memory
	int			Num;		// ID number
};


// eTexture functions
// Load texture from file
eTexture*	vw_LoadTexture(const char *TextureName, const char *RememberAsName, bool NeedCompression, int LoadAs=AUTO_FILE, int NeedResizeW=0, int NeedResizeH=0);
// Create texture from memory
eTexture*	vw_CreateTextureFromMemory(const char *TextureName, BYTE * DIB, int DWidth, int DHeight, int DChanels, bool NeedCompression, int NeedResizeW=0, int NeedResizeH=0);;
// Release texture
void		vw_ReleaseTexture(eTexture* Texture);


// Texture manager functions

// Release all textures
void		vw_ReleaseAllTextures();
// Set textures properties
void		vw_SetTextureProp(int nFiltering, int nAddress_Mode, bool nAlpha = false, int nAFlag = TX_ALPHA_EQUAL, bool nMipMap = true);
// Set textures alpha color
void		vw_SetTextureAlpha(BYTE nARed, BYTE nAGreen, BYTE nABlue);

// Find texture by name
eTexture*	vw_FindTextureByName(const char *Name);
// Find texture by ID
eTexture*	vw_FindTextureByNum(int Num);


// Convert supported image file to VW2D format
void 		vw_ConvertImageToVW2D(const char *SrcName, const char *DestName);


#endif // Texture_H
