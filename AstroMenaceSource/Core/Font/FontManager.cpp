/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2014 Michael Kurinnoy, Viewizard


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
// смещение при прорисовки текста
int			GlobalFontOffsetY = 0;

eFontChar *StartFontChar = 0;
eFontChar *EndFontChar = 0;



//------------------------------------------------------------------------------------
// инициализация фонта
//------------------------------------------------------------------------------------
int vw_InitFont(const char *FontName)
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


	printf("Font initialized: %s\n\n", FontName);
	return 0;
}


//------------------------------------------------------------------------------------
// устанавливаем размеры шрифта
//------------------------------------------------------------------------------------
void vw_SetFontSize(int FontSize)
{
	InternalFontSize = FontSize;
}





//------------------------------------------------------------------------------------
// задаем фиксированное смещение при прорисовке через vw_DrawFont
//------------------------------------------------------------------------------------
void vw_SetFontOffsetY(int NewOffsetY)
{
	GlobalFontOffsetY = NewOffsetY;
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
		if (Tmp->UTF32 == UTF32 && Tmp->FontSize == InternalFontSize) return Tmp;
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
void vw_ReleaseAllFontChars()
{
	// удаляем все символы
	eFontChar *tmp = StartFontChar;
	while (tmp!=0)
	{
		eFontChar *tmp2 = tmp->Next;
		delete tmp;
		tmp = tmp2;
	}

	StartFontChar = 0;
	EndFontChar = 0;
}



//-----------------------------------------------------------------------------
// освобождаем память от сгенерированных символов с удалением текстур
//-----------------------------------------------------------------------------
void vw_ReleaseAllFontCharsWithTextures()
{
	// удаляем все символы
	eFontChar *tmp = StartFontChar;
	while (tmp!=0)
	{
		eFontChar *tmp2 = tmp->Next;

		// удаляем текстуру
		if (tmp->CharTexture != 0)
		{
			// проверяем все символы и "зануляем" указатель текстуры если другой символ использует эту же текстуру
			eFontChar *tmp_texture = tmp->Next;
			while (tmp_texture!=0)
			{
				if (tmp_texture->CharTexture == tmp->CharTexture) tmp_texture->CharTexture = 0;
				tmp_texture = tmp_texture->Next;
			}

			// удаляем текстуру
			vw_ReleaseTexture(tmp->CharTexture);
		}

		delete tmp;
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
	// удаляем все символы, в этом случае всегда только символы без текстур
	vw_ReleaseAllFontChars();

	// освобождаем буфер фонта
	if (InternalFontBuffer != 0)
	{
		delete [] InternalFontBuffer; InternalFontBuffer =0;
	}

}
