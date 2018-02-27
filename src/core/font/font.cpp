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

#include "font.h"
#include <stdarg.h> // va_start
#include <forward_list>
#include <vector>

// TODO move to std::string, all utf8<->utf32 stuff must be revised first

// TODO provide static vw_DrawFont(), dynamic realization we have now,
// static should create array with text blocks as key and VBO/VAO/IBO (and other data)
// as value for fast rendering.

namespace {
// FreeType related stuff.
FT_Library		InternalLibrary;
FT_Face			InternalFace;
std::unique_ptr<uint8_t[]>	InternalFontBuffer{};
// Font settings.
int 			InternalFontSize{0};
int			GlobalFontOffsetY{0};

struct eFontChar {
	unsigned	UTF32; // key element 1 (UTF32 code)
	int		FontSize; // key element 2 (character generated size)

	eTexture	*Texture; // font character texture
	int		TexturePositionLeft;
	int		TexturePositionRight;
	int		TexturePositionTop;
	int		TexturePositionBottom;

	// font character metrics
	int		Width;
	int		Height;
	int		Left;
	int		Top;
	float		AdvanceX;

	// constructor
	eFontChar(unsigned _UTF32, int _FontSize, eTexture *_Texture,
		  int _TexturePositionLeft, int _TexturePositionRight,
		  int _TexturePositionTop, int _TexturePositionBottom,
		  int _Width, int _Height, int _Left, int _Top, float _AdvanceX) :
		UTF32{_UTF32},
		FontSize{_FontSize},
		Texture{_Texture},
		TexturePositionLeft{_TexturePositionLeft},
		TexturePositionRight{_TexturePositionRight},
		TexturePositionTop{_TexturePositionTop},
		TexturePositionBottom{_TexturePositionBottom},
		Width{_Width},
		Height{_Height},
		Left{_Left},
		Top{_Top},
		AdvanceX{_AdvanceX}
	{};
	// trick for forward_list<unique_ptr<T>> work with iterator
	bool CheckUTF32andSize(unsigned _UTF32)
	{
		return ((UTF32 == _UTF32) &&
			(FontSize == InternalFontSize));
	}
	void CheckTexture(eTexture *_Texture)
	{
		if (Texture == _Texture)
			Texture = nullptr;
	}
};

// List with connected font characters.
std::forward_list<std::unique_ptr<eFontChar>> FontChars_List;
// Local draw buffer, that dynamically allocate memory at maximum
// required size only one time per game execution.
// Never use reset(), only clear() for this buffer.
std::vector<float> DrawBuffer{};
} // unnamed namespace


/*
 * Font initialization by font name (path to file).
 */
int vw_InitFont(const std::string &FontName)
{
	if (FontName.empty())
		return -1;

	if (FT_Init_FreeType(&InternalLibrary)) {
		fprintf(stderr, "Can't initialize library, font: %s\n", FontName.c_str());
		return -1;
	}

	if (InternalFontBuffer.get() != nullptr)
		InternalFontBuffer.reset();

	std::unique_ptr<eFILE> FontFile = vw_fopen(FontName);
	if (FontFile == nullptr) {
		fprintf(stderr, "Can't open font file: %s\n", FontName.c_str());
		return -1;
	}

	FontFile->fseek(0, SEEK_END);
	int FontBufferSize = FontFile->ftell();
	FontFile->fseek(0, SEEK_SET);

	InternalFontBuffer.reset(new uint8_t[FontBufferSize]);
	FontFile->fread(InternalFontBuffer.get(), FontBufferSize, 1);

	vw_fclose(FontFile);

	if (FT_New_Memory_Face(InternalLibrary, InternalFontBuffer.get(), FontBufferSize, 0, &InternalFace)) {
		fprintf(stderr, "Can't create font face from memory, font: %s\n", FontName.c_str());
		return -1;
	}

	printf("Font initialized: %s\n\n", FontName.c_str());
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
static eFontChar* FindFontCharByUTF32(unsigned UTF32)
{
	// a bit tricky, since we can't work with iterator for
	// forward_list<unique_ptr<T>> in usual way
	for (auto&& Tmp : FontChars_List) {
		if (Tmp->CheckUTF32andSize(UTF32))
			return Tmp.get();
	}

	return nullptr;
}

/*
 * Check font character by UTF32 code.
 */
bool vw_CheckFontCharByUTF32(unsigned UTF32)
{
	return (FindFontCharByUTF32(UTF32) != nullptr);
}

/*
 * Release all font characters and created for this characters textures.
 */
void vw_ReleaseAllFontChars()
{
	// release all font characters
	while (!FontChars_List.empty()) {

		// TODO this should be fixed in "texture" subsystem, this part is ugly

		// release texture, created to this character
		if (FontChars_List.front()->Texture != nullptr) {
			// one texture could be used by many characters
			// make sure, we don't release one texture twice
			eTexture *Texture = FontChars_List.front()->Texture;
			// a bit tricky, since we can't work with iterator for
			// forward_list<unique_ptr<T>> in usual way
			for (auto&& Tmp : FontChars_List) {
				Tmp->CheckTexture(Texture);
			}
			vw_ReleaseTexture(Texture);
		}
		FontChars_List.pop_front();
	}
	// reset list
	FontChars_List.clear();
}

/*
 *  Shutdown font.
 */
void vw_ShutdownFont()
{
	if (InternalFontBuffer.get() != nullptr)
		InternalFontBuffer.reset();
}

/*
 * Load data and generate font character.
 */
static eFontChar* LoadFontChar(unsigned UTF32)
{
	// setup parameters
	if (FT_Set_Char_Size(InternalFace, InternalFontSize << 6, InternalFontSize << 6, 96, 96)) {
		fprintf(stderr, "Can't set char size %i.", InternalFontSize);
		return nullptr;
	}
	// load glyph
	if (FT_Load_Char( InternalFace, UTF32, FT_LOAD_RENDER | FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT)) {
		fprintf(stderr, "Can't load glyph: %u\n", UTF32);
		return nullptr;
	}

	// create new character
	FontChars_List.push_front(std::unique_ptr<eFontChar>(new eFontChar(UTF32, InternalFontSize, nullptr,
							     0, InternalFace->glyph->bitmap.width, 0, InternalFace->glyph->bitmap.rows,
							     InternalFace->glyph->bitmap.width, InternalFace->glyph->bitmap.rows,
							     InternalFace->glyph->bitmap_left, InternalFace->glyph->bitmap_top,
							     InternalFace->glyph->advance.x / 64.0f)));

	if ((FontChars_List.front()->Width > 0) && (FontChars_List.front()->Height > 0)) {
		// buffer for RGBA, data for font characters texture
		std::unique_ptr<uint8_t[]> tmpPixels{new uint8_t[FontChars_List.front()->Width * FontChars_List.front()->Height * 4]};
		// white color for RGBA, alpha channel will be corrected later
		memset(tmpPixels.get(), 255, FontChars_List.front()->Width * FontChars_List.front()->Height * 4);
		// convert greyscale to RGB+Alpha (32bits), now we need correct only alpha channel
		for (int j = 0; j < FontChars_List.front()->Height; j++) {
			int StrideSrc = j * FontChars_List.front()->Width * 4;
			int StrideDst = (FontChars_List.front()->Height - j - 1) * FontChars_List.front()->Width;
			for (int i = 0; i < FontChars_List.front()->Width; i++) {
				// alpha channel
				memcpy(tmpPixels.get() + StrideSrc + i * 4 + 3, InternalFace->glyph->bitmap.buffer + StrideDst + i, 1);
			}
		}

		// fake texture file name based on UTF32 code
		char FakeTExtureFileName[MAX_PATH];
		sprintf(FakeTExtureFileName, "fontsize_%i_character_%u", InternalFontSize, UTF32);

		vw_SetTextureProp(RI_MAGFILTER_LINEAR | RI_MINFILTER_LINEAR | RI_MIPFILTER_NONE,
				  RI_CLAMP_TO_EDGE, true, TX_ALPHA_GREYSC, false);
		FontChars_List.front()->Texture = vw_CreateTextureFromMemory(FakeTExtureFileName, tmpPixels.get(),
									     FontChars_List.front()->Width, FontChars_List.front()->Height,
									     4, 0, 0, 0, false);
	}

	return FontChars_List.front().get();
}

/*
 * Generate font characters by list.
 */
void vw_GenerateFontChars(int FontTextureWidth, int FontTextureHeight, const char *CharsList)
{
	if (CharsList ==nullptr)
		return;

	printf("Font characters generation start.\n");

	// fake texture file name based on CharsList
	const char *TextureName{CharsList};
	// buffer for RGBA, data for font characters texture
	std::unique_ptr<uint8_t[]> DIB{new uint8_t[FontTextureWidth*FontTextureHeight * 4]};
	// make sure, DIB filled by black and alpha set to zero,
	// or we will have white borders on each character
	memset(DIB.get(), 0, FontTextureWidth*FontTextureHeight * 4);

	// initial setup
	if (FT_Set_Char_Size(InternalFace, InternalFontSize << 6, InternalFontSize << 6, 96, 96)) {
		fprintf(stderr, "Can't set char size %i.", InternalFontSize);
		return;
	}

	// create one large bitmap with all font characters from list
	int CurrentDIBX{0};
	int CurrentDIBY{0};
	int EdgingSpace{2};
	int MaxHeightInCurrentLine{0};
	const char *CharsList2{CharsList};
	while (strlen(CharsList) > 0) {
		unsigned CurrentChar;
		// convert into UTF32 code
		CharsList = vw_UTF8toUTF32(CharsList, &CurrentChar);

		// load glyph
		if (FT_Load_Char(InternalFace, CurrentChar, FT_LOAD_RENDER | FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT)) {
			fprintf(stderr, "Can't load Char: %u\n", CurrentChar);
			return;
		}

		FontChars_List.push_front(std::unique_ptr<eFontChar>(new eFontChar(CurrentChar, InternalFontSize, nullptr,
								     0, 0, 0, 0,
								     InternalFace->glyph->bitmap.width, InternalFace->glyph->bitmap.rows,
								     InternalFace->glyph->bitmap_left, InternalFace->glyph->bitmap_top,
								     InternalFace->glyph->advance.x / 64.0f)));

		// move to next line in bitmap if not enough space
		if (CurrentDIBX + FontChars_List.front()->Width > FontTextureWidth) {
			CurrentDIBX = 0;
			CurrentDIBY += MaxHeightInCurrentLine + EdgingSpace;
			MaxHeightInCurrentLine = 0;
		}
		// looks like no more space left at all, fail
		if (CurrentDIBY + FontChars_List.front()->Height > FontTextureHeight) {
			fprintf(stderr, "Can't generate all font chars in one texture. Too many chars or too small texture size!\n");
			break;
		}

		// copy glyph into bitmap
		uint8_t ColorRGB[3] = {255, 255, 255};
		for (int j = 0; j < FontChars_List.front()->Height; j++) {
			for (int i = 0; i < FontChars_List.front()->Width; i++) {
				memcpy(DIB.get() + (FontTextureHeight - CurrentDIBY - j - 1) * FontTextureWidth * 4 + (CurrentDIBX + i) * 4,
				       ColorRGB, 3);
				memcpy(DIB.get() + (FontTextureHeight - CurrentDIBY - j - 1) * FontTextureWidth * 4 + (CurrentDIBX + i) * 4 + 3,
				       InternalFace->glyph->bitmap.buffer + j * FontChars_List.front()->Width + i, 1);
			}
		}

		// setup new character
		FontChars_List.front()->TexturePositionLeft = CurrentDIBX;
		FontChars_List.front()->TexturePositionRight = CurrentDIBX + FontChars_List.front()->Width;
		FontChars_List.front()->TexturePositionTop = CurrentDIBY;
		FontChars_List.front()->TexturePositionBottom = CurrentDIBY + FontChars_List.front()->Height;

		// detect new line position by height
		if (MaxHeightInCurrentLine < FontChars_List.front()->Height)
			MaxHeightInCurrentLine = FontChars_List.front()->Height;
		CurrentDIBX += FontChars_List.front()->Width + EdgingSpace;
	}

	// create texture from bitmap
	vw_SetTextureProp(RI_MAGFILTER_LINEAR | RI_MINFILTER_LINEAR | RI_MIPFILTER_NONE, RI_CLAMP_TO_EDGE, true, TX_ALPHA_GREYSC, false);
	eTexture *FontTexture = vw_CreateTextureFromMemory(TextureName, DIB.get(), FontTextureWidth, FontTextureHeight, 4, 0);
	if (FontTexture == nullptr) {
		fprintf(stderr, "Can't create font texture.\n");
		return;
	}

	// setup texture to all font characters from list
	while (strlen(CharsList2) > 0) {
		unsigned CurrentChar;
		CharsList2 = vw_UTF8toUTF32(CharsList2, &CurrentChar);
		eFontChar *TMPChar = FindFontCharByUTF32(CurrentChar);
		if (TMPChar != nullptr)
			TMPChar->Texture = FontTexture;
	}

	printf("Font characters generation end.\n\n");
}

/*
 * Add data to local draw buffer.
 */
static inline void AddToDrawBuffer(float FirstX, float FirstY, float SecondX, float SecondY)
{
	DrawBuffer.push_back(FirstX);
	DrawBuffer.push_back(FirstY);
	DrawBuffer.push_back(SecondX);
	DrawBuffer.push_back(SecondY);
}

/*
 * Calculate width factors.
 */
static void CalculateWidthFactors(const char *Text, const float StrictWidth, float *SpaceWidthFactor, float *FontWidthFactor)
{
	float LineWidth1{0}; // for StrictWidth > 0
	float LineWidth2{0}; // for StrictWidth < 0
	int SpaceCount{0};

	while (strlen(Text) > 0) {
		unsigned UTF32;
		Text = vw_UTF8toUTF32(Text, &UTF32);
		eFontChar *DrawChar = FindFontCharByUTF32(UTF32);
		if (DrawChar == nullptr)
			DrawChar = LoadFontChar(UTF32);

		// calculate space characters count in text
		if (UTF32 == 0x020) {
			if (StrictWidth > 0)
				SpaceCount++;
			else
				LineWidth2 += *SpaceWidthFactor;
		} else {
			if (StrictWidth > 0)
				LineWidth1 += DrawChar->AdvanceX;
			else
				LineWidth2 += DrawChar->AdvanceX;
		}
	}

	// correction for factors in both cases
	if (StrictWidth > 0) {
		if ((StrictWidth > LineWidth1) &&
		    (SpaceCount != 0))
			*SpaceWidthFactor = (StrictWidth - LineWidth1)/SpaceCount;
	} else {
		if (StrictWidth * (-1.0f) < LineWidth2)
			*FontWidthFactor = StrictWidth/LineWidth2 * (-1.0f);
	}
}

/*
 * Draw text with current font.
 *
 * StrictWidth - strict text by width:
 *      if StrictWidth > 0, reduce space width only
 *      if StrictWidth < 0, reduce all font character's width
 * ExpandWidth - expand width to provided parameter
 */
void vw_DrawFont(int X, int Y, float StrictWidth, float ExpandWidth, float FontScale,
		 float R, float G, float B, float Transp, const char *Text, ...)
{
	if (Text == nullptr)
		return;
	if (Transp >= 1.0f)
		Transp = 1.0f;

	// care about aspect ration
	float AW, AH;
	bool ASpresent = vw_GetAspectWH(&AW, &AH);
	// get current viewport
	int W, H;
	vw_GetViewport(nullptr, nullptr, &W, &H);
	float AHw{H * 1.0f};

	// check text position
	if (ASpresent) {
		if (Y > AH)
			return;
	} else {
		if (Y > H)
			return;
	}
	if (Y + InternalFontSize * FontScale < 0)
		return;

	// get string with variable arguments
	char text[1024];
	va_list ap;
	va_start(ap, Text);
	vsprintf(text, Text, ap);
	va_end(ap);
	if (strlen(text) == 0)
		return;

	// start position on X axis for character
	float Xstart{X * 1.0f};
	// calculate default space width, 0x020 - utf8 code for space character
	if (FindFontCharByUTF32(0x020) == nullptr)
		LoadFontChar(0x020);
	float SpaceWidthFactor = FindFontCharByUTF32(0x020)->AdvanceX*FontScale;
	// width factor for for space charecter, make sure, we have space width at least 65% of current font size
	if (SpaceWidthFactor < (InternalFontSize * 0.65f))
		SpaceWidthFactor = InternalFontSize * 0.65f;
	// width factor for characters
	float FontWidthFactor{1.0f};

	// calculate width factors
	if (StrictWidth != 0)
		CalculateWidthFactors(text, StrictWidth, &SpaceWidthFactor, &FontWidthFactor);

	// calculate text width, all characters that we already rendered
	float LineWidth{0};

	vw_SetTextureBlend(true, RI_BLEND_SRCALPHA, RI_BLEND_INVSRCALPHA);
	vw_SetColor(R, G, B, Transp);
	eTexture *CurrentTexture{nullptr};

	// combine calculated width factor and global width scale
	FontWidthFactor = FontScale*FontWidthFactor;

	// store pointer on first character in text
	const char *textdraw{text};
	// draw all characters in text by blocks grouped by texture id
	while (strlen(textdraw) > 0) {
		unsigned UTF32;
		textdraw = vw_UTF8toUTF32(textdraw, &UTF32);
		// find current character
		eFontChar *DrawChar = FindFontCharByUTF32(UTF32);
		if (DrawChar == nullptr)
			DrawChar = LoadFontChar(UTF32);
		// for first character in text - setup texture by first character
		if (CurrentTexture == nullptr)
			CurrentTexture = DrawChar->Texture;

		// looks like texture should be changed
		if (CurrentTexture != DrawChar->Texture) {
			// draw all we have in buffer with current texture
			if (!DrawBuffer.empty()) {
				vw_SetTexture(0, CurrentTexture);
				vw_SendVertices(RI_TRIANGLES, DrawBuffer.size() / sizeof(DrawBuffer.data()[0]),
						RI_2f_XY | RI_1_TEX, DrawBuffer.data(), 4 * sizeof(DrawBuffer.data()[0]));
				DrawBuffer.clear();
			}
			// setup new texture
			CurrentTexture = DrawChar->Texture;
		}

		// put into draw buffer all characters data, except spaces
		if (UTF32 != 0x020) {
			float DrawX{Xstart + DrawChar->Left * FontWidthFactor};
			float DrawY{Y + GlobalFontOffsetY + (InternalFontSize - DrawChar->Top) * FontScale};

			// setup Y axis correction, if need
			float tmpPosY{0};
			// should be altered only in the case of RI_UL_CORNER
			if (ASpresent)
				tmpPosY = (AH - DrawY - DrawY - DrawChar->Height * FontScale);
			else
				tmpPosY = (AHw - DrawY - DrawY - DrawChar->Height * FontScale);

			float ImageHeight{DrawChar->Texture->Height * 1.0f};
			float ImageWidth{DrawChar->Texture->Width * 1.0f};

			float FrameHeight{(DrawChar->TexturePositionBottom * 1.0f) / ImageHeight};
			float FrameWidth{(DrawChar->TexturePositionRight * 1.0f) / ImageWidth};

			float Yst{(DrawChar->TexturePositionTop * 1.0f) / ImageHeight};
			float Xst{(DrawChar->TexturePositionLeft * 1.0f) / ImageWidth};

			// first triangle
			AddToDrawBuffer(DrawX, DrawY + tmpPosY + DrawChar->Height * FontScale, Xst, 1.0f - Yst);
			AddToDrawBuffer(DrawX, DrawY + tmpPosY, Xst, 1.0f - FrameHeight);
			AddToDrawBuffer(DrawX + DrawChar->Width * FontWidthFactor, DrawY + tmpPosY, FrameWidth, 1.0f - FrameHeight);
			// second triangle
			AddToDrawBuffer(DrawX, DrawY + tmpPosY + DrawChar->Height * FontScale, Xst, 1.0f - Yst);
			AddToDrawBuffer(DrawX + DrawChar->Width * FontWidthFactor, DrawY + tmpPosY, FrameWidth, 1.0f - FrameHeight);
			AddToDrawBuffer(DrawX + DrawChar->Width * FontWidthFactor, DrawY + tmpPosY + DrawChar->Height * FontScale, FrameWidth, 1.0f - Yst);

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
	if (!DrawBuffer.empty()) {
		vw_SetTexture(0, CurrentTexture);
		vw_SendVertices(RI_TRIANGLES, DrawBuffer.size() / sizeof(DrawBuffer.data()[0]),
				RI_2f_XY | RI_1_TEX, DrawBuffer.data(), 4 * sizeof(DrawBuffer.data()[0]));
		DrawBuffer.clear();
	}

	// reset rendering states
	vw_SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	vw_SetTextureBlend(false, 0, 0);
	vw_BindTexture(0, 0);
}

/*
 * Get string size with current font size.
 */
int vw_FontSize(const char *Text, ...)
{
	if (Text == nullptr)
		return 0;

	// get string with variable arguments
	char text[1024];
	va_list ap;
	va_start(ap, Text);
	vsprintf(text, Text, ap);
	va_end(ap);
	if (strlen(text) == 0)
		return 0;

	const char *textdraw{text};
	// calculate default space width
	if (FindFontCharByUTF32(0x020) == nullptr)
		LoadFontChar(0x020);
	float SpaceWidth = FindFontCharByUTF32(0x020)->AdvanceX;
	// make sure, we have space width at least 65% of current font size
	if (SpaceWidth < (InternalFontSize * 0.65f))
		SpaceWidth = InternalFontSize * 0.65f;

	float LineWidth{0.0f};
	while (strlen(textdraw) > 0) {
		unsigned UTF32;
		textdraw = vw_UTF8toUTF32(textdraw, &UTF32);
		// find current character
		eFontChar *DrawChar = FindFontCharByUTF32(UTF32);
		if (DrawChar == nullptr)
			DrawChar = LoadFontChar(UTF32);

		// calculate space characters count in text
		if (UTF32 == 0x020)
			LineWidth += SpaceWidth;
		else
			LineWidth += DrawChar->AdvanceX;
	}

	return (int)LineWidth;
}

/*
 * Draw 3D text with current font.
 */
void vw_DrawFont3D(float X, float Y, float Z, const char *Text, ...)
{
	if (Text == nullptr)
		return;

	// get string with variable arguments
	char text[1024];
	va_list ap;
	va_start(ap, Text);
	vsprintf(text, Text, ap);
	va_end(ap);
	if (strlen(text) == 0)
		return;

	// start position on X axis for character
	float Xstart{0.0f};
	// calculate default space width, 0x020 - utf8 code for space character
	float SpaceWidth = FindFontCharByUTF32(0x020)->AdvanceX;
	// make sure, we have space width at least 65% of current font size
	if (SpaceWidth < (InternalFontSize * 0.65f))
		SpaceWidth = InternalFontSize * 0.65f;

	eTexture *CurrentTexture{nullptr};
	vw_SetTextureBlend(true, RI_BLEND_SRCALPHA, RI_BLEND_INVSRCALPHA);
	vw_SetColor(1.0f, 1.0f, 1.0f, 1.0f);

	vw_PushMatrix();
	vw_Translate(VECTOR3D(X, Y, Z));

	// this is billboard, not a 3D model, rotate it to the camera view
	VECTOR3D CurrentCameraRotation;
	vw_GetCameraRotation(&CurrentCameraRotation);
	vw_Rotate(CurrentCameraRotation.y, 0.0f, 1.0f, 0.0f);
	vw_Rotate(CurrentCameraRotation.x, 1.0f, 0.0f, 0.0f);

	// store pointer on first character in text
	const char *textdraw{text};
	// draw all characters in text by blocks grouped by texture id
	while (strlen(textdraw) > 0) {
		unsigned UTF32;
		textdraw = vw_UTF8toUTF32(textdraw, &UTF32);
		// find current character
		eFontChar *DrawChar = FindFontCharByUTF32(UTF32);
		if (DrawChar == nullptr)
			DrawChar = LoadFontChar(UTF32);
		// for first character in text - setup texture by first character
		if (CurrentTexture == nullptr)
			CurrentTexture = DrawChar->Texture;

		// looks like texture should be changed
		if (CurrentTexture != DrawChar->Texture) {
			// draw all we have in buffer with current texture
			if (!DrawBuffer.empty()) {
				vw_SetTexture(0, CurrentTexture);
				vw_SendVertices(RI_TRIANGLES, DrawBuffer.size() / sizeof(DrawBuffer.data()[0]),
						RI_2f_XY | RI_1_TEX, DrawBuffer.data(), 4 * sizeof(DrawBuffer.data()[0]));
				DrawBuffer.clear();
			}
			// setup new texture
			CurrentTexture = DrawChar->Texture;
		}

		// put into draw buffer all characters data, except spaces
		if (UTF32 != 0x020) {
			float DrawX{Xstart + DrawChar->Left};
			float DrawY{(InternalFontSize - DrawChar->Top) * 1.0f};

			float ImageHeight{DrawChar->Texture->Height * 1.0f};
			float ImageWidth{DrawChar->Texture->Width * 1.0f};

			float FrameHeight{(DrawChar->TexturePositionBottom * 1.0f) / ImageHeight};
			float FrameWidth{(DrawChar->TexturePositionRight * 1.0f) / ImageWidth};

			float Yst{(DrawChar->TexturePositionTop * 1.0f) / ImageHeight};
			float Xst{(DrawChar->TexturePositionLeft * 1.0f) / ImageWidth};

			// first triangle
			AddToDrawBuffer(DrawX / 10.0f, (DrawY + DrawChar->Height) / 10.0f, Xst, 1.0f - Yst);
			AddToDrawBuffer(DrawX / 10.0f, DrawY / 10.0f, Xst, 1.0f - FrameHeight);
			AddToDrawBuffer((DrawX + DrawChar->Width) / 10.0f, DrawY / 10.0f, FrameWidth, 1.0f - FrameHeight);
			// second triangle
			AddToDrawBuffer(DrawX / 10.0f, (DrawY + DrawChar->Height) / 10.0f, Xst, 1.0f - Yst);
			AddToDrawBuffer((DrawX + DrawChar->Width) / 10.0f, DrawY / 10.0f, FrameWidth, 1.0f - FrameHeight);
			AddToDrawBuffer((DrawX + DrawChar->Width) / 10.0f, (DrawY + DrawChar->Height) / 10.0f, FrameWidth, 1.0f - Yst);

			Xstart += DrawChar->AdvanceX;
		} else
			Xstart += SpaceWidth;
	}

	// text is over, draw all we have in buffer
	if (!DrawBuffer.empty()) {
		vw_SetTexture(0, CurrentTexture);
		vw_SendVertices(RI_TRIANGLES, DrawBuffer.size() / sizeof(DrawBuffer.data()[0]),
				RI_2f_XY | RI_1_TEX, DrawBuffer.data(), 4 * sizeof(DrawBuffer.data()[0]));
		DrawBuffer.clear();
	}

	// reset rendering states
	vw_PopMatrix();
	vw_SetTextureBlend(false, 0, 0);
	vw_BindTexture(0, 0);
}
