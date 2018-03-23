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

/*
Note, all texture-related code aimed to 1 byte texture alignment.
Plus, default OpenGL setup also aimed for 1 byte alignment:
glPixelStorei(GL_PACK_ALIGNMENT, 1);
glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

Code work with VW2D and TGA images format without any issues.
But, for example, BMP image format (that provide 4 byte dib alignment),
could have an issues (at least for 24 bits per pixel images).

This mean, if you add one more image format support, make sure you
care about byte alignment.
*/

#include "../graphics/graphics.h"
#include "../vfs/vfs.h"
#include "../math/math.h"
#include "texture.h"

namespace {

// Default filtering type (near, linear, ... ).
int FilteringTex{RI_MAGFILTER_POINT | RI_MINFILTER_POINT | RI_MIPFILTER_POINT};
// Default address mode (wrap, clamp ... ).
int Address_ModeTex{RI_WRAP_U | RI_WRAP_V};
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
// Map with all loaded textures.
std::unordered_map<std::string, sTexture> TexturesMap;

} // unnamed namespace

namespace texture {
int ReadTGA(std::vector<uint8_t> &DIB, sFILE *pFile, int &DWidth, int &DHeight, int &DChanels);
} // namespace texture


/*
 * Set textures properties.
 */
void vw_SetTextureProp(int nFiltering, int nAddress_Mode, bool nAlpha, eAlphaCreateMode nAFlag, bool nMipMap)
{
	FilteringTex = nFiltering;
	Address_ModeTex = nAddress_Mode;
	AlphaTex = nAlpha;
	AFlagTex = nAFlag;
	MipMapTex = nMipMap;
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
sTexture *vw_FindTextureByName(const std::string &Name)
{
	auto tmpTexture = TexturesMap.find(Name);

	if (tmpTexture != TexturesMap.end())
		return &tmpTexture->second;

	return nullptr;
}

/*
 * Release texture.
 */
void vw_ReleaseTexture(sTexture *Texture)
{
	if (!Texture)
		return;

	// we are forced to check them all in cycle, since our key is texture's name
	for (const auto &tmpTexture : TexturesMap) {
		if ((&tmpTexture.second == Texture)) {
			vw_DeleteTexture(Texture->TextureID);
			TexturesMap.erase(tmpTexture.first);
			// forced to leave - current iterator invalidated by erase()
			return;
		}
	}
}

/*
 * Release all textures.
 */
void vw_ReleaseAllTextures()
{
	for (auto &tmpTexture : TexturesMap) {
		vw_DeleteTexture(tmpTexture.second.TextureID);
	}
	TexturesMap.clear();

	FilteringTex = RI_MAGFILTER_POINT | RI_MINFILTER_POINT | RI_MIPFILTER_POINT;
	Address_ModeTex = RI_WRAP_U | RI_WRAP_V;
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
static void ResizeToPOT(std::vector<uint8_t> &DIB, sTexture &Texture)
{
	if (DIB.empty())
		return;

	// calculate closest power of two texture size
	int potWidth = PowerOfTwo(Texture.Width);
	int potHeight = PowerOfTwo(Texture.Height);

	if ((potWidth == Texture.Width) &&
	    (potHeight == Texture.Height))
		return;

	// don't copy DIB, but move, since we need resize it on next step
	std::vector<uint8_t> tmpDIB{std::move(DIB)};
	DIB.resize(potWidth * potHeight * Texture.Bytes);

	// fill new buffer with default color and alpha (if we have alpha channel)
	uint8_t ColorF[]{Texture.ARed, Texture.AGreen, Texture.ABlue, 0};
	for (int i = 0; i < potWidth * potHeight * Texture.Bytes; i += Texture.Bytes) {
		memcpy(DIB.data() + i, ColorF, Texture.Bytes);
	}

	// calculate stride
	int tmpStride = Texture.Width * Texture.Bytes;

	// copy pixels line by line
	for (int y = 0; y < Texture.Height; y++) {
		int tmpOffsetDst = (y + potHeight - Texture.Height) * potWidth * Texture.Bytes;
		int tmpOffsetSrc = y * tmpStride;
		memcpy(DIB.data() + tmpOffsetDst, tmpDIB.data() + tmpOffsetSrc, tmpStride);
	}

	// store new width and height
	Texture.Width = potWidth;
	Texture.Height = potHeight;
}

/*
 * Resize image to custom size.
 */
static void ResizeImage(int newWidth, int newHeight, std::vector<uint8_t> &DIB, sTexture &Texture)
{
	if (DIB.empty() || ((newWidth == Texture.Width) && (newHeight == Texture.Height)))
		return;

	// don't copy DIB, but move, since we need resize it on next step
	std::vector<uint8_t> tmpDIB{std::move(DIB)};
	DIB.resize(newWidth * newHeight * Texture.Bytes);

	// change size with nearest neighbor resizing algorithm for speed
	for (int j = 0; j < newHeight; j++) {
		int tmpOffset = ((j * Texture.Height) / newHeight) * Texture.Width;
		for (int i = 0; i < newWidth; i++) {
			memcpy(DIB.data() + (i + j * newWidth) * Texture.Bytes,
			       tmpDIB.data() + (tmpOffset + i * Texture.Width / newWidth) * Texture.Bytes,
			       sizeof(DIB[0]) * Texture.Bytes);
		}
	}

	// store new width and height
	Texture.Width = newWidth;
	Texture.Height = newHeight;
}

/*
 * Create alpha channel.
 */
static void CreateAlpha(std::vector<uint8_t> &DIB, sTexture &Texture, eAlphaCreateMode AlphaFlag)
{
	if (DIB.empty())
		return;

	// don't copy DIB, but move, since we need resize it on next step
	std::vector<uint8_t> tmpDIB{std::move(DIB)};
	DIB.resize(Texture.Width * Texture.Height * 4);

	for(int i = 0; i < Texture.Height; i++) {
		int tmpOffsetDst = Texture.Width * 4 * i;
		int tmpOffsetSrc = Texture.Width * 3 * i;

		for(int j2 = 0; j2 < Texture.Width; j2++) {
			// copy color
			memcpy(DIB.data() + tmpOffsetDst, tmpDIB.data() + tmpOffsetSrc, sizeof(DIB[0]) * 3);

			// create alpha
			switch(AlphaFlag) {
			case eAlphaCreateMode::GREYSC:
				DIB[tmpOffsetDst + 3] = (uint8_t)(((float)DIB[tmpOffsetDst] / 255) * 28) +
							(uint8_t)(((float)DIB[tmpOffsetDst + 1] / 255) * 150) +
							(uint8_t)(((float)DIB[tmpOffsetDst + 2] / 255) * 76);
				break;

			case eAlphaCreateMode::EQUAL:
				if ((Texture.ABlue == DIB[tmpOffsetDst]) &&
				    (Texture.AGreen == DIB[tmpOffsetDst + 1]) &&
				    (Texture.ARed == DIB[tmpOffsetDst + 2]))
					DIB[tmpOffsetDst + 3] = 0;
				else
					DIB[tmpOffsetDst + 3] = 255;
				break;

			default:
				DIB[tmpOffsetDst + 3] = 255;
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
static void RemoveAlpha(std::vector<uint8_t> &DIB, sTexture &Texture)
{
	if (DIB.empty())
		return;

	// don't copy DIB, but move, since we need resize it on next step
	std::vector<uint8_t> tmpDIB{std::move(DIB)};
	DIB.resize(Texture.Width * Texture.Height * 3);

	for(int i = 0; i < Texture.Height; i++) {
		int tmpOffsetDst = Texture.Width * 3 * i;
		int tmpOffsetSrc = Texture.Width * 4 * i;

		for(int j = 0; j < Texture.Width; j++) {
			memcpy(DIB.data() + tmpOffsetDst, tmpDIB.data() + tmpOffsetSrc, sizeof(DIB[0]) * 3);

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
	std::vector<uint8_t> tmpImage{};
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
		texture::ReadTGA(tmpImage, pFile.get(), tmpWidth, tmpHeight, tmpChanels);
		break;

	default:
		std::cerr << __func__ << "(): " << "Unable to load " << SrcName << "\n";
		return;
	}

	if (tmpImage.empty()) {
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
	SDL_RWwrite(FileVW2D, tmpImage.data(), tmpWidth * tmpHeight * tmpChanels, 1);

	SDL_RWclose(FileVW2D);
}

/*
 * Load texture from file.
 */
sTexture *vw_LoadTexture(const std::string &TextureName, int CompressionType,
			 eLoadTextureAs LoadAs, int NeedResizeW, int NeedResizeH)
{
	if (TextureName.empty())
		return nullptr;

	int DWidth{0};
	int DHeight{0};
	int DChanels{0};
	std::vector<uint8_t> tmpImage{};

	std::unique_ptr<sFILE> pFile = vw_fopen(TextureName);
	if (!pFile) {
		std::cerr << __func__ << "(): " << "Unable to found " << TextureName << "\n";
		return nullptr;
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
		texture::ReadTGA(tmpImage, pFile.get(), DWidth, DHeight, DChanels);
		break;

	case eLoadTextureAs::VW2D:
		pFile->fseek(4, SEEK_SET); // skip sign "VW2D"
		pFile->fread(&DWidth, sizeof(DWidth), 1);
		pFile->fread(&DHeight, sizeof(DHeight), 1);
		pFile->fread(&DChanels, sizeof(DChanels), 1);
		tmpImage.resize(DWidth * DHeight * DChanels);
		pFile->fread(tmpImage.data(), DWidth * DHeight * DChanels, 1);
		break;

	default:
		return nullptr;
	}

	if (tmpImage.empty()) {
		std::cerr << __func__ << "(): " << "Unable to load " << TextureName << "\n";
		return nullptr;
	}

	vw_fclose(pFile);

	return vw_CreateTextureFromMemory(TextureName, tmpImage, DWidth, DHeight, DChanels,
					  CompressionType, NeedResizeW, NeedResizeH);
}

/*
 * Create texture from memory.
 */
sTexture *vw_CreateTextureFromMemory(const std::string &TextureName, std::vector<uint8_t> &DIB, int DIBWidth,
				     int DIBHeight, int DIBChanels, int CompressionType, int NeedResizeW,
				     int NeedResizeH, bool NeedDuplicateCheck)
{
	if (TextureName.empty() || DIB.empty() || (DIBWidth <= 0) || (DIBHeight <= 0))
		return nullptr;

	// check for availability, probably, we already have it loaded
	if (NeedDuplicateCheck) {
		sTexture *tmpTexture = vw_FindTextureByName(TextureName);
		if (tmpTexture) {
			std::cout << "Texture already loaded: " << TextureName << "\n";
			return tmpTexture;
		}
	}

	TexturesMap[TextureName].ARed = ARedTex;
	TexturesMap[TextureName].AGreen = AGreenTex;
	TexturesMap[TextureName].ABlue = ABlueTex;
	TexturesMap[TextureName].TextureID = 0;
	TexturesMap[TextureName].Width = DIBWidth;
	TexturesMap[TextureName].Height = DIBHeight;
	TexturesMap[TextureName].Bytes = DIBChanels;

	// if we have alpha channel, but don't need them - remove
	if ((TexturesMap[TextureName].Bytes == 4) && !AlphaTex)
		RemoveAlpha(DIB, TexturesMap[TextureName]);
	// if we don't have alpha channel, but need them - create
	else if ((TexturesMap[TextureName].Bytes == 3) && (AlphaTex))
		CreateAlpha(DIB, TexturesMap[TextureName], AFlagTex);

	// Note, in case of resize, we should provide width and height (but not just one of them).
	if (NeedResizeW && NeedResizeH)
		ResizeImage(NeedResizeW, NeedResizeH, DIB, TexturesMap[TextureName]);

	// in case we change size, it is important to store "source" (initial) size
	// that need for 2D rendering calculation, when we operate with pixels
	// and don't count on NPOT resize
	TexturesMap[TextureName].SrcWidth = TexturesMap[TextureName].Width;
	TexturesMap[TextureName].SrcHeight = TexturesMap[TextureName].Height;

	// if hardware don't support NPOT textures, forced to resize them manually
	if (!vw_GetDevCaps()->TextureNPOTSupported)
		ResizeToPOT(DIB, TexturesMap[TextureName]);

	TexturesMap[TextureName].TextureID = vw_BuildTexture(DIB.data(), TexturesMap[TextureName].Width,
							     TexturesMap[TextureName].Height, MipMapTex,
							     TexturesMap[TextureName].Bytes, CompressionType);
	vw_SetTextureFiltering(FilteringTex);
	vw_SetTextureAddressMode(Address_ModeTex);
	vw_BindTexture(0, 0);

	std::cout << "Texture created from memory: " << TextureName << "\n";
	return &TexturesMap[TextureName];
}
