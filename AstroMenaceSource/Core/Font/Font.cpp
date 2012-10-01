/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: Font.cpp

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



void vw_AttachFontChar(eFontChar* FontChar);
void vw_DetachFontChar(eFontChar* FontChar);
extern FT_Face face;




//------------------------------------------------------------------------------------
// загрузка и генерация всех необходимых данных для символа (utf32)
//------------------------------------------------------------------------------------
eFontChar* vw_LoadFontChar(unsigned UTF32)
{
	// прежде всего, пытаемся загрузить символ, а уже потом создаем структуру


	// загрузка глифа нужного нам символа
	if (FT_Load_Char( face, UTF32, FT_LOAD_RENDER | FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT))
	{
		fprintf(stderr, "Can't load Char: %u\n", UTF32);
		return 0;
	}


	// создаем структуру FontChar
	eFontChar* NewChar;
	NewChar = new eFontChar;

	NewChar->UTF32 = UTF32;
	NewChar->CharTexture = 0;
	NewChar->Width = face->glyph->bitmap.width;
	NewChar->Height = face->glyph->bitmap.rows;
	NewChar->Left = face->glyph->bitmap_left;
	NewChar->Top = face->glyph->bitmap_top;
	NewChar->Prev = 0;
	NewChar->Next = 0;


	BYTE * pixels;
	pixels = new BYTE[NewChar->Width*NewChar->Height*4];

	// битмап идет в 1 канале градаций серого, делаем 32бита rgba
	int k=0;
	for (int j=0; j<NewChar->Height; j++)
	{
		for (int i=0; i<NewChar->Width; i++)
		{
			memcpy(pixels + k, face->glyph->bitmap.buffer+j*NewChar->Width+i, 1);
			k++;
			memcpy(pixels + k, face->glyph->bitmap.buffer+j*NewChar->Width+i, 1);
			k++;
			memcpy(pixels + k, face->glyph->bitmap.buffer+j*NewChar->Width+i, 1);
			k++;
			memcpy(pixels + k, face->glyph->bitmap.buffer+j*NewChar->Width+i, 1);
			k++;
		}
		// должен быть приведен к DWORD построчно
		while((k % 4) != 0) k++;
	}

	// называем текстуру номером символа в утф32
	char texturefilename[MAX_PATH];
	sprintf(texturefilename, "%i", UTF32);

	NewChar->CharTexture = vw_CreateTextureFromMemory(texturefilename, pixels, NewChar->Width, NewChar->Height, 4, false);
	// очищаем память
	delete [] pixels;

	// подключаем к менеджеру
	vw_AttachFontChar(NewChar);
	return NewChar;
}




//-----------------------------------------------------------------------------
// делаем генерацию нужных символов по списку
//-----------------------------------------------------------------------------
void vw_GenerateFontChars(const char * CharsList)
{

	printf("Font characters generation start.\n");

	// перебираем всю строку
	while (strlen(CharsList) > 0)
	{
		unsigned CurrentChar;
		// преобразуем в утф32 и "сдвигаемся" на следующий символ в строке
		CharsList = utf8_to_utf32(CharsList, &CurrentChar);
		// загружаем символ и все необходимые данные для него
		vw_LoadFontChar(CurrentChar);
	}

	printf("Font characters generation end.\n\n");
}
