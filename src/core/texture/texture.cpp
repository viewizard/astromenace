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


	Web Site: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

// NOTE in future, use make_unique() to make unique_ptr-s (since C++14)

/*
Note, all texture-related code aimed to 1 byte texture alignment.
Plus, default OpenGL setup also aimed for 1 byte alignment:
glPixelStorei(GL_PACK_ALIGNMENT, 1);
glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

Code work with VW2D and TGA images format without any issues.
But, for example, BMP image format (that provide 4 byte DIB alignment),
could have an issues (at least for 24 bits per pixel images).

This mean, if you add one more image format support, make sure you
care about byte alignment.
*/

#include "../vfs/vfs.h"
#include "../math/math.h"
#include "texture.h"

namespace {

// Default anisotropy level
GLint AnisotropyLevelTex{0};
// Default filtering type.
sTextureFilter FilteringTex{};
// Default address mode.
sTextureWrap AddressModeTex{};
// Default alpha channel status (should we create if don't have it, or remove if have it).
bool AlphaTex{false};
// Alpha channel default color (used for alpha channel generation).
uint8_t ARedTex{0};
uint8_t AGreenTex{0};
uint8_t ABlueTex{0};
// Alpha channel default algorithm (used for alpha channel generation).
eAlphaCreateMode AFlagTex{eAlphaCreateMode::EQUAL};
// Default mip mapping type.
bool MipMapTex{true};

// we need only Width and Height, but, keep all this data for now,
// glGetTexLevelParameterfv() usage not an option, since it could stalls the OpenGL pipeline
// and require texture bind for current texture unit
// Width/Height contain final texture size
// SrcWidth/SrcHeight contain initial source image size, befor POT correction in case
// hardware don't support non-POT size textures
struct sTexture {
	int Width;		// Texture width
	int Height;		// Texture height
	int SrcWidth;		// Source image width
	int SrcHeight;		// Source image height
	int Bytes;		// Bytes per pixel
};

// Map with all loaded textures.
std::unordered_map<std::string, GLtexture> TexturesNameToIDMap;
std::unordered_map<GLtexture, sTexture> TexturesIDtoDataMap;

} // unnamed namespace

namespace texture {
int ReadTGA(std::unique_ptr<uint8_t[]> &PixelsArray, sFILE *pFile, int &DWidth, int &DHeight, int &DChanels);
} // namespace texture


/*
 * Set textures properties.
 */
void vw_SetTextureProp(const sTextureFilter &Filtering, GLint AnisotropyLevel, const sTextureWrap &AddressMode,
		       bool Alpha, eAlphaCreateMode AFlag, bool MipMap)
{
	FilteringTex = Filtering;
	AnisotropyLevelTex = AnisotropyLevel,
	AddressModeTex = AddressMode;
	AlphaTex = Alpha;
	AFlagTex = AFlag;
	MipMapTex = MipMap;
}

/*
 * Set textures alpha color.
 */
void vw_SetTextureAlpha(uint8_t nARed, uint8_t nAGreen, uint8_t nABlue)
{
	ARedTex = nARed;
	AGreenTex = nAGreen;
	ABlueTex = nABlue;
}

/*
 * Find texture by name.
 */
GLtexture vw_FindTextureByName(const std::string &Name)
{
	auto tmpTexture = TexturesNameToIDMap.find(Name);
	if (tmpTexture != TexturesNameToIDMap.end())
		return tmpTexture->second;

	return 0;
}

/*
 * Find texture's size by ID.
 */
bool vw_FindTextureSizeByID(GLtexture TextureID, float *Width, float *Height)
{
	auto tmpTexture = TexturesIDtoDataMap.find(TextureID);
	if (tmpTexture == TexturesIDtoDataMap.end())
		return false;

	if (Width)
		*Width = static_cast<float>(tmpTexture->second.Width);
	if (Height)
		*Height = static_cast<float>(tmpTexture->second.Height);

	return true;
}

/*
 * Release texture.
 */
void vw_ReleaseTexture(GLtexture TextureID)
{
	if (!TextureID)
		return;

	vw_DeleteTexture(TextureID);
	TexturesIDtoDataMap.erase(TextureID);

	for (auto &tmpTexture : TexturesNameToIDMap) {
		if (tmpTexture.second == TextureID) {
			TexturesNameToIDMap.erase(tmpTexture.first);
			return;
		}
	}

}

/*
 * Release all textures.
 */
void vw_ReleaseAllTextures()
{
	for (auto &tmpTexture : TexturesIDtoDataMap) {
		vw_DeleteTexture(tmpTexture.first);
	}
	TexturesIDtoDataMap.clear();
	TexturesNameToIDMap.clear();

	FilteringTex = sTextureFilter{};
	AnisotropyLevelTex = 0;
	AddressModeTex = sTextureWrap{};
	AlphaTex = false;
}

/*
 * Calculate power of two.
 */
static int PowerOfTwo(int Num)
{
	int tmpValue{1};

	while (tmpValue < Num) {
		tmpValue <<= 1;
	}
	return tmpValue;
}

/*
 * Resize image to closest power of two size.
 */
static void ResizeToPOT(std::unique_ptr<uint8_t[]> &PixelsArray, sTexture &Texture)
{
	if (!PixelsArray.get())
		return;

	// calculate closest power of two texture size
	int potWidth = PowerOfTwo(Texture.Width);
	int potHeight = PowerOfTwo(Texture.Height);

	if ((potWidth == Texture.Width) &&
	    (potHeight == Texture.Height))
		return;

	// don't copy pixel's array, but move, since we need resize it on next step
	std::unique_ptr<uint8_t[]> tmpPixelsArray{std::move(PixelsArray)};
	PixelsArray.reset(new uint8_t[potWidth * potHeight * Texture.Bytes]);

	// fill new buffer with default color and alpha (if we have alpha channel)
	uint8_t ColorF[]{ARedTex, AGreenTex, ABlueTex, 0};
	for (int i = 0; i < potWidth * potHeight * Texture.Bytes; i += Texture.Bytes) {
		memcpy(PixelsArray.get() + i, ColorF, Texture.Bytes);
	}

	// calculate stride
	int tmpStride = Texture.Width * Texture.Bytes;

	// copy pixels line by line
	for (int y = 0; y < Texture.Height; y++) {
		int tmpOffsetDst = (y + potHeight - Texture.Height) * potWidth * Texture.Bytes;
		int tmpOffsetSrc = y * tmpStride;
		memcpy(PixelsArray.get() + tmpOffsetDst, tmpPixelsArray.get() + tmpOffsetSrc, tmpStride);
	}

	// store new width and height
	Texture.Width = potWidth;
	Texture.Height = potHeight;
}

/*
 * Resize image to custom size.
 */
static void ResizeImage(int newWidth, int newHeight, std::unique_ptr<uint8_t[]> &PixelsArray, sTexture &Texture)
{
	if (!PixelsArray.get() || ((newWidth == Texture.Width) && (newHeight == Texture.Height)))
		return;

	// don't copy PixelsArray, but move, since we need resize it on next step
	std::unique_ptr<uint8_t[]> tmpPixelsArray{std::move(PixelsArray)};
	PixelsArray.reset(new uint8_t[newWidth * newHeight * Texture.Bytes]);

	// change size with nearest neighbor resizing algorithm for speed
	for (int j = 0; j < newHeight; j++) {
		int tmpOffset = ((j * Texture.Height) / newHeight) * Texture.Width;
		for (int i = 0; i < newWidth; i++) {
			memcpy(PixelsArray.get() + (i + j * newWidth) * Texture.Bytes,
			       tmpPixelsArray.get() + (tmpOffset + i * Texture.Width / newWidth) * Texture.Bytes,
			       sizeof(PixelsArray.get()[0]) * Texture.Bytes);
		}
	}

	// store new width and height
	Texture.Width = newWidth;
	Texture.Height = newHeight;
}

/*
 * Create alpha channel.
 */
static void CreateAlpha(std::unique_ptr<uint8_t[]> &PixelsArray, sTexture &Texture, eAlphaCreateMode AlphaFlag)
{
	if (!PixelsArray.get())
		return;

	// don't copy pixel's array, but move, since we need resize it on next step
	std::unique_ptr<uint8_t[]> tmpPixelsArray{std::move(PixelsArray)};
	PixelsArray.reset(new uint8_t[Texture.Width * Texture.Height * 4]);

	for(int i = 0; i < Texture.Height; i++) {
		int tmpOffsetDst = Texture.Width * 4 * i;
		int tmpOffsetSrc = Texture.Width * 3 * i;

		for(int j2 = 0; j2 < Texture.Width; j2++) {
			// copy color
			memcpy(PixelsArray.get() + tmpOffsetDst,
			       tmpPixelsArray.get() + tmpOffsetSrc,
			       sizeof(PixelsArray.get()[0]) * 3);

			// create alpha
			switch(AlphaFlag) {
			case eAlphaCreateMode::GREYSC:
				PixelsArray.get()[tmpOffsetDst + 3] = static_cast<uint8_t>(
					static_cast<float>(PixelsArray.get()[tmpOffsetDst]) / 255 * 28 +
					static_cast<float>(PixelsArray.get()[tmpOffsetDst + 1]) / 255 * 150 +
					static_cast<float>(PixelsArray.get()[tmpOffsetDst + 2]) / 255 * 76);
				break;

			case eAlphaCreateMode::EQUAL:
				if ((ABlueTex == PixelsArray.get()[tmpOffsetDst]) &&
				    (AGreenTex == PixelsArray.get()[tmpOffsetDst + 1]) &&
				    (ARedTex == PixelsArray.get()[tmpOffsetDst + 2]))
					PixelsArray.get()[tmpOffsetDst + 3] = 0;
				else
					PixelsArray.get()[tmpOffsetDst + 3] = 255;
				break;

			default:
				PixelsArray.get()[tmpOffsetDst + 3] = 255;
				break;
			}

			tmpOffsetDst += 4;
			tmpOffsetSrc += 3;
		}
	}

	// store new bytes per pixel
	Texture.Bytes = 4;
}

/*
 * Remove alpha channel.
 */
static void RemoveAlpha(std::unique_ptr<uint8_t[]> &PixelsArray, sTexture &Texture)
{
	if (!PixelsArray.get())
		return;

	// don't copy pixel's array, but move, since we need resize it on next step
	std::unique_ptr<uint8_t[]> tmpPixelsArray{std::move(PixelsArray)};
	PixelsArray.reset(new uint8_t[Texture.Width * Texture.Height * 3]);

	for(int i = 0; i < Texture.Height; i++) {
		int tmpOffsetDst = Texture.Width * 3 * i;
		int tmpOffsetSrc = Texture.Width * 4 * i;

		for(int j = 0; j < Texture.Width; j++) {
			memcpy(PixelsArray.get() + tmpOffsetDst,
			       tmpPixelsArray.get() + tmpOffsetSrc,
			       sizeof(PixelsArray.get()[0]) * 3);

			tmpOffsetDst += 3;
			tmpOffsetSrc += 4;
		}
	}

	// store new bytes per pixel
	Texture.Bytes = 3;
}

/*
 * Convert supported image file format to VW2D format.
 */
void vw_ConvertImageToVW2D(const std::string &SrcName, const std::string &DestName)
{
	if (SrcName.empty() || DestName.empty())
		return;

	int tmpWidth{0};
	int tmpHeight{0};
	int tmpChanels{0};
	std::unique_ptr<uint8_t[]> tmpPixelsArray{};
	eLoadTextureAs LoadAs{eLoadTextureAs::TGA};

	std::unique_ptr<sFILE> pFile = vw_fopen(SrcName);
	if (pFile == nullptr) {
		std::cerr << __func__ << "(): " << "Unable to found " << SrcName << "\n";
		return;
	}

	// check extension
	if (vw_CheckFileExtension(SrcName, ".tga"))
		LoadAs = eLoadTextureAs::TGA;
	else
		std::cerr << __func__ << "(): " << "Format not supported " << SrcName << "\n";

	switch (LoadAs) {
	case eLoadTextureAs::TGA:
		texture::ReadTGA(tmpPixelsArray, pFile.get(), tmpWidth, tmpHeight, tmpChanels);
		break;

	default:
		std::cerr << __func__ << "(): " << "Unable to load " << SrcName << "\n";
		return;
	}

	if (!tmpPixelsArray.get()) {
		std::cerr << __func__ << "(): " << "Unable to load " << SrcName << "\n";
		return;
	}

	vw_fclose(pFile);

	// write data to disk
	SDL_RWops *FileVW2D;
	FileVW2D = SDL_RWFromFile(DestName.c_str(), "wb");
	if (!FileVW2D) {
		std::cerr << __func__ << "(): " << "Can't create " << DestName << " file on disk.\n";
		return;
	}

	char tmpSign[4]{'V','W','2','D'};
	SDL_RWwrite(FileVW2D, tmpSign, 4, 1);

	SDL_RWwrite(FileVW2D, &tmpWidth, sizeof(tmpWidth), 1);
	SDL_RWwrite(FileVW2D, &tmpHeight, sizeof(tmpHeight), 1);
	SDL_RWwrite(FileVW2D, &tmpChanels, sizeof(tmpChanels), 1);
	SDL_RWwrite(FileVW2D, tmpPixelsArray.get(), tmpWidth * tmpHeight * tmpChanels, 1);

	SDL_RWclose(FileVW2D);
}

/*
 * Load texture from file.
 */
GLtexture vw_LoadTexture(const std::string &TextureName, eTextureCompressionType CompressionType,
			 eLoadTextureAs LoadAs, int NeedResizeW, int NeedResizeH)
{
	if (TextureName.empty())
		return 0;

	int DWidth{0};
	int DHeight{0};
	int DChanels{0};
	// std::unique_ptr, we need only memory allocation without container's features
	// don't use std::vector here, since it allocates AND value-initializes
	std::unique_ptr<uint8_t[]> tmpPixelsArray{};

	std::unique_ptr<sFILE> pFile = vw_fopen(TextureName);
	if (!pFile) {
		std::cerr << __func__ << "(): " << "Unable to found " << TextureName << "\n";
		return 0;
	}

	// check extension
	if (LoadAs == eLoadTextureAs::AUTO) {
		if (vw_CheckFileExtension(TextureName, ".tga"))
			LoadAs = eLoadTextureAs::TGA;
		else if (vw_CheckFileExtension(TextureName, ".vw2d"))
			LoadAs = eLoadTextureAs::VW2D;
		else
			std::cerr << __func__ << "(): " << "Format not supported " << TextureName << "\n";
	}

	// load texture
	switch(LoadAs) {
	case eLoadTextureAs::TGA:
		texture::ReadTGA(tmpPixelsArray, pFile.get(), DWidth, DHeight, DChanels);
		break;

	case eLoadTextureAs::VW2D:
		pFile->fseek(4, SEEK_SET); // skip sign "VW2D"
		pFile->fread(&DWidth, sizeof(DWidth), 1);
		pFile->fread(&DHeight, sizeof(DHeight), 1);
		pFile->fread(&DChanels, sizeof(DChanels), 1);
		tmpPixelsArray.reset(new uint8_t[DWidth * DHeight * DChanels]);
		pFile->fread(tmpPixelsArray.get(), DWidth * DHeight * DChanels, 1);
		break;

	default:
		return 0;
	}

	if (!tmpPixelsArray.get()) {
		std::cerr << __func__ << "(): " << "Unable to load " << TextureName << "\n";
		return 0;
	}

	vw_fclose(pFile);

	return vw_CreateTextureFromMemory(TextureName, tmpPixelsArray, DWidth, DHeight, DChanels,
					  CompressionType, NeedResizeW, NeedResizeH);
}

/*
 * Create texture from memory.
 */
GLtexture vw_CreateTextureFromMemory(const std::string &TextureName, std::unique_ptr<uint8_t[]> &PixelsArray,
				     int ImageWidth, int ImageHeight, int ImageChanels,
				     eTextureCompressionType CompressionType,
				     int NeedResizeW, int NeedResizeH, bool NeedDuplicateCheck)
{
	if (TextureName.empty() || !PixelsArray.get() || (ImageWidth <= 0) || (ImageHeight <= 0))
		return 0;

	// check for availability, probably, we already have it loaded
	if (NeedDuplicateCheck) {
		GLtexture tmpTextureID = vw_FindTextureByName(TextureName);
		if (tmpTextureID) {
			std::cout << "Texture already loaded: " << TextureName << "\n";
			return tmpTextureID;
		}
	}

	sTexture newTexture{};
	newTexture.Width = ImageWidth;
	newTexture.Height = ImageHeight;
	newTexture.Bytes = ImageChanels;

	// if we have alpha channel, but don't need them - remove
	if ((newTexture.Bytes == 4) && !AlphaTex)
		RemoveAlpha(PixelsArray, newTexture);
	// if we don't have alpha channel, but need them - create
	else if ((newTexture.Bytes == 3) && (AlphaTex))
		CreateAlpha(PixelsArray, newTexture, AFlagTex);

	// Note, in case of resize, we should provide width and height (but not just one of them).
	if (NeedResizeW && NeedResizeH)
		ResizeImage(NeedResizeW, NeedResizeH, PixelsArray, newTexture);

	// in case we change size, it is important to store "source" (initial) size
	// that need for 2D rendering calculation, when we operate with pixels
	// and don't count on NPOT resize
	newTexture.SrcWidth = newTexture.Width;
	newTexture.SrcHeight = newTexture.Height;

	// if hardware don't support NPOT textures, forced to resize image manually
	if (!vw_GetDevCaps().ARB_texture_non_power_of_two)
		ResizeToPOT(PixelsArray, newTexture);

	GLtexture TextureID = vw_BuildTexture(PixelsArray, newTexture.Width, newTexture.Height,
					      MipMapTex, newTexture.Bytes, CompressionType);

	if (!TextureID)
		return 0;

	vw_SetTextureFiltering(FilteringTex);
	if (AnisotropyLevelTex)
		vw_SetTextureAnisotropy(AnisotropyLevelTex);
	vw_SetTextureAddressMode(AddressModeTex);
	vw_BindTexture(0, 0);

	// create new entries
	TexturesNameToIDMap.emplace(TextureName, TextureID);
	TexturesIDtoDataMap.emplace(TextureID, newTexture);

	std::cout << "Texture created from memory: " << TextureName << "\n";
	return TextureID;
}
