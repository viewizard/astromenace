/************************************************************************************

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


	Web Site: http://www.viewizard.com/
	Project: http://sourceforge.net/projects/openastromenace/
	E-mail: viewizard@viewizard.com

*************************************************************************************/


#include "Font.h"


// интерфейс библиотеки
FT_Library  InternalLibrary;
// интерфейс объекта шрифта
FT_Face     InternalFace;
// буфер фонта
BYTE 		*InternalFontBuffer = 0;
// размер фонта при генерации
int 		InternalFontSize;

eFontChar *StartFontChar = 0;
eFontChar *EndFontChar = 0;



//------------------------------------------------------------------------------------
// инициализация фонта
//------------------------------------------------------------------------------------
int vw_InitFont(const char *FontName, int FontSize)
{
	// иним библиотеку
	if (FT_Init_FreeType( &InternalLibrary ))
	{
		fprintf(stderr, "Can't initialize library, font: %s\n", FontName);
		return -1;
	}

	// подготавливаем данные для загрузки шрифта из памяти
	if (InternalFontBuffer != 0){delete [] InternalFontBuffer; InternalFontBuffer =0;}
	int FontBufferSize = 0;
	eFILE* FontFile = vw_fopen(FontName);

	if (!FontFile)
	{
		fprintf(stderr, "Can't open font file: %s\n", FontName);
		return -1;
	}

	FontFile->fseek( 0, SEEK_END );
	FontBufferSize = FontFile->ftell();
	FontFile->fseek( 0, SEEK_SET );

	InternalFontBuffer = new BYTE[FontBufferSize];
	FontFile->fread( InternalFontBuffer, FontBufferSize, 1 );

	vw_fclose(FontFile);


	// создаем фейс
	if (FT_New_Memory_Face( InternalLibrary, InternalFontBuffer, FontBufferSize, 0, &InternalFace ))
	{
		fprintf(stderr, "Can't create font face from memory, font: %s\n", FontName);
		return -1;
	}


	// устанавливаем размеры
	if (FT_Set_Char_Size( InternalFace, FontSize <<6, FontSize <<6, 96, 96 ))
	{
		fprintf(stderr, "Can't set char size, font: %s\n", FontName);
		return -1;
	}

	InternalFontSize = FontSize;

	printf("Font initialized: %s\n\n", FontName);
	return 0;
}




//------------------------------------------------------------------------------------
// Нахождение символа по его UTF32
//------------------------------------------------------------------------------------
eFontChar* vw_FindFontCharByUTF32(unsigned UTF32)
{
	eFontChar *Tmp = StartFontChar;

	while (Tmp != 0)
	{
		eFontChar *Tmp1 = Tmp->Next;
		if (Tmp->UTF32 == UTF32) return Tmp;
		Tmp = Tmp1;
	}

	return 0;
}



//------------------------------------------------------------------------------------
// подключение символа к менеджеру фонта
//------------------------------------------------------------------------------------
void vw_AttachFontChar(eFontChar* FontChar)
{
	if (FontChar == 0) return;

	// первый в списке...
	if (EndFontChar == 0)
	{
		FontChar->Prev = 0;
		FontChar->Next = 0;
		StartFontChar = FontChar;
		EndFontChar = FontChar;
	}
	else // продолжаем заполнение...
	{
		FontChar->Prev = EndFontChar;
		FontChar->Next = 0;
		EndFontChar->Next = FontChar;
		EndFontChar = FontChar;
	}
}





//------------------------------------------------------------------------------------
// отключение символа от менеджера фонта
//------------------------------------------------------------------------------------
void vw_DetachFontChar(eFontChar* FontChar)
{
	if (FontChar == 0) return;

	// переустанавливаем указатели...
	if (StartFontChar == FontChar) StartFontChar = FontChar->Next;
	if (EndFontChar == FontChar) EndFontChar = FontChar->Prev;


	if (FontChar->Next != 0) FontChar->Next->Prev = FontChar->Prev;
		else if (FontChar->Prev != 0) FontChar->Prev->Next = 0;
	if (FontChar->Prev != 0) FontChar->Prev->Next = FontChar->Next;
		else if (FontChar->Next != 0) FontChar->Next->Prev = 0;
}



//-----------------------------------------------------------------------------
// освобождаем память от сгенерированных символов
//-----------------------------------------------------------------------------
void vw_ReleaseAllFontChar()
{
	// удаляем все символы
	eFontChar *tmp = StartFontChar;
	while (tmp!=0)
	{
		eFontChar *tmp2 = tmp->Next;
		delete tmp; tmp = 0;
		tmp = tmp2;
	}

	StartFontChar = 0;
	EndFontChar = 0;
}



//-----------------------------------------------------------------------------
// завершаем работу, освобождаем память
//-----------------------------------------------------------------------------
void vw_ShutdownFont()
{
	// удаляем все символы
	vw_ReleaseAllFontChar();

	// освобождаем буфер фонта
	if (InternalFontBuffer != 0)
	{
		delete [] InternalFontBuffer; InternalFontBuffer =0;
	}

}




//------------------------------------------------------------------------------------
// создаем текстуру с текстом который передали
//------------------------------------------------------------------------------------
int vw_TextureFromText(const char *FontName, int FontSize, const char * Text)
{
	if (Text == 0) return -1;
	if (FontName == 0) return -1;
	if (FontSize <= 0) return -1;

	// интерфейс библиотеки
	FT_Library  TextureLibrary;
	// интерфейс объекта шрифта
	FT_Face     TextureFace;
	// буфер фонта
	BYTE *TextureFontBuffer = 0;

	// иним библиотеку
	if (FT_Init_FreeType( &TextureLibrary ))
	{
		fprintf(stderr, "Can't initialize library, font: %s\n", FontName);
		return -1;
	}

	eFILE* FontFile = vw_fopen(FontName);

	if (!FontFile)
	{
		fprintf(stderr, "Can't open font file: %s\n", FontName);
		return -1;
	}

	FontFile->fseek( 0, SEEK_END );
	int FontBufferSize = 0;
	FontBufferSize = FontFile->ftell();
	FontFile->fseek( 0, SEEK_SET );

	TextureFontBuffer = new BYTE[FontBufferSize];
	FontFile->fread( TextureFontBuffer, FontBufferSize, 1 );

	vw_fclose(FontFile);

	// создаем фейс
	if (FT_New_Memory_Face( TextureLibrary, TextureFontBuffer, FontBufferSize, 0, &TextureFace ))
	{
		fprintf(stderr, "Can't create font face from memory, font: %s\n", FontName);
		return -1;
	}

	// устанавливаем размеры
	if (FT_Set_Char_Size( TextureFace, FontSize <<6, FontSize <<6, 96, 96 ))
	{
		fprintf(stderr, "Can't set char size, font: %s\n", FontName);
		return -1;
	}



	// считаем размер текстуры
	int NeedTextureWidth = 0;
	int NeedTextureHeight = 0;
	const char *Text1 = Text;
	while (strlen(Text1) > 0)
	{
		unsigned CurrentChar;
		// преобразуем в утф32 и "сдвигаемся" на следующий символ в строке
		Text1 = utf8_to_utf32(Text1, &CurrentChar);

		// загрузка глифа нужного нам символа
		if (FT_Load_Char( TextureFace, CurrentChar, FT_LOAD_RENDER | FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT))
		{
			fprintf(stderr, "Can't load Char: %u\n", CurrentChar);
			return -1;
		}

		// учитываем размер каждого символа
		NeedTextureWidth += TextureFace->glyph->bitmap.width+TextureFace->glyph->bitmap_left;
		if (CurrentChar == 0x020) NeedTextureWidth += FontSize * 2 / 3; // пробел

		int CurrentGlyphHeight = FontSize+TextureFace->glyph->bitmap.rows-TextureFace->glyph->bitmap_top;
		if (NeedTextureHeight < CurrentGlyphHeight) NeedTextureHeight = CurrentGlyphHeight;

	}



	// создаем массив
	BYTE * DIB;
	DIB = new BYTE[NeedTextureWidth*NeedTextureHeight*4]; // всегда делаем rgba
	// устанавливаем 0 везде, чтобы потом не краcить rgb, а только формировать альфу
	memset(DIB, 0, NeedTextureWidth*NeedTextureHeight*4);

	// данные для работы с вклеиванием в текстуру
	int CurrentDIBX = 0;

	const char *Text2 = Text;
	while (strlen(Text2) > 0)
	{
		unsigned CurrentChar;
		// преобразуем в утф32 и "сдвигаемся" на следующий символ в строке
		Text2 = utf8_to_utf32(Text2, &CurrentChar);


		// загрузка глифа нужного нам символа
		if (FT_Load_Char( TextureFace, CurrentChar, FT_LOAD_RENDER | FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT))
		{
			fprintf(stderr, "Can't load Char: %u\n", CurrentChar);
			return -1;
		}

		CurrentDIBX += TextureFace->glyph->bitmap_left;
		if (CurrentDIBX < 0) CurrentDIBX = 0;

		// "вклеиваем" новый символ в массив
		BYTE ColorRGB[3]={255,255,255};
		int PosYcorrection = FontSize-TextureFace->glyph->bitmap_top;
		if (CurrentChar != 0x020)
		for (int j=PosYcorrection; j<TextureFace->glyph->bitmap.rows+PosYcorrection; j++)
		for (int i=0; i<TextureFace->glyph->bitmap.width; i++)
		{
			memcpy(DIB + (NeedTextureHeight-j-1)*NeedTextureWidth*4 + (CurrentDIBX+i)*4,
					ColorRGB,
					3);
			memcpy(DIB + (NeedTextureHeight-j-1)*NeedTextureWidth*4 + (CurrentDIBX+i)*4 + 3,
					TextureFace->glyph->bitmap.buffer+(j-PosYcorrection)*TextureFace->glyph->bitmap.width+i,
					1);
		}

		// учитываем размер каждого символа
		CurrentDIBX += TextureFace->glyph->bitmap.width;
		if (CurrentChar == 0x020) CurrentDIBX += 16 * 2 / 3; // пробел
	}


	// создаем текстуру
	vw_SetTextureProp(RI_MAGFILTER_LINEAR | RI_MINFILTER_LINEAR | RI_MIPFILTER_NONE, RI_CLAMP_TO_EDGE, true, TX_ALPHA_GREYSC, false);
	eTexture* FontTexture = vw_CreateTextureFromMemory(Text, DIB, NeedTextureWidth, NeedTextureHeight, 4, false);


	// освобождаем память
	delete [] DIB;
	delete [] TextureFontBuffer;

	if (FontTexture == 0)
	{
		fprintf(stderr, "Can't create font texture.\n");
		return -1;
	}

	return 0;
}
