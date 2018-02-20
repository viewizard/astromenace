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


#ifndef TEXT_H
#define TEXT_H


#include "../base.h"
#include "../font/font.h"
#include "../vfs/vfs.h"




struct sLanguageList {
	const char *code;
	const char *title;
};
sLanguageList *vw_GetLanguageList();
int vw_GetLanguageListCount();


// загружаем текстовый .csv
int vw_InitText(const char *FileName, const char SymbolSeparator, const char SymbolEndOfLine);
// устанавливаем язык
void vw_SetTextLanguage(int Language);
// получаем текст из файлы, Language=-1 - будет использован язык, установленный через vw_SetTextLanguage
const char *vw_GetText(const char *ItemID, int Language=-1);
// проверяем, есть ли символ в фонте, перебираем по тексту всех языков
int vw_CheckFontCharsInText();
// освобождаем данные
void vw_ReleaseText();



#endif // TEXT_H

