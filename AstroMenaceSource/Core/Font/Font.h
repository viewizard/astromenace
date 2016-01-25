/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright (c) 2006-2016 Mikhail Kurinnoi, Viewizard


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
	int			FontSize;

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
	float		AdvanceX;

	// указатели на список
	eFontChar*	Prev;
	eFontChar*	Next;
};




// инициализация фонта
int vw_InitFont(const char *FontName);
// устанавливаем текущий размер шрифта
void vw_SetFontSize(int FontSize);
// задаем фиксированное смещение при прорисовке через vw_DrawFont
void vw_SetFontOffsetY(int NewOffsetY);
// делаем генерацию нужных символов по списку
void vw_GenerateFontChars(int FontTextureWidth, int FontTextureHeight, const char * CharsList);
// загрузка символа с генерацией всех необходимых данных
eFontChar* vw_LoadFontChar(unsigned UTF32);
// Нахождение символа по его UTF32
eFontChar* vw_FindFontCharByUTF32(unsigned UTF32);
// освобождаем память от сгенерированных символов
void vw_ReleaseAllFontChars();
// освобождаем память от сгенерированных символов с удалением текстур
void vw_ReleaseAllFontCharsWithTextures();
// освобождаем память, завершаем работу с фонтом
void vw_ShutdownFont();


// получаем размер строки
int vw_FontSize(const char *Text, ...);
// вывод строки в 2д
void vw_DrawFont(int X, int Y, float FlattenWidth, float MaxWidth, float FontScale, float R, float G, float B, float Transp, const char *Text, ...);
// вывод строки в 3д
void vw_DrawFont3D(float X, float Y, float Z, const char *Text, ...);






#endif // FONT_H

