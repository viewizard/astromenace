/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: Font.h

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


#ifndef FONT_H
#define FONT_H


#include "../Core.h"
#include <ft2build.h>
#include FT_FREETYPE_H



//-----------------------------------------------------------------------------
// структура eFontChar
//-----------------------------------------------------------------------------
struct eFontChar
{
	// utf32 идентификатор
	unsigned 	UTF32;

	// указатель на сгенерированную текстуру
	eTexture* 	CharTexture;

	// указатели точек границ символа, на использовании общей текстуры
	int			TexturePositionLeft;
	int			TexturePositionRight;
	int			TexturePositionTop;
	int			TexturePositionBottom;

	// метрика растра символа
	int 		Width;
	int 		Height;
	int 		Left;
	int 		Top;

	// указатели на список
	eFontChar*	Prev;
	eFontChar*	Next;
};




// инициализация фонта
int vw_InitFont(const char *FontName, int FontSize);
// делаем генерацию нужных символов по списку
void vw_GenerateFontChars(const char * CharsList);
// загрузка символа с генерацией всех необходимых данных
eFontChar* vw_LoadFontChar(unsigned UTF32);
// Нахождение символа по его UTF32
eFontChar* vw_FindFontCharByUTF32(unsigned UTF32);
// освобождаем память
void vw_ReleaseFont();

// получаем размер строки
int vw_FontSize(const char *Text, ...);
// вывод строки в 2д
void vw_DrawFont(int X, int Y, float FlattenWidth, float MaxWidth, float FontScale, float R, float G, float B, float Transp, const char *Text, ...);
// вывод строки в 3д
void vw_DrawFont3D(float X, float Y, float Z, const char *Text, ...);






#endif // FONT_H

