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

// TODO move to std::string, probably, we need move directly to std::u32string (utf32)
//      for all text rendering and text input (profile names)

// TODO remove va_start, use variadic templates instead (?)

// TODO provide static vw_DrawText(), dynamic realization we have now,
//      static should create array with text blocks as key and VBO/VAO/IBO (and other data)
//      as value for fast rendering.

// TODO switch to std::unordered_multimap from std::forward_list
//      that will allow fast access and we could manually check second key (FontSize)

// TODO (?) add VBO (DYNAMIC) and VAO

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
// we are safe with sIF_dual_type here, since font size and position will not exceed 'float'
sIF_dual_type<unsigned, float> InternalFontSize{0};
constexpr float GlobalFontOffsetY{2.0f}; // FIXME 'fix' for legacy related code, since previously we are used texture instead of
					 //       freetype, so, all vw_DrawText() calls have wrong Y position now in game code

struct sTexturePos {
	float left, top, right, bottom;

	sTexturePos (const unsigned _left, const unsigned _top,
		     const unsigned _right, const unsigned _bottom) :
		/* we are safe with static_cast here, since size will not exceed 'float' */
		left{static_cast<float>(_left)},
		top{static_cast<float>(_top)},
		right{static_cast<float>(_right)},
		bottom{static_cast<float>(_bottom)}
	{}
};

struct sFontMetrics {
	// we are safe with sIF_dual_type here, since position and font size will not exceed 'float'
	sIF_dual_type<int, float> X, Y;
	sIF_dual_type<unsigned, float> Width, Height;
	float AdvanceX;

	sFontMetrics (const int _X, const int _Y,
		      const unsigned _Width, const unsigned _Height,
		      const long _AdvanceX /* in 1/64th of points */) :
		X{_X},
		Y{_Y},
		Width{_Width},
		Height{_Height},
		/* we are safe with static_cast here, since 'advance.x' will not exceed 'float' */
		AdvanceX{static_cast<float>(_AdvanceX) / 64.0f}
	{}
};

struct sFontChar {
	char32_t UTF32; // key element 1 (UTF32 code)
	sIF_dual_type<unsigned, float> FontSize; // key element 2 (character generated size)

	GLtexture Texture{0};
	sTexturePos TexturePos;
	sFontMetrics FontMetrics;

	// constructor
	sFontChar(char32_t _UTF32, const sIF_dual_type<unsigned, float> &_FontSize,
		  const sTexturePos &_TexturePos,
		  const sFontMetrics &_FontMetrics) :
		UTF32{_UTF32},
		FontSize{_FontSize},
		TexturePos{_TexturePos},
		FontMetrics{_FontMetrics}
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
// Local vertex array, that dynamically allocate memory at maximum required
// size only one time per game execution. Don't use std::vector here,
// since it have poor performance compared to std::unique_ptr.
std::unique_ptr<float[]> VertexArray{};
unsigned int VertexArrayPosition{0};
unsigned int VertexArraySize{0};
std::unique_ptr<unsigned[]> IndexArray{};
GLuint IndexBO{0};
unsigned int IndexArraySize{0};
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
	long FontBufferSize = FontFile->ftell();
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

	// FIXME probably, this part should be moved to separate method and call only on OpenGL context destroy
	if (IndexBO)
		vw_DeleteBufferObject(IndexBO);
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
	if (FT_Set_Char_Size(InternalFace, InternalFontSize.i() << 6, InternalFontSize.i() << 6, 96, 96)) {
		std::cerr << __func__ << "(): " << "Can't set char size " << InternalFontSize.i() << "\n";
		return nullptr;
	}
	// load glyph
	if (FT_Load_Char(InternalFace, UTF32, FT_LOAD_RENDER | FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT)) {
		std::cerr << __func__ << "(): " << "Can't load glyph: " << UTF32 << "\n";
		return nullptr;
	}

	// create new character
	FontCharsList.push_front(std::unique_ptr<sFontChar>(new sFontChar(UTF32, InternalFontSize,
				 sTexturePos{0, 0, InternalFace->glyph->bitmap.width, InternalFace->glyph->bitmap.rows},
				 sFontMetrics{InternalFace->glyph->bitmap_left, InternalFace->glyph->bitmap_top,
				 InternalFace->glyph->bitmap.width, InternalFace->glyph->bitmap.rows,
				 InternalFace->glyph->advance.x /* in 1/64th of points */})));

	if ((FontCharsList.front()->FontMetrics.Width.i() > 0) && (FontCharsList.front()->FontMetrics.Height.i() > 0)) {
		// buffer for RGBA, data for font characters texture, initialize it with white color (255)
		std::unique_ptr<uint8_t[]> tmpPixels(new uint8_t[FontCharsList.front()->FontMetrics.Width.i() *
								 FontCharsList.front()->FontMetrics.Height.i() * 4]);
		memset(tmpPixels.get(), 255 /*white*/, FontCharsList.front()->FontMetrics.Width.i() *
						       FontCharsList.front()->FontMetrics.Height.i() * 4);
		// convert greyscale to RGB+Alpha (32bits), now we need correct only alpha channel
		for (unsigned j = 0; j < FontCharsList.front()->FontMetrics.Height.i(); j++) {
			int StrideSrc = j * FontCharsList.front()->FontMetrics.Width.i() * 4;
			int StrideDst = (FontCharsList.front()->FontMetrics.Height.i() - j - 1) *
					FontCharsList.front()->FontMetrics.Width.i();
			for (unsigned i = 0; i < FontCharsList.front()->FontMetrics.Width.i(); i++) {
				// alpha channel
				memcpy(tmpPixels.get() + StrideSrc + i * 4 + 3,
				       InternalFace->glyph->bitmap.buffer + StrideDst + i,
				       1);
			}
		}

		// fake texture file name based on font's size and UTF32 code
		std::string FakeTextureFileName{"fontsize_" + std::to_string(InternalFontSize.i()) +
						"_character_" + std::to_string(UTF32)};

		vw_SetTextureProp(eTextureBasicFilter::BILINEAR, 1,
				  eTextureWrapMode::CLAMP_TO_EDGE, true, eAlphaCreateMode::GREYSC, false);
		FontCharsList.front()->Texture = vw_CreateTextureFromMemory(FakeTextureFileName, tmpPixels,
									    FontCharsList.front()->FontMetrics.Width.i(),
									    FontCharsList.front()->FontMetrics.Height.i(),
									    4, eTextureCompressionType::NONE,
									    0, 0);
	}

	std::cout << "Font character was created for size: "
		  << InternalFontSize.i() << ",  char: '"
		  << ConvertUTF8.to_bytes(UTF32) << "',  code: "
		  << "0x" << std::uppercase << std::hex << UTF32 << std::dec << "\n";

	return FontCharsList.front().get();
}

/*
 * Generate font characters by list.
 */
int vw_GenerateFontChars(unsigned FontTextureWidth, unsigned FontTextureHeight,
			 const std::unordered_set<char32_t> &CharsSetUTF32)
{
	if (CharsSetUTF32.empty())
		return ERR_PARAMETERS;

	std::cout << "Font characters generation start.\n";

	// buffer for RGBA, data for font characters texture
	// make sure, tmpPixels filled by black and alpha set to zero (0),
	// or we will have white borders on each character
	std::unique_ptr<uint8_t[]> tmpPixels(new uint8_t[FontTextureWidth * FontTextureHeight * 4]);
	memset(tmpPixels.get(), 0 /*black + transparent*/, FontTextureWidth * FontTextureHeight * 4);

	// initial setup
	if (FT_Set_Char_Size(InternalFace /* handle to face object */,
			     InternalFontSize.i() << 6 /* char_width in 1/64th of points */,
			     InternalFontSize.i() << 6 /* char_height in 1/64th of points */,
			     96 /* horizontal device resolution */,
			     96 /* vertical device resolution */)) {
		std::cerr << __func__ << "(): " << "Can't set char size " << InternalFontSize.i() << "\n";
		return ERR_EXT_RES;
	}

	// create one large bitmap with all font characters from list
	int CurrentPixelsX{0};
	int CurrentPixelsY{0};
	int EdgingSpace{2};
	unsigned MaxHeightInCurrentLine{0};
	for (const auto &CurrentChar : CharsSetUTF32) {
		// load glyph
		if (FT_Load_Char(InternalFace, CurrentChar, FT_LOAD_RENDER | FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT)) {
			std::cerr << __func__ << "(): " << "Can't load Char: " << CurrentChar << "\n";
			return ERR_EXT_RES;
		}

		FontCharsList.push_front(std::unique_ptr<sFontChar>(new sFontChar(CurrentChar, InternalFontSize,
					 sTexturePos{0, 0, 0, 0},
					 sFontMetrics{InternalFace->glyph->bitmap_left, InternalFace->glyph->bitmap_top,
					 InternalFace->glyph->bitmap.width, InternalFace->glyph->bitmap.rows,
					 InternalFace->glyph->advance.x /* in 1/64th of points */})));

		// move to next line in bitmap if not enough space
		if (CurrentPixelsX + FontCharsList.front()->FontMetrics.Width.i() > FontTextureWidth) {
			CurrentPixelsX = 0;
			CurrentPixelsY += MaxHeightInCurrentLine + EdgingSpace;
			MaxHeightInCurrentLine = 0;
		}
		// looks like no more space left at all, fail
		if (CurrentPixelsY + FontCharsList.front()->FontMetrics.Height.i() > FontTextureHeight) {
			std::cerr << __func__ << "(): " << "Can't generate all font chars in one texture.\n"
				  << "Too many chars or too small texture size!\n";
			break;
		}

		// copy glyph into bitmap
		uint8_t ColorRGB[3]{255, 255, 255};
		for (unsigned j = 0; j < FontCharsList.front()->FontMetrics.Height.i(); j++) {
			unsigned int tmpOffset = (FontTextureHeight - CurrentPixelsY - j - 1) * FontTextureWidth * 4;
			for (unsigned i = 0; i < FontCharsList.front()->FontMetrics.Width.i(); i++) {
				memcpy(tmpPixels.get() + tmpOffset + (CurrentPixelsX + i) * 4,
				       ColorRGB,
				       3);
				memcpy(tmpPixels.get() + tmpOffset + (CurrentPixelsX + i) * 4 + 3,
				       InternalFace->glyph->bitmap.buffer + j * FontCharsList.front()->FontMetrics.Width.i() + i,
				       1);
			}
		}

		// setup new character
		// we are safe with static_cast here, since size will not exceed 'float'
		FontCharsList.front()->TexturePos.left = static_cast<float>(CurrentPixelsX);
		FontCharsList.front()->TexturePos.right = static_cast<float>(CurrentPixelsX) +
							  FontCharsList.front()->FontMetrics.Width.f();
		FontCharsList.front()->TexturePos.top = static_cast<float>(CurrentPixelsY);
		FontCharsList.front()->TexturePos.bottom = static_cast<float>(CurrentPixelsY) +
							   FontCharsList.front()->FontMetrics.Height.f();

		// detect new line position by height
		if (MaxHeightInCurrentLine < FontCharsList.front()->FontMetrics.Height.i())
			MaxHeightInCurrentLine = FontCharsList.front()->FontMetrics.Height.i();
		CurrentPixelsX += FontCharsList.front()->FontMetrics.Width.i() + EdgingSpace;
	}

	// create texture from bitmap
	vw_SetTextureProp(eTextureBasicFilter::BILINEAR, 1,
			  eTextureWrapMode::CLAMP_TO_EDGE, true, eAlphaCreateMode::GREYSC, false);
	std::string tmpTextureName{"auto_generated_texture_for_fonts_" +
				   std::to_string(SDL_GetTicks()) + "_" +
				   std::to_string(InternalFontSize.i())};
	GLtexture FontTexture = vw_CreateTextureFromMemory(tmpTextureName, tmpPixels, FontTextureWidth, FontTextureHeight, 4);
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
	VertexArray[VertexArrayPosition++] = CoordX;
	VertexArray[VertexArrayPosition++] = CoordY;
	VertexArray[VertexArrayPosition++] = TextureU;
	VertexArray[VertexArrayPosition++] = TextureV;
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
				LineWidth1 += DrawChar->FontMetrics.AdvanceX;
			else
				LineWidth2 += DrawChar->FontMetrics.AdvanceX;
		}
	}

	// correction for factors in both cases
	if (StrictWidth > 0) {
		if ((StrictWidth > LineWidth1) && SpaceCount)
			// we are safe with static_cast here, since SpaceCount
			// will not exceed 'float' in our case for sure (usually, <100)
			SpaceWidthFactor = (StrictWidth - LineWidth1) / static_cast<float>(SpaceCount);
	} else {
		if ( -StrictWidth < LineWidth2)
			FontWidthFactor = -StrictWidth / LineWidth2;
	}
}

/*
 * Calculate default space width.
 */
static void CalculateDefaultSpaceWidth(float &SpaceWidthFactor, float FontScale)
{
	if (!FindFontCharByUTF32(SpaceUTF32))
		LoadFontChar(SpaceUTF32);
	SpaceWidthFactor = FindFontCharByUTF32(SpaceUTF32)->FontMetrics.AdvanceX * FontScale;
	// width factor for for space charecter, make sure, we have space width at least 65% of current font size
	if (SpaceWidthFactor < (InternalFontSize.f() * 0.65f))
		SpaceWidthFactor = InternalFontSize.f() * 0.65f;
}

/*
 * Draw buffer on texture change.
 * Caller should care about pointers. nullptr not allowed.
 */
static void DrawBufferOnTextureChange(GLtexture &CurrentTexture, const sFontChar *DrawChar)
{
	// draw all we have with current texture
	if (VertexArrayPosition) {
		vw_BindTexture(0, CurrentTexture);
		vw_Draw3D(ePrimitiveType::TRIANGLES, VertexArrayPosition * 6 / 16, // index / vertex size factor
			  RI_2f_XY | RI_1_TEX, VertexArray.get(), 4 * sizeof(VertexArray.get()[0]),
			  0, 0, IndexArray.get(), IndexBO);
		VertexArrayPosition = 0;
	}
	// setup new texture
	CurrentTexture = DrawChar->Texture;
}

/*
 * Draw buffer on text end.
 */
static void DrawBufferOnTextEnd(GLtexture CurrentTexture)
{
	if (!VertexArrayPosition)
		return;

	vw_BindTexture(0, CurrentTexture);
	vw_Draw3D(ePrimitiveType::TRIANGLES, VertexArrayPosition * 6 / 16, // index / vertex size factor
		  RI_2f_XY | RI_1_TEX, VertexArray.get(), 4 * sizeof(VertexArray.get()[0]),
		  0, 0, IndexArray.get(), IndexBO);
	VertexArrayPosition = 0;
}

/*
 * vw_DrawTextUTF32 wrapper with variadic arguments and conversion into utf32.
 */
int vw_DrawText(int X, int Y, float StrictWidth, float ExpandWidth, float FontScale,
		const sRGBCOLOR &Color, float Transp, const char *Text, ...)
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

	return vw_DrawTextUTF32(X, Y, StrictWidth, ExpandWidth, FontScale, Color, Transp, UTF32String);
}

/*
 * Draw buffers routine (allocate memory, reset counters...).
 */
static void DrawBuffersRoutine(unsigned TextSize)
{
	// triangles points (4) * (RI_2f_XYZ + RI_2f_TEX) * Text.size()
	unsigned int tmpVertexArraySize = 4 * (2 + 2) * TextSize;
	if (tmpVertexArraySize > VertexArraySize) {
		VertexArraySize = tmpVertexArraySize;
		VertexArray.reset(new float[VertexArraySize]);
	}
	VertexArrayPosition = 0;

	unsigned int tmpIndexArraySize = TextSize * 6; // 2 triangles with 3 vertices each
	if (tmpIndexArraySize > IndexArraySize) {
		IndexArraySize = tmpIndexArraySize;
		IndexArray.reset(new unsigned[IndexArraySize]);
		// since we know exactly, what vertex buffer will contain,
		// generate index array with proper sequence
		for (unsigned int i = 0, j = 0; i < IndexArraySize; ) {
			IndexArray[i++] = j + 0;
			IndexArray[i++] = j + 1;
			IndexArray[i++] = j + 2;
			IndexArray[i++] = j + 0;
			IndexArray[i++] = j + 2;
			IndexArray[i++] = j + 3;
			j += 4; // "add" to "vertex array" position 4 vertices
		}

		if (IndexBO)
			vw_DeleteBufferObject(IndexBO);
	}

	// IBO should be re-generated on game restart and on index array size change
	if (!IndexBO && IndexArraySize && IndexArray.get() && vw_GetDevCaps().OpenGL_1_5_supported)
		vw_BuildBufferObject(eBufferObject::Index, IndexArraySize * sizeof(unsigned), IndexArray.get(), IndexBO);
}

/*
 * Draw text with current font. Origin is upper left corner.
 *
 * StrictWidth - strict text by width:
 *      if StrictWidth > 0, reduce space width only
 *      if StrictWidth < 0, reduce all font character's width
 * ExpandWidth - expand width to provided parameter
 */
int vw_DrawTextUTF32(int X, int Y, float StrictWidth, float ExpandWidth, float FontScale,
		     const sRGBCOLOR &Color, float Transp, const std::u32string &Text)
{
	if (Text.empty())
		return ERR_PARAMETERS;
	if (Transp >= 1.0f)
		Transp = 1.0f;

	// 1) we are safe with static_cast here, since InternalFontSize * FontScale
	//    will not exceed 'int' in our case for sure (usually, <100)
	// 2) preference for checking integers, so, we convert float to int
	//    for best speed and accuracy
	if (Y + static_cast<int>(InternalFontSize.f() * FontScale) < 0)
		return 0; // it's ok, we work in proper way here

	// start position on X axis for character
	// we are safe with static_cast here, since X will be less that screen resolution (usually, <10000)
	float Xstart{static_cast<float>(X)};
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
	vw_SetColor(Color.r, Color.g, Color.b, Transp);
	GLtexture CurrentTexture{0};
	float ImageHeight{0.0f};
	float ImageWidth{0.0f};

	// combine calculated width factor and global width scale
	FontWidthFactor = FontScale*FontWidthFactor;

	// we are safe with static_cast here, since text size will not exceed 'unsigned' in our case for sure
	DrawBuffersRoutine(static_cast<unsigned>(Text.size()));

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
			float DrawX{Xstart + DrawChar->FontMetrics.X.f() * FontWidthFactor};
			// we are safe with static_cast here, since X will be less that screen resolution (usually, <10000)
			float DrawY{static_cast<float>(Y) + GlobalFontOffsetY +
				    (InternalFontSize.f() - DrawChar->FontMetrics.Y.f()) * FontScale};

			// texture's UV coordinates
			float U_Left{DrawChar->TexturePos.left / ImageWidth};
			float V_Top{DrawChar->TexturePos.top / ImageHeight};
			float U_Right{DrawChar->TexturePos.right / ImageWidth};
			float V_Bottom{DrawChar->TexturePos.bottom / ImageHeight};

			// triangle's points (index buffer will provide proper sequence)
			AddToDrawBuffer(DrawX, DrawY, U_Left, V_Top);
			AddToDrawBuffer(DrawX, DrawY + DrawChar->FontMetrics.Height.f() * FontScale, U_Left, V_Bottom);
			AddToDrawBuffer(DrawX + DrawChar->FontMetrics.Width.f() * FontWidthFactor,
					DrawY + DrawChar->FontMetrics.Height.f() * FontScale,
					U_Right, V_Bottom);
			AddToDrawBuffer(DrawX + DrawChar->FontMetrics.Width.f() * FontWidthFactor, DrawY, U_Right, V_Top);

			Xstart += DrawChar->FontMetrics.AdvanceX * FontWidthFactor;
			LineWidth += DrawChar->FontMetrics.AdvanceX * FontWidthFactor;
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
 * vw_TextWidthUTF32 wrapper with variadic arguments and conversion into utf32.
 */
int vw_TextWidth(const char *Text, ...)
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

	return vw_TextWidthUTF32(UTF32String);
}

/*
 * Get string size with current font size.
 */
int vw_TextWidthUTF32(const std::u32string &Text)
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
			LineWidth += DrawChar->FontMetrics.AdvanceX;
	}

	return static_cast<int>(LineWidth);
}

/*
 * Draw 3D text with current font.
 */
int vw_DrawText3DUTF32(float X, float Y, float Z, const std::u32string &Text)
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

	/* we are safe with static_cast here, since text size will not exceed 'unsigned' */
	DrawBuffersRoutine(static_cast<unsigned>(Text.size()));

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
			float DrawX{Xstart + DrawChar->FontMetrics.X.f()};
			float DrawY{InternalFontSize.f() - DrawChar->FontMetrics.Y.f()};

			// texture's UV coordinates
			// convert origin from bottom left to upper left corner
			float U_Left{DrawChar->TexturePos.left / ImageWidth};
			float V_Top{1.0f - DrawChar->TexturePos.top / ImageHeight};
			float U_Right{DrawChar->TexturePos.right / ImageWidth};
			float V_Bottom{1.0f - DrawChar->TexturePos.bottom / ImageHeight};

			// triangle's points (index buffer will provide proper sequence)
			AddToDrawBuffer(DrawX / 10.0f, (DrawY + DrawChar->FontMetrics.Height.f()) / 10.0f, U_Left,V_Top);
			AddToDrawBuffer(DrawX / 10.0f, DrawY / 10.0f, U_Left, V_Bottom);
			AddToDrawBuffer((DrawX + DrawChar->FontMetrics.Width.f()) / 10.0f, DrawY / 10.0f, U_Right, V_Bottom);
			AddToDrawBuffer((DrawX + DrawChar->FontMetrics.Width.f()) / 10.0f,
					(DrawY + DrawChar->FontMetrics.Height.f()) / 10.0f, U_Right, V_Top);

			Xstart += DrawChar->FontMetrics.AdvanceX;
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
