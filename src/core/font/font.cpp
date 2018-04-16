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

// TODO move to std::string, probably, we need move directly to std::u32string (utf32)
//      for all text rendering and text input (profile names)

// TODO remove va_start, use variadic templates instead (?)

// TODO provide static vw_DrawFont(), dynamic realization we have now,
//      static should create array with text blocks as key and VBO/VAO/IBO (and other data)
//      as value for fast rendering.

// TODO switch to std::unordered_multimap from std::forward_list
//      that will allow fast access and we could manually check second key (FontSize)

// TODO since we use RI_TRIANGLES, use 4 vertices + index buffer for vw_SendVertices()
//      instead of 6 vertices, so, we send 4 vertices and index buffer for 6 elements,
//      something like {1, 2, 3, 3, 4, 1}
//                               ^  ^  ^ second triangle indexes
//                      ^  ^  ^ first triangle indexes

// NOTE in future, use make_unique() to make unique_ptr-s (since C++14)

#include "../texture/texture.h"
#include "../graphics/graphics.h"
#include "../font/font.h"
#include "../camera/camera.h"
#include "../math/math.h"
#include "../vfs/vfs.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <stdarg.h> // va_start

namespace {

// FreeType related stuff.
FT_Library InternalLibrary;
FT_Face InternalFace;
// std::unique_ptr, we need only memory allocation without container's features
// don't use std::vector here, since it allocates AND value-initializes
std::unique_ptr<uint8_t[]> InternalFontBuffer{};
// Font settings.
int InternalFontSize{0};
int GlobalFontOffsetY{0};

struct sFontChar {
	char32_t UTF32; // key element 1 (UTF32 code)
	int FontSize; // key element 2 (character generated size)

	GLtexture Texture{0}; // font character texture
	int TexturePositionLeft;
	int TexturePositionRight;
	int TexturePositionTop;
	int TexturePositionBottom;

	// font character metrics
	int Width;
	int Height;
	int Left;
	int Top;
	float AdvanceX;

	// constructor
	sFontChar(char32_t _UTF32, int _FontSize,
		  int _TexturePositionLeft, int _TexturePositionRight,
		  int _TexturePositionTop, int _TexturePositionBottom,
		  int _Width, int _Height, int _Left, int _Top, float _AdvanceX) :
		UTF32{_UTF32},
		FontSize{_FontSize},
		TexturePositionLeft{_TexturePositionLeft},
		TexturePositionRight{_TexturePositionRight},
		TexturePositionTop{_TexturePositionTop},
		TexturePositionBottom{_TexturePositionBottom},
		Width{_Width},
		Height{_Height},
		Left{_Left},
		Top{_Top},
		AdvanceX{_AdvanceX}
	{}
	// trick for forward_list<unique_ptr<T>> work with iterator
	bool CheckUTF32andSize(char32_t _UTF32)
	{
		return ((UTF32 == _UTF32) &&
			(FontSize == InternalFontSize));
	}
};

// List with connected font characters.
std::forward_list<std::unique_ptr<sFontChar>> FontCharsList;
// Local draw buffer, that dynamically allocate memory at maximum required
// size only one time per game execution. Don't use std::vector here,
// since it have poor performance compared to std::unique_ptr.
std::unique_ptr<float []> DrawBuffer{};
unsigned int DrawBufferCurrentPosition{0};
unsigned int DrawBufferSize{0};
// space character utf32 code
constexpr char32_t SpaceUTF32{32};

} // unnamed namespace


/*
 * Font initialization by font name (path to file).
 */
int vw_InitFont(const std::string &FontName)
{
	if (FontName.empty())
		return ERR_PARAMETERS;

	if (FT_Init_FreeType(&InternalLibrary)) {
		std::cerr << __func__ << "(): " << "Can't initialize library, font: " << FontName << "\n";
		return ERR_EXT_RES;
	}

	if (InternalFontBuffer.get())
		InternalFontBuffer.reset();

	std::unique_ptr<sFILE> FontFile = vw_fopen(FontName);
	if (!FontFile) {
		std::cerr << __func__ << "(): " << "Can't open font file: " << FontName << "\n";
		return ERR_FILE_NOT_FOUND;
	}

	FontFile->fseek(0, SEEK_END);
	int FontBufferSize = FontFile->ftell();
	FontFile->fseek(0, SEEK_SET);

	InternalFontBuffer.reset(new uint8_t[FontBufferSize]);
	FontFile->fread(InternalFontBuffer.get(), FontBufferSize, 1);

	vw_fclose(FontFile);

	if (FT_New_Memory_Face(InternalLibrary, InternalFontBuffer.get(), FontBufferSize, 0, &InternalFace)) {
		std::cerr << __func__ << "(): " << "Can't create font face from memory, font: " << FontName << "\n";
		return ERR_EXT_RES;
	}

	std::cout << "Font initialized: " << FontName << "\n\n";
	return 0;
}

/*
 * Set current font size.
 */
void vw_SetFontSize(int FontSize)
{
	InternalFontSize = FontSize;
}

/*
 * Set font offset.
 */
void vw_SetFontOffsetY(int NewOffsetY)
{
	GlobalFontOffsetY = NewOffsetY;
}

/*
 * Find font by UTF32 code.
 */
static sFontChar *FindFontCharByUTF32(char32_t UTF32)
{
	// a bit tricky, since we can't work with iterator for
	// forward_list<unique_ptr<T>> in usual way
	for (const auto &tmpChar : FontCharsList) {
		if (tmpChar->CheckUTF32andSize(UTF32))
			return tmpChar.get();
	}

	return nullptr;
}

/*
 * Check font character by UTF32 code.
 */
bool vw_CheckFontCharByUTF32(char32_t UTF32)
{
	return (FindFontCharByUTF32(UTF32) != nullptr);
}

/*
 * Release all font characters and created for this characters textures.
 */
void vw_ReleaseAllFontChars()
{
	// release all font characters
	while (!FontCharsList.empty()) {
		vw_ReleaseTexture(FontCharsList.front()->Texture);
		FontCharsList.pop_front();
	}
	// reset list
	FontCharsList.clear();
}

/*
 *  Shutdown font.
 */
void vw_ShutdownFont()
{
	if (InternalFontBuffer.get())
		InternalFontBuffer.reset();
}

/*
 * Load data and generate font character.
 */
static sFontChar *LoadFontChar(char32_t UTF32)
{
	// setup parameters
	if (FT_Set_Char_Size(InternalFace, InternalFontSize << 6, InternalFontSize << 6, 96, 96)) {
		std::cerr << __func__ << "(): " << "Can't set char size " << InternalFontSize << "\n";
		return nullptr;
	}
	// load glyph
	if (FT_Load_Char( InternalFace, UTF32, FT_LOAD_RENDER | FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT)) {
		std::cerr << __func__ << "(): " << "Can't load glyph: " << UTF32 << "\n";
		return nullptr;
	}

	// create new character
	FontCharsList.push_front(std::unique_ptr<sFontChar>(new sFontChar(UTF32, InternalFontSize,
				 0, InternalFace->glyph->bitmap.width, 0, InternalFace->glyph->bitmap.rows,
				 InternalFace->glyph->bitmap.width, InternalFace->glyph->bitmap.rows,
				 InternalFace->glyph->bitmap_left, InternalFace->glyph->bitmap_top,
				 InternalFace->glyph->advance.x / 64.0f)));

	if ((FontCharsList.front()->Width > 0) && (FontCharsList.front()->Height > 0)) {
		// buffer for RGBA, data for font characters texture, initialize it with white color (255)
		std::vector<uint8_t> tmpPixels(FontCharsList.front()->Width * FontCharsList.front()->Height * 4, 255 /*white*/);
		// convert greyscale to RGB+Alpha (32bits), now we need correct only alpha channel
		for (int j = 0; j < FontCharsList.front()->Height; j++) {
			int StrideSrc = j * FontCharsList.front()->Width * 4;
			int StrideDst = (FontCharsList.front()->Height - j - 1) * FontCharsList.front()->Width;
			for (int i = 0; i < FontCharsList.front()->Width; i++) {
				// alpha channel
				memcpy(tmpPixels.data() + StrideSrc + i * 4 + 3,
				       InternalFace->glyph->bitmap.buffer + StrideDst + i,
				       1);
			}
		}

		// fake texture file name based on font's size and UTF32 code
		std::string FakeTextureFileName{"fontsize_" + std::to_string(InternalFontSize) +
						"_character_" + std::to_string(UTF32)};

		vw_SetTextureProp(eTextureBasicFilter::BILINEAR, 0,
				  eTextureWrapMode::CLAMP_TO_EDGE, true, eAlphaCreateMode::GREYSC, false);
		FontCharsList.front()->Texture = vw_CreateTextureFromMemory(FakeTextureFileName, tmpPixels,
									    FontCharsList.front()->Width,
									    FontCharsList.front()->Height,
									    4, 0, 0, 0, false);
	}

	std::cout << "Font character was created for size: "
		  << InternalFontSize << ",  char: '"
		  << ConvertUTF8.to_bytes(UTF32) << "',  code: "
		  << "0x" << std::uppercase << std::hex << UTF32 << std::dec << "\n";

	return FontCharsList.front().get();
}

/*
 * Generate font characters by list.
 */
int vw_GenerateFontChars(int FontTextureWidth, int FontTextureHeight, const std::unordered_set<char32_t> &CharsSetUTF32)
{
	if (CharsSetUTF32.empty())
		return ERR_PARAMETERS;

	std::cout << "Font characters generation start.\n";

	// buffer for RGBA, data for font characters texture
	// make sure, DIB filled by black and alpha set to zero (0), or we will have white borders on each character
	std::vector<uint8_t> DIB(FontTextureWidth * FontTextureHeight * 4, 0 /*black + transparent*/);

	// initial setup
	if (FT_Set_Char_Size(InternalFace, InternalFontSize << 6, InternalFontSize << 6, 96, 96)) {
		std::cerr << __func__ << "(): " << "Can't set char size " << InternalFontSize << "\n";
		return ERR_EXT_RES;
	}

	// create one large bitmap with all font characters from list
	int CurrentDIBX{0};
	int CurrentDIBY{0};
	int EdgingSpace{2};
	int MaxHeightInCurrentLine{0};
	for (const auto &CurrentChar : CharsSetUTF32) {
		// load glyph
		if (FT_Load_Char(InternalFace, CurrentChar, FT_LOAD_RENDER | FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT)) {
			std::cerr << __func__ << "(): " << "Can't load Char: " << CurrentChar << "\n";
			return ERR_EXT_RES;
		}

		FontCharsList.push_front(std::unique_ptr<sFontChar>(new sFontChar(CurrentChar, InternalFontSize,
					 0, 0, 0, 0,
					 InternalFace->glyph->bitmap.width, InternalFace->glyph->bitmap.rows,
					 InternalFace->glyph->bitmap_left, InternalFace->glyph->bitmap_top,
					 InternalFace->glyph->advance.x / 64.0f)));

		// move to next line in bitmap if not enough space
		if (CurrentDIBX + FontCharsList.front()->Width > FontTextureWidth) {
			CurrentDIBX = 0;
			CurrentDIBY += MaxHeightInCurrentLine + EdgingSpace;
			MaxHeightInCurrentLine = 0;
		}
		// looks like no more space left at all, fail
		if (CurrentDIBY + FontCharsList.front()->Height > FontTextureHeight) {
			std::cerr << __func__ << "(): " << "Can't generate all font chars in one texture.\n"
				  << "Too many chars or too small texture size!\n";
			break;
		}

		// copy glyph into bitmap
		uint8_t ColorRGB[3]{255, 255, 255};
		for (int j = 0; j < FontCharsList.front()->Height; j++) {
			unsigned int tmpOffset = (FontTextureHeight - CurrentDIBY - j - 1) * FontTextureWidth * 4;
			for (int i = 0; i < FontCharsList.front()->Width; i++) {
				memcpy(DIB.data() + tmpOffset + (CurrentDIBX + i) * 4,
				       ColorRGB,
				       3);
				memcpy(DIB.data() + tmpOffset + (CurrentDIBX + i) * 4 + 3,
				       InternalFace->glyph->bitmap.buffer + j * FontCharsList.front()->Width + i,
				       1);
			}
		}

		// setup new character
		FontCharsList.front()->TexturePositionLeft = CurrentDIBX;
		FontCharsList.front()->TexturePositionRight = CurrentDIBX + FontCharsList.front()->Width;
		FontCharsList.front()->TexturePositionTop = CurrentDIBY;
		FontCharsList.front()->TexturePositionBottom = CurrentDIBY + FontCharsList.front()->Height;

		// detect new line position by height
		if (MaxHeightInCurrentLine < FontCharsList.front()->Height)
			MaxHeightInCurrentLine = FontCharsList.front()->Height;
		CurrentDIBX += FontCharsList.front()->Width + EdgingSpace;
	}

	// create texture from bitmap
	vw_SetTextureProp(eTextureBasicFilter::BILINEAR, 0,
			  eTextureWrapMode::CLAMP_TO_EDGE, true, eAlphaCreateMode::GREYSC, false);
	GLtexture FontTexture = vw_CreateTextureFromMemory("auto_generated_texture_for_fonts", DIB,
							   FontTextureWidth, FontTextureHeight, 4, 0);
	if (!FontTexture) {
		std::cerr << __func__ << "(): " << "Can't create font texture.\n";
		return ERR_MEM;
	}

	// setup texture to all font characters from list
	for (const auto &CurrentChar : CharsSetUTF32) {
		sFontChar *tmpChar = FindFontCharByUTF32(CurrentChar);
		if (tmpChar)
			tmpChar->Texture = FontTexture;
	}

	std::cout << "Font characters generation end.\n\n";
	return 0;
}

/*
 * Add data to local draw buffer.
 */
static inline void AddToDrawBuffer(float CoordX, float CoordY, float TextureU, float TextureV)
{
	DrawBuffer[DrawBufferCurrentPosition++] = CoordX;
	DrawBuffer[DrawBufferCurrentPosition++] = CoordY;
	DrawBuffer[DrawBufferCurrentPosition++] = TextureU;
	DrawBuffer[DrawBufferCurrentPosition++] = TextureV;
}

/*
 * Calculate width factors.
 */
static void CalculateWidthFactors(const std::u32string &Text, const float StrictWidth,
				  float &SpaceWidthFactor, float &FontWidthFactor)
{
	float LineWidth1{0}; // for StrictWidth > 0
	float LineWidth2{0}; // for StrictWidth < 0
	int SpaceCount{0};

	for (const auto &UTF32 : Text) {
		sFontChar *DrawChar = FindFontCharByUTF32(UTF32);
		if (!DrawChar)
			DrawChar = LoadFontChar(UTF32);

		// calculate space characters count in text
		if (UTF32 == SpaceUTF32) {
			if (StrictWidth > 0)
				SpaceCount++;
			else
				LineWidth2 += SpaceWidthFactor;
		} else {
			if (StrictWidth > 0)
				LineWidth1 += DrawChar->AdvanceX;
			else
				LineWidth2 += DrawChar->AdvanceX;
		}
	}

	// correction for factors in both cases
	if (StrictWidth > 0) {
		if ((StrictWidth > LineWidth1) && SpaceCount)
			SpaceWidthFactor = (StrictWidth - LineWidth1) / SpaceCount;
	} else {
		if (StrictWidth * (-1.0f) < LineWidth2)
			FontWidthFactor = StrictWidth / LineWidth2 * (-1.0f);
	}
}

/*
 * Calculate default space width.
 */
static void CalculateDefaultSpaceWidth(float &SpaceWidthFactor, float FontScale)
{
	if (!FindFontCharByUTF32(SpaceUTF32))
		LoadFontChar(SpaceUTF32);
	SpaceWidthFactor = FindFontCharByUTF32(SpaceUTF32)->AdvanceX * FontScale;
	// width factor for for space charecter, make sure, we have space width at least 65% of current font size
	if (SpaceWidthFactor < (InternalFontSize * 0.65f))
		SpaceWidthFactor = InternalFontSize * 0.65f;
}

/*
 * Draw buffer on texture change.
 * Caller should care about pointers. nullptr not allowed.
 */
static void DrawBufferOnTextureChange(GLtexture &CurrentTexture, const sFontChar *DrawChar)
{
	// draw all we have in buffer with current texture
	if (DrawBufferCurrentPosition) {
		vw_BindTexture(0, CurrentTexture);
		vw_SendVertices(RI_TRIANGLES, DrawBufferCurrentPosition / sizeof(DrawBuffer.get()[0]),
				RI_2f_XY | RI_1_TEX, DrawBuffer.get(), 4 * sizeof(DrawBuffer.get()[0]));
		DrawBufferCurrentPosition = 0;
	}
	// setup new texture
	CurrentTexture = DrawChar->Texture;
}

/*
 * Draw buffer on text end.
 */
static void DrawBufferOnTextEnd(GLtexture CurrentTexture)
{
	if (!DrawBufferCurrentPosition)
		return;

	vw_BindTexture(0, CurrentTexture);
	vw_SendVertices(RI_TRIANGLES, DrawBufferCurrentPosition / sizeof(DrawBuffer.get()[0]),
			RI_2f_XY | RI_1_TEX, DrawBuffer.get(), 4 * sizeof(DrawBuffer.get()[0]));
	DrawBufferCurrentPosition = 0;
}

/*
 * vw_DrawFontUTF32 wrapper with variadic arguments and conversion into utf32.
 */
int vw_DrawFont(int X, int Y, float StrictWidth, float ExpandWidth, float FontScale,
		 float R, float G, float B, float Transp, const char *Text, ...)
{
	if (!Text)
		return ERR_PARAMETERS;
	// get string with variadic arguments
	// allocate buffer
	va_list ap;
	va_start(ap, Text);
	std::vector<char> buffer(1 + std::vsnprintf(nullptr, 0, Text, ap));
	va_end(ap);
	// get data into buffer
	va_start(ap, Text);
	std::vsnprintf(buffer.data(), buffer.size(), Text, ap);
	va_end(ap);
	// convert from utf8 into utf32
	const std::u32string UTF32String{ConvertUTF8.from_bytes(buffer.data())};

	return vw_DrawFontUTF32(X, Y, StrictWidth, ExpandWidth, FontScale, R, G, B, Transp, UTF32String);
}

/*
 * Draw text with current font. Origin is upper left corner.
 *
 * StrictWidth - strict text by width:
 *      if StrictWidth > 0, reduce space width only
 *      if StrictWidth < 0, reduce all font character's width
 * ExpandWidth - expand width to provided parameter
 */
int vw_DrawFontUTF32(int X, int Y, float StrictWidth, float ExpandWidth, float FontScale,
		     float R, float G, float B, float Transp, const std::u32string &Text)
{
	if (Text.empty())
		return ERR_PARAMETERS;
	if (Transp >= 1.0f)
		Transp = 1.0f;

	if (Y + InternalFontSize * FontScale < 0)
		return 0; // it's ok, we work in proper way here

	// start position on X axis for character
	float Xstart{X * 1.0f};
	// calculate default space width
	float SpaceWidthFactor{0};
	CalculateDefaultSpaceWidth(SpaceWidthFactor, FontScale);
	// width factor for characters
	float FontWidthFactor{1.0f};

	// calculate width factors
	if (StrictWidth)
		CalculateWidthFactors(Text, StrictWidth, SpaceWidthFactor, FontWidthFactor);

	// calculate text width, all characters that we already rendered
	float LineWidth{0};

	vw_SetTextureBlend(true, eTextureBlendFactor::SRC_ALPHA, eTextureBlendFactor::ONE_MINUS_SRC_ALPHA);
	vw_SetColor(R, G, B, Transp);
	GLtexture CurrentTexture{0};
	float ImageHeight{0.0f};
	float ImageWidth{0.0f};

	// combine calculated width factor and global width scale
	FontWidthFactor = FontScale*FontWidthFactor;

	// RI_TRIANGLES * (RI_2f_XYZ + RI_2f_TEX) * Text.size()
	unsigned int tmpDrawBufferSize = 6 * (2 + 2) * Text.size();
	if (tmpDrawBufferSize > DrawBufferSize) {
		DrawBufferSize = tmpDrawBufferSize;
		DrawBuffer.reset(new float[DrawBufferSize]);
	}
	DrawBufferCurrentPosition = 0;

	// draw all characters in text by blocks grouped by texture id
	for (const auto &UTF32 : Text) {
		// find current character
		sFontChar *DrawChar = FindFontCharByUTF32(UTF32);
		if (!DrawChar)
			DrawChar = LoadFontChar(UTF32);
		// for first character in text - setup texture by first character
		if (!CurrentTexture) {
			CurrentTexture = DrawChar->Texture;
			vw_FindTextureSizeByID(DrawChar->Texture, &ImageWidth, &ImageHeight);
		}

		// looks like texture should be changed
		if (CurrentTexture != DrawChar->Texture) {
			DrawBufferOnTextureChange(CurrentTexture, DrawChar);
			vw_FindTextureSizeByID(DrawChar->Texture, &ImageWidth, &ImageHeight);
		}

		// put into draw buffer all characters data, except spaces
		if (UTF32 != SpaceUTF32) {
			float DrawX{Xstart + DrawChar->Left * FontWidthFactor};
			float DrawY{Y + GlobalFontOffsetY + (InternalFontSize - DrawChar->Top) * FontScale};

			// texture's UV coordinates
			float U_Left{(DrawChar->TexturePositionLeft * 1.0f) / ImageWidth};
			float V_Top{(DrawChar->TexturePositionTop * 1.0f) / ImageHeight};
			float U_Right{(DrawChar->TexturePositionRight * 1.0f) / ImageWidth};
			float V_Bottom{(DrawChar->TexturePositionBottom * 1.0f) / ImageHeight};

			// first triangle
			AddToDrawBuffer(DrawX, DrawY, U_Left, V_Top);
			AddToDrawBuffer(DrawX, DrawY + DrawChar->Height * FontScale, U_Left, V_Bottom);
			AddToDrawBuffer(DrawX + DrawChar->Width * FontWidthFactor, DrawY + DrawChar->Height * FontScale,
					U_Right, V_Bottom);
			// second triangle
			AddToDrawBuffer(DrawX, DrawY, U_Left, V_Top);
			AddToDrawBuffer(DrawX + DrawChar->Width * FontWidthFactor, DrawY + DrawChar->Height * FontScale,
					U_Right, V_Bottom);
			AddToDrawBuffer(DrawX + DrawChar->Width * FontWidthFactor, DrawY, U_Right, V_Top);

			Xstart += DrawChar->AdvanceX * FontWidthFactor;
			LineWidth += DrawChar->AdvanceX * FontWidthFactor;
		} else {
			Xstart += SpaceWidthFactor * FontWidthFactor;
			LineWidth += SpaceWidthFactor * FontWidthFactor;
		}

		// care about restrictions
		if ((ExpandWidth != 0.0f) &&
		    (LineWidth >= ExpandWidth))
			break;
	}

	// text is over, draw all we have in buffer
	DrawBufferOnTextEnd(CurrentTexture);

	// reset rendering states
	vw_SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	vw_SetTextureBlend(false, eTextureBlendFactor::ONE, eTextureBlendFactor::ZERO);
	vw_BindTexture(0, 0);
	return 0;
}

/*
 * vw_FontSizeUTF32 wrapper with variadic arguments and conversion into utf32.
 */
int vw_FontSize(const char *Text, ...)
{
	if (!Text)
		return ERR_PARAMETERS;

	// get string with variadic arguments
	// allocate buffer
	va_list ap;
	va_start(ap, Text);
	std::vector<char> buffer(1 + std::vsnprintf(nullptr, 0, Text, ap));
	va_end(ap);
	// get data into buffer
	va_start(ap, Text);
	std::vsnprintf(buffer.data(), buffer.size(), Text, ap);
	va_end(ap);
	// convert from utf8 into utf32
	const std::u32string UTF32String{ConvertUTF8.from_bytes(buffer.data())};

	return vw_FontSizeUTF32(UTF32String);
}

/*
 * Get string size with current font size.
 */
int vw_FontSizeUTF32(const std::u32string &Text)
{
	if (Text.empty())
		return ERR_PARAMETERS;

	// calculate default space width
	float SpaceWidth{0};
	CalculateDefaultSpaceWidth(SpaceWidth, 1.0f /* don't scale */);

	float LineWidth{0.0f};
	for (const auto &UTF32 : Text) {
		// find current character
		sFontChar *DrawChar = FindFontCharByUTF32(UTF32);
		if (!DrawChar)
			DrawChar = LoadFontChar(UTF32);

		// calculate space characters count in text
		if (UTF32 == SpaceUTF32)
			LineWidth += SpaceWidth;
		else
			LineWidth += DrawChar->AdvanceX;
	}

	return (int)LineWidth;
}

/*
 * vw_DrawFont3DUTF32 wrapper with variadic arguments and conversion into utf32.
 */
int vw_DrawFont3D(float X, float Y, float Z, const char *Text, ...)
{
	if (!Text)
		return ERR_PARAMETERS;

	// get string with variadic arguments
	// allocate buffer
	va_list ap;
	va_start(ap, Text);
	std::vector<char> buffer(1 + std::vsnprintf(nullptr, 0, Text, ap));
	va_end(ap);
	// get data into buffer
	va_start(ap, Text);
	std::vsnprintf(buffer.data(), buffer.size(), Text, ap);
	va_end(ap);
	// convert from utf8 into utf32
	const std::u32string UTF32String{ConvertUTF8.from_bytes(buffer.data())};

	return vw_DrawFont3DUTF32(X, Y, Z, UTF32String);
}

/*
 * Draw 3D text with current font.
 */
int vw_DrawFont3DUTF32(float X, float Y, float Z, const std::u32string &Text)
{
	if (Text.empty())
		return ERR_PARAMETERS;

	// start position on X axis for character
	float Xstart{0.0f};
	// calculate default space width
	float SpaceWidth{0};
	CalculateDefaultSpaceWidth(SpaceWidth, 1.0f /* don't scale */);

	GLtexture CurrentTexture{0};
	float ImageHeight{0.0f};
	float ImageWidth{0.0f};
	vw_SetTextureBlend(true, eTextureBlendFactor::SRC_ALPHA, eTextureBlendFactor::ONE_MINUS_SRC_ALPHA);
	vw_SetColor(1.0f, 1.0f, 1.0f, 1.0f);

	vw_PushMatrix();
	vw_Translate(sVECTOR3D(X, Y, Z));

	// this is billboard, not a 3D model, rotate it to the camera view
	sVECTOR3D CurrentCameraRotation;
	vw_GetCameraRotation(&CurrentCameraRotation);
	vw_Rotate(CurrentCameraRotation.y, 0.0f, 1.0f, 0.0f);
	vw_Rotate(CurrentCameraRotation.x, 1.0f, 0.0f, 0.0f);

	// draw all characters in text by blocks grouped by texture id
	for (const auto &UTF32 : Text) {
		// find current character
		sFontChar *DrawChar = FindFontCharByUTF32(UTF32);
		if (!DrawChar)
			DrawChar = LoadFontChar(UTF32);
		// for first character in text - setup texture by first character
		if (!CurrentTexture) {
			CurrentTexture = DrawChar->Texture;
			vw_FindTextureSizeByID(DrawChar->Texture, &ImageWidth, &ImageHeight);
		}

		// looks like texture should be changed
		if (CurrentTexture != DrawChar->Texture) {
			DrawBufferOnTextureChange(CurrentTexture, DrawChar);
			vw_FindTextureSizeByID(DrawChar->Texture, &ImageWidth, &ImageHeight);
		}

		// put into draw buffer all characters data, except spaces
		if (UTF32 != SpaceUTF32) {
			float DrawX{Xstart + DrawChar->Left};
			float DrawY{(InternalFontSize - DrawChar->Top) * 1.0f};

			// texture's UV coordinates
			// convert origin from bottom left to upper left corner
			float U_Left{(DrawChar->TexturePositionLeft * 1.0f) / ImageWidth};
			float V_Top{1.0f - (DrawChar->TexturePositionTop * 1.0f) / ImageHeight};
			float U_Right{(DrawChar->TexturePositionRight * 1.0f) / ImageWidth};
			float V_Bottom{1.0f - (DrawChar->TexturePositionBottom * 1.0f) / ImageHeight};

			// first triangle
			AddToDrawBuffer(DrawX / 10.0f, (DrawY + DrawChar->Height) / 10.0f, U_Left,V_Top);
			AddToDrawBuffer(DrawX / 10.0f, DrawY / 10.0f, U_Left, V_Bottom);
			AddToDrawBuffer((DrawX + DrawChar->Width) / 10.0f, DrawY / 10.0f, U_Right, V_Bottom);
			// second triangle
			AddToDrawBuffer(DrawX / 10.0f, (DrawY + DrawChar->Height) / 10.0f, U_Left, V_Top);
			AddToDrawBuffer((DrawX + DrawChar->Width) / 10.0f, DrawY / 10.0f, U_Right, V_Bottom);
			AddToDrawBuffer((DrawX + DrawChar->Width) / 10.0f, (DrawY + DrawChar->Height) / 10.0f, U_Right, V_Top);

			Xstart += DrawChar->AdvanceX;
		} else
			Xstart += SpaceWidth;
	}

	// text is over, draw all we have in buffer
	DrawBufferOnTextEnd(CurrentTexture);

	// reset rendering states
	vw_PopMatrix();
	vw_SetTextureBlend(false, eTextureBlendFactor::ONE, eTextureBlendFactor::ZERO);
	vw_BindTexture(0, 0);
	return 0;
}
