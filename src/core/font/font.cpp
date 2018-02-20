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
#include <forward_list>

/* TODO translate comments */
/* TODO move to std::string, all utf8<->utf32 stuff must be revised first */
/* TODO move vertex buffers to std::vector (?) looks pretty ugly now */
/* TODO Font characters rendering should be revised in order to use use RI_TRIANGLES instead of RI_QUADS.
 * The main reason is provide more friendly code portability on different platforms.
 * This also increase buffers size on 50%, but buffers are small enough.
 */

/* FreeType related stuff. */
FT_Library	InternalLibrary;
FT_Face		InternalFace;
BYTE		*InternalFontBuffer = nullptr;
/* Font settings. */
int 		InternalFontSize;
int		GlobalFontOffsetY = 0;

struct eFontChar {
	unsigned 	UTF32; /* UTF32 code */
	int		FontSize; /* character generated size */

	eTexture	*Texture; /* font character texture */
	int		TexturePositionLeft;
	int		TexturePositionRight;
	int		TexturePositionTop;
	int		TexturePositionBottom;

	/* font character metrics */
	int		Width;
	int		Height;
	int		Left;
	int		Top;
	float		AdvanceX;

	/* constructor */
	eFontChar(unsigned _UTF32, int _FontSize, eTexture *_Texture,
		  int _TexturePositionLeft, int _TexturePositionRight,
		  int _TexturePositionTop, int _TexturePositionBottom,
		  int _Width, int _Height, int _Left, int _Top, float _AdvanceX) :
		UTF32(_UTF32),
		FontSize(_FontSize),
		Texture(_Texture),
		TexturePositionLeft(_TexturePositionLeft),
		TexturePositionRight(_TexturePositionRight),
		TexturePositionTop(_TexturePositionTop),
		TexturePositionBottom(_TexturePositionBottom),
		Width(_Width),
		Height(_Height),
		Left(_Left),
		Top(_Top),
		AdvanceX(_AdvanceX)
	{};
};

/* List with connected font characters. */
std::forward_list<eFontChar*> FontChars_List;


/*
 * Font initialization by font name (path to file).
 */
int vw_InitFont(const char *FontName)
{
	if (FT_Init_FreeType(&InternalLibrary)) {
		fprintf(stderr, "Can't initialize library, font: %s\n", FontName);
		return -1;
	}

	if (InternalFontBuffer != nullptr) {
		delete [] InternalFontBuffer;
		InternalFontBuffer = nullptr;
	}

	eFILE* FontFile = vw_fopen(FontName);
	if (FontFile == nullptr) {
		fprintf(stderr, "Can't open font file: %s\n", FontName);
		return -1;
	}

	FontFile->fseek(0, SEEK_END);
	int FontBufferSize = FontFile->ftell();
	FontFile->fseek(0, SEEK_SET);

	InternalFontBuffer = new BYTE[FontBufferSize];
	FontFile->fread(InternalFontBuffer, FontBufferSize, 1);

	vw_fclose(FontFile);

	if (FT_New_Memory_Face(InternalLibrary, InternalFontBuffer, FontBufferSize, 0, &InternalFace)) {
		fprintf(stderr, "Can't create font face from memory, font: %s\n", FontName);
		return -1;
	}

	printf("Font initialized: %s\n\n", FontName);
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
	for (eFontChar *Tmp : FontChars_List) {
		if ((Tmp->UTF32 == UTF32) &&
		    (Tmp->FontSize == InternalFontSize))
			return Tmp;
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
	/* release all font characters */
	while (!FontChars_List.empty()) {

		/* TODO this should be fixed in "texture" subsystem, this part is ugly */

		/* release texture, created to this character */
		if (FontChars_List.front()->Texture != nullptr) {
			/* one texture could be used by many characters
			 * make sure, we don't release one texture twice */
			eTexture *Texture = FontChars_List.front()->Texture;
			for (eFontChar *Tmp : FontChars_List) {
				if (Tmp->Texture == Texture)
					Tmp->Texture = nullptr;
			}
			vw_ReleaseTexture(Texture);
		}
		delete FontChars_List.front();
		FontChars_List.pop_front();
	}
	/* reset list */
	FontChars_List.clear();
}

/*
 *  Shutdown font.
 */
void vw_ShutdownFont()
{
	if (InternalFontBuffer != nullptr) {
		delete [] InternalFontBuffer;
		InternalFontBuffer = nullptr;
	}
}

/*
 * Load data and generate font character.
 */
static eFontChar* LoadFontChar(unsigned UTF32)
{
	/* setup parameters */
	if (FT_Set_Char_Size(InternalFace, InternalFontSize << 6, InternalFontSize << 6, 96, 96)) {
		fprintf(stderr, "Can't set char size %i.", InternalFontSize);
		return nullptr;
	}
	/* load glyph */
	if (FT_Load_Char( InternalFace, UTF32, FT_LOAD_RENDER | FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT)) {
		fprintf(stderr, "Can't load glyph: %u\n", UTF32);
		return nullptr;
	}

	/* create new character */
	eFontChar *NewChar = new eFontChar(UTF32, InternalFontSize, nullptr,
					   0, InternalFace->glyph->bitmap.width, 0, InternalFace->glyph->bitmap.rows,
					   InternalFace->glyph->bitmap.width, InternalFace->glyph->bitmap.rows,
					   InternalFace->glyph->bitmap_left, InternalFace->glyph->bitmap_top,
					   InternalFace->glyph->advance.x / 64.0f);

	if ((NewChar->Width > 0) && (NewChar->Height > 0)) {
		/* buffer for RGBA, data for font characters texture */
		BYTE *pixels = new BYTE[NewChar->Width*NewChar->Height*4];
		/* white color for RGBA, alpha channel will be corrected later */
		memset(pixels, 255, NewChar->Width*NewChar->Height*4);
		/* convert greyscale to RGB+Alpha (32bits), now we need correct only alpha channel */
		for (int j = 0; j < NewChar->Height; j++) {
			int StrideSrc = j*NewChar->Width*4;
			int StrideDst = (NewChar->Height - j - 1)*NewChar->Width;
			for (int i = 0; i < NewChar->Width; i++) {
				/* alpha channel */
				memcpy(pixels + StrideSrc + i*4 + 3, InternalFace->glyph->bitmap.buffer + StrideDst + i, 1);
			}
		}

		/* fake texture file name based on UTF32 code */
		char FakeTExtureFileName[MAX_PATH];
		sprintf(FakeTExtureFileName, "fontsize_%i_character_%u", InternalFontSize, UTF32);

		vw_SetTextureProp(RI_MAGFILTER_LINEAR | RI_MINFILTER_LINEAR | RI_MIPFILTER_NONE,
				  RI_CLAMP_TO_EDGE, true, TX_ALPHA_GREYSC, false);
		NewChar->Texture = vw_CreateTextureFromMemory(FakeTExtureFileName, pixels,
							      NewChar->Width, NewChar->Height,
							      4, 0, 0, 0, false);
		delete [] pixels;
	}

	/* push font character to list */
	FontChars_List.push_front(NewChar);
	return NewChar;
}

/*
 * Generate font characters by list.
 */
void vw_GenerateFontChars(int FontTextureWidth, int FontTextureHeight, const char *CharsList)
{
	printf("Font characters generation start.\n");

	/* fake texture file name based on CharsList */
	const char *TextureName = CharsList;
	/* buffer for RGBA, data for font characters texture */
	BYTE *DIB = new BYTE[FontTextureWidth*FontTextureHeight*4];
	/* make sure, DIB filled by black and alpha set to zero,
	 * or we will have white borders on each character
	 */
	memset(DIB, 0, FontTextureWidth*FontTextureHeight*4);

	/* initial setup */
	int CurrentDIBX = 0;
	int CurrentDIBY = 0;
	int EdgingSpace = 2;
	int MaxHeightInCurrentLine = 0;

	if (FT_Set_Char_Size(InternalFace, InternalFontSize << 6, InternalFontSize << 6, 96, 96)) {
		fprintf(stderr, "Can't set char size %i.", InternalFontSize);
		delete [] DIB;
		return;
	}

	/* create one large bitmap with all font characters from list */
	const char *CharsList2 = CharsList;
	while (strlen(CharsList) > 0) {
		unsigned CurrentChar;
		/* convert into UTF32 code */
		CharsList = utf8_to_utf32(CharsList, &CurrentChar);

		/* load glyph*/
		if (FT_Load_Char(InternalFace, CurrentChar, FT_LOAD_RENDER | FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT)) {
			fprintf(stderr, "Can't load Char: %u\n", CurrentChar);
			delete [] DIB;
			return;
		}

		eFontChar *NewChar = new eFontChar(CurrentChar, InternalFontSize, nullptr,
						   0, 0, 0, 0,
						   InternalFace->glyph->bitmap.width, InternalFace->glyph->bitmap.rows,
						   InternalFace->glyph->bitmap_left, InternalFace->glyph->bitmap_top,
						   InternalFace->glyph->advance.x / 64.0f);

		/* move to next line in bitmap if not enough space */
		if (CurrentDIBX + NewChar->Width > FontTextureWidth) {
			CurrentDIBX = 0;
			CurrentDIBY += MaxHeightInCurrentLine + EdgingSpace;
			MaxHeightInCurrentLine = 0;
		}
		/* looks like no more space left at all, fail */
		if (CurrentDIBY + NewChar->Height > FontTextureHeight) {
			fprintf(stderr, "Can't generate all font chars in one texture. Too many chars or too small texture size!\n");
			delete NewChar;
			break;
		}

		/* copy glyph into bitmap */
		BYTE ColorRGB[3] = {255,255,255};
		for (int j = 0; j < NewChar->Height; j++) {
			for (int i = 0; i < NewChar->Width; i++) {
				memcpy(DIB + (FontTextureHeight-CurrentDIBY-j-1)*FontTextureWidth*4 + (CurrentDIBX+i)*4,
				       ColorRGB, 3);
				memcpy(DIB + (FontTextureHeight-CurrentDIBY-j-1)*FontTextureWidth*4 + (CurrentDIBX+i)*4 + 3,
				       InternalFace->glyph->bitmap.buffer+j*NewChar->Width+i, 1);
			}
		}

		/* setup new character */
		NewChar->TexturePositionLeft = CurrentDIBX;
		NewChar->TexturePositionRight = CurrentDIBX + NewChar->Width;
		NewChar->TexturePositionTop = CurrentDIBY;
		NewChar->TexturePositionBottom = CurrentDIBY + NewChar->Height;

		/* detect new line position by height */
		if (MaxHeightInCurrentLine < NewChar->Height)
			MaxHeightInCurrentLine = NewChar->Height;
		CurrentDIBX += NewChar->Width + EdgingSpace;

		/* push font character to list */
		FontChars_List.push_front(NewChar);
	}

	/* create texture from bitmap */
	vw_SetTextureProp(RI_MAGFILTER_LINEAR | RI_MINFILTER_LINEAR | RI_MIPFILTER_NONE, RI_CLAMP_TO_EDGE, true, TX_ALPHA_GREYSC, false);
	eTexture *FontTexture = vw_CreateTextureFromMemory(TextureName, DIB, FontTextureWidth, FontTextureHeight, 4, 0);
	/* release memory */
	delete [] DIB;
	if (FontTexture == nullptr) {
		fprintf(stderr, "Can't create font texture.\n");
		return;
	}

	/* setup texture to all font characters from list */
	while (strlen(CharsList2) > 0) {
		unsigned CurrentChar;
		CharsList2 = utf8_to_utf32(CharsList2, &CurrentChar);
		eFontChar *TMPChar = FindFontCharByUTF32(CurrentChar);
		if (TMPChar != nullptr)
			TMPChar->Texture = FontTexture;
	}

	printf("Font characters generation end.\n\n");
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

	/* care about aspect ration */
	float AW, AH;
	bool ASpresent = vw_GetAspectWH(&AW, &AH);
	/* get current viewport */
	int W, H;
	vw_GetViewport(nullptr, nullptr, &W, &H);
	float AHw = H*1.0f;

	/* check text position */
	if (ASpresent) {
		if (Y > AH)
			return;
	} else {
		if (Y > H)
			return;
	}
	if (Y + InternalFontSize*FontScale < 0)
		return;

	/* get string with variable arguments */
	char text[1024];
	va_list ap;
	va_start(ap, Text);
	vsprintf(text, Text, ap);
	va_end(ap);
	if (strlen(text) == 0)
		return;

	float Xstart = X;
	/* calculate default space width */
	if (FindFontCharByUTF32(0x020) == nullptr)
		LoadFontChar(0x020);
	float SpaceWidth = FindFontCharByUTF32(0x020)->AdvanceX*FontScale;
	/* make sure, we have space width at least 65% of current font size */
	if (SpaceWidth < (InternalFontSize * 0.65f)) SpaceWidth = InternalFontSize * 0.65f;
	// коэф. изменения букв по ширине
	float FontWidthScale = 1.0f;

	if (Transp >= 1.0f) Transp = 1.0f;

	// если нужно выравнивать, считаем данные пробелов
	if (StrictWidth > 0) {
		float LineWidth = 0;
		int SpaceCount = 0;

		const char *CountCheck = text;
		while (strlen(CountCheck) > 0) {
			unsigned UTF32;
			// преобразуем в утф32 и "сдвигаемся" на следующий символ в строке
			CountCheck = utf8_to_utf32(CountCheck, &UTF32);
			// находим наш текущий символ
			eFontChar *DrawChar = FindFontCharByUTF32(UTF32);
			if (DrawChar == nullptr)
				DrawChar = LoadFontChar(UTF32);

			// считаем кол-во пробелов
			if (UTF32 == 0x020)
				SpaceCount++;
			else
				LineWidth += DrawChar->AdvanceX;
		}

		if ((StrictWidth > LineWidth) &&
		    (SpaceCount != 0))
			SpaceWidth = (StrictWidth - LineWidth)/SpaceCount;
	}
	// если нужно сжать, считаем коэф. сжатия букв
	if (StrictWidth < 0) {
		float LineWidth = 0;

		const char *CountCheck = text;
		while (strlen(CountCheck) > 0) {
			unsigned UTF32;
			// преобразуем в утф32 и "сдвигаемся" на следующий символ в строке
			CountCheck = utf8_to_utf32(CountCheck, &UTF32);
			// находим наш текущий символ
			eFontChar *DrawChar = FindFontCharByUTF32(UTF32);
			if (DrawChar == nullptr)
				DrawChar = LoadFontChar(UTF32);

			// считаем длину символов с пробелами
			if (UTF32 != 0x020)
				LineWidth += DrawChar->AdvanceX;
			else
				LineWidth += SpaceWidth;
		}

		if (StrictWidth*(-1.0f) < LineWidth) FontWidthScale = StrictWidth/LineWidth*(-1.0f);
	}

	float LineWidth = 0;

	// установка свойств текстуры
	vw_SetTextureBlend(true, RI_BLEND_SRCALPHA, RI_BLEND_INVSRCALPHA);
	// ставим цвет
	vw_SetColor(R, G, B, Transp);

	// для отрисовки
	eTexture *CurrentTexture = nullptr;
	int k = 0;
	// буфер для последовательности RI_QUADS
	// войдет RI_2f_XYZ | RI_2f_TEX
	float *tmp = new float[(2+2)*4*strlen(text)];

	// чтобы меньше делать операций умножения, включаем коэф. один в другой сразу для ширины символов
	FontWidthScale = FontScale*FontWidthScale;

	// прорисовка текста
	const char *textdraw = text;
	// прорисовываем все символы
	while (strlen(textdraw) > 0) {
		unsigned UTF32;
		// преобразуем в утф32 и "сдвигаемся" на следующий символ в строке
		textdraw = utf8_to_utf32(textdraw, &UTF32);
		// находим наш текущий символ
		eFontChar *DrawChar = FindFontCharByUTF32(UTF32);
		if (DrawChar == nullptr)
			DrawChar = LoadFontChar(UTF32);
		// первый символ - запоминаем его текстуру
		if (CurrentTexture == nullptr)
			CurrentTexture = DrawChar->Texture;

		/* looks like texture should be changed */
		if (CurrentTexture != DrawChar->Texture) {
			/* draw all we have in buffer with current texture */
			if (k > 0) {
				vw_SetTexture(0, CurrentTexture);
				vw_SendVertices(RI_QUADS, 4*(k/16), RI_2f_XY | RI_1_TEX, tmp, 4*sizeof(float));
			}

			/* setup new texture */
			CurrentTexture = DrawChar->Texture;
			/* reset vertex counter */
			k = 0;
		}

		// если не пробел - рисуем
		if (UTF32 != 0x020) {
			float DrawX = Xstart + DrawChar->Left*FontWidthScale;
			float DrawY = Y + GlobalFontOffsetY + (InternalFontSize - DrawChar->Top)*FontScale;

			// Вычисление поправки по У в зависимости от DrawCorner
			// - расположения угла начала координат
			float tmpPosY = 0;
			// изменяем только в случае RI_UL_CORNER
			if (ASpresent) tmpPosY = (AH - DrawY - DrawY - DrawChar->Height*FontScale);
			else tmpPosY = (AHw - DrawY - DrawY - DrawChar->Height*FontScale);

			float ImageHeight = DrawChar->Texture->Height*1.0f;
			float ImageWidth = DrawChar->Texture->Width*1.0f;

			float FrameHeight = (DrawChar->TexturePositionBottom*1.0f )/ImageHeight;
			float FrameWidth = (DrawChar->TexturePositionRight*1.0f )/ImageWidth;

			float Yst = (DrawChar->TexturePositionTop*1.0f)/ImageHeight;
			float Xst = (DrawChar->TexturePositionLeft*1.0f)/ImageWidth;

			tmp[k++] = DrawX;
			tmp[k++] = DrawY + tmpPosY + DrawChar->Height*FontScale;
			tmp[k++] = Xst;
			tmp[k++] = 1.0f - Yst;

			tmp[k++] = DrawX;
			tmp[k++] = DrawY + tmpPosY;
			tmp[k++] = Xst;
			tmp[k++] = 1.0f - FrameHeight;

			tmp[k++] = DrawX + DrawChar->Width*FontWidthScale;
			tmp[k++] = DrawY + tmpPosY;
			tmp[k++] = FrameWidth;
			tmp[k++] = 1.0f-FrameHeight;

			tmp[k++] = DrawX + DrawChar->Width*FontWidthScale;
			tmp[k++] = DrawY +tmpPosY + DrawChar->Height*FontScale;
			tmp[k++] = FrameWidth;
			tmp[k++] = 1.0f - Yst;

			Xstart += DrawChar->AdvanceX*FontWidthScale;
			LineWidth += DrawChar->AdvanceX*FontWidthScale;
		} else {
			Xstart += SpaceWidth*FontWidthScale;
			LineWidth += SpaceWidth*FontWidthScale;
		}

		// если нужно прорисовывать с ограничением по длине
		if ((ExpandWidth != 0.0f) &&
		    (LineWidth >= ExpandWidth))
			break;
	}

	// если что-то было в буфере - выводим
	if (k > 0) {
		// Установка текстуры
		vw_SetTexture(0, CurrentTexture);
		// отрисовываем все что есть в буфере
		vw_SendVertices(RI_QUADS, 4*(k/16), RI_2f_XY | RI_1_TEX, tmp, 4*sizeof(float));
	}

	if (tmp != nullptr)
		delete [] tmp;
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

	/* get string with variable arguments */
	char text[1024];
	va_list ap;
	va_start(ap, Text);
	vsprintf(text, Text, ap);
	va_end(ap);
	if (strlen(text) == 0)
		return 0;

	const char *textdraw = text;
	/* calculate default space width */
	if (FindFontCharByUTF32(0x020) == nullptr)
		LoadFontChar(0x020);
	float SpaceWidth = FindFontCharByUTF32(0x020)->AdvanceX;
	/* make sure, we have space width at least 65% of current font size */
	if (SpaceWidth < (InternalFontSize * 0.65f)) SpaceWidth = InternalFontSize * 0.65f;

	float LineWidth = 0;
	while (strlen(textdraw) > 0) {
		unsigned UTF32;
		// преобразуем в утф32 и "сдвигаемся" на следующий символ в строке
		textdraw = utf8_to_utf32(textdraw, &UTF32);
		// находим наш текущий символ
		eFontChar *DrawChar = FindFontCharByUTF32(UTF32);
		if (DrawChar == nullptr)
			DrawChar = LoadFontChar(UTF32);

		// считаем кол-во пробелов
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

	/* get string with variable arguments */
	char text[1024];
	va_list ap;
	va_start(ap, Text);
	vsprintf(text, Text, ap);
	va_end(ap);
	if (strlen(text) == 0)
		return;

	// прорисовка текста
	const char *textdraw = text;

	float Xstart = 0.0f;
	/* calculate default space width */
	float SpaceWidth = FindFontCharByUTF32(0x020)->AdvanceX;
	/* make sure, we have space width at least 65% of current font size */
	if (SpaceWidth < (InternalFontSize * 0.65f))
		SpaceWidth = InternalFontSize * 0.65f;

	textdraw = text;

	// для отрисовки
	eTexture* CurrentTexture = nullptr;
	int k = 0;
	// буфер для последовательности RI_QUADS
	// войдет RI_2f_XY | RI_2f_TEX
	float *tmp = new float[(2 + 2) * 4 * strlen(textdraw)];

	// установка свойств текстуры
	vw_SetTextureBlend(true, RI_BLEND_SRCALPHA, RI_BLEND_INVSRCALPHA);
	// всегда стаим белый цвет
	vw_SetColor(1.0f, 1.0f, 1.0f, 1.0f);

	vw_PushMatrix();

	vw_Translate(VECTOR3D(X, Y, Z));
	VECTOR3D CurrentCameraRotation;
	vw_GetCameraRotation(&CurrentCameraRotation);

	// поворачиваем к камере
	vw_Rotate(CurrentCameraRotation.y, 0.0f, 1.0f, 0.0f);
	vw_Rotate(CurrentCameraRotation.x, 1.0f, 0.0f, 0.0f);

	// прорисовываем все символы
	while (strlen(textdraw) > 0) {
		unsigned UTF32;
		// преобразуем в утф32 и "сдвигаемся" на следующий символ в строке
		textdraw = utf8_to_utf32(textdraw, &UTF32);
		// находим наш текущий символ
		eFontChar* DrawChar = FindFontCharByUTF32(UTF32);
		if (DrawChar == nullptr)
			DrawChar = LoadFontChar(UTF32);
		// первый символ - запоминаем его текстуру
		if (CurrentTexture == nullptr)
			CurrentTexture = DrawChar->Texture;

		/* looks like texture should be changed */
		if (CurrentTexture != DrawChar->Texture) {
			/* draw all we have in buffer with current texture */
			if (k > 0) {
				vw_SetTexture(0, CurrentTexture);
				vw_SendVertices(RI_QUADS, 4*(k/16), RI_2f_XY | RI_1_TEX, tmp, 4*sizeof(float));
			}

			/* setup new texture */
			CurrentTexture = DrawChar->Texture;
			/* reset vertex counter */
			k = 0;
		}

		// если не пробел - рисуем
		if (UTF32 != 0x020) {
			float DrawX = Xstart + DrawChar->Left;
			float DrawY = InternalFontSize - DrawChar->Top;

			float ImageHeight = DrawChar->Texture->Height*1.0f;
			float ImageWidth = DrawChar->Texture->Width*1.0f;

			float FrameHeight = (DrawChar->TexturePositionBottom*1.0f )/ImageHeight;
			float FrameWidth = (DrawChar->TexturePositionRight*1.0f )/ImageWidth;

			float Yst = (DrawChar->TexturePositionTop*1.0f)/ImageHeight;
			float Xst = (DrawChar->TexturePositionLeft*1.0f)/ImageWidth;

			tmp[k++] = DrawX/10.0f;
			tmp[k++] = (DrawY + DrawChar->Height)/10.0f;
			tmp[k++] = Xst;
			tmp[k++] = 1.0f - Yst;

			tmp[k++] = DrawX/10.0f;
			tmp[k++] = DrawY/10.0f;
			tmp[k++] = Xst;
			tmp[k++] = 1.0f - FrameHeight;

			tmp[k++] = (DrawX + DrawChar->Width)/10.0f;
			tmp[k++] = DrawY/10.0f;
			tmp[k++] = FrameWidth;
			tmp[k++] = 1.0f - FrameHeight;

			tmp[k++] = (DrawX + DrawChar->Width)/10.0f;
			tmp[k++] = (DrawY + DrawChar->Height)/10.0f;
			tmp[k++] = FrameWidth;
			tmp[k++] = 1.0f - Yst;

			Xstart += DrawChar->AdvanceX;
		} else {
			Xstart += SpaceWidth;
		}
	}

	// если что-то было в буфере - выводим
	if (k > 0) {
		// Установка текстуры
		vw_SetTexture(0, CurrentTexture);
		// отрисовываем все что есть в буфере
		vw_SendVertices(RI_QUADS, 4*(k/16), RI_2f_XY | RI_1_TEX, tmp, 4*sizeof(float));
	}

	vw_PopMatrix();

	if (tmp != nullptr)
		delete [] tmp;
	vw_SetTextureBlend(false, 0, 0);
	vw_BindTexture(0, 0);
}
