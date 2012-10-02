/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: FontManager.cpp

	Copyright (c) 2012 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 3.1

******************************************************************************

	AstroMenace game source code available under "dual licensing" model.
	The licensing options available are:

	* Commercial Licensing. This is the appropriate option if you are
	  creating proprietary applications and you are not prepared to
	  distribute and share the source code of your application.
	  Contact us for pricing at viewizard@viewizard.com

	* Open Source Licensing. This is the appropriate option if you want
	  to share the source code of your application with everyone you
	  distribute it to, and you also want to give them the right to share
	  who uses it. You should have received a copy of the GNU General Public
	  License version 3 with this source codes.
	  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/


#include "Font.h"


// интерфейс библиотеки
FT_Library  library;
// интерфейс объекта шрифта
FT_Face     face;
// буфер фонта
BYTE *FontBuffer = 0;
// размер фонта при генерации
int InternalFontSize;

eFontChar *StartFontChar = 0;
eFontChar *EndFontChar = 0;



//------------------------------------------------------------------------------------
// инициализация фонта
//------------------------------------------------------------------------------------
int vw_InitFont(const char *FontName, int FontSize)
{
	// иним библиотеку
	if (FT_Init_FreeType( &library ))
	{
		fprintf(stderr, "Can't initialize library, font: %s\n", FontName);
		return -1;
	}

	// подготавливаем данные для загрузки шрифта из памяти
	if (FontBuffer != 0){delete [] FontBuffer; FontBuffer =0;}
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

	FontBuffer = new BYTE[FontBufferSize];
	FontFile->fread( FontBuffer, FontBufferSize, 1 );

	vw_fclose(FontFile);


	// создаем фейс
	if (FT_New_Memory_Face( library, FontBuffer, FontBufferSize, 0, &face ))
	{
		fprintf(stderr, "Can't create font face from memory, font: %s\n", FontName);
		return -1;
	}


	// устанавливаем размеры
	if (FT_Set_Char_Size( face, FontSize <<6, FontSize <<6, 96, 96 ))
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
// завершаем работу, освобождаем память
//-----------------------------------------------------------------------------
void vw_ReleaseFont()
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

	// освобождаем буфер фонта
	if (FontBuffer != 0)
	{
		delete [] FontBuffer; FontBuffer =0;
	}

}

