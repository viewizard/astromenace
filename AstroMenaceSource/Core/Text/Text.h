/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: Text.h

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


#ifndef TEXT_H
#define TEXT_H


#include "../Base.h"
#include "../Font/Font.h"
#include "../VirtualFileSystem/VFS.h"




// загружаем текстовый .csv
void vw_InitText(const char *FileName, const char SymbolSeparator, const char SymbolEndOfLine);
// устанавливаем язык
void vw_SetTextLanguage(int Language);
// получаем текст из файлы
const char *vw_GetText(const char *ItemID);
// проверяем, есть ли символ в фонте, перебираем по тексту всех языков
int vw_CheckFontCharsInText();
// освобождаем данные
void vw_ReleaseText();



#endif // TEXT_H

