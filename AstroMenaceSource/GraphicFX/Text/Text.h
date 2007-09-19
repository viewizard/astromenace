/////////////////////////////////////////////////
// AstroMenace
/////////////////////////////////////////////////
// File: Text.h
/////////////////////////////////////////////////
// Copyright (c) 2006, Michael Kurinnoy
// All Rights Reserved.
/////////////////////////////////////////////////
// Ver : 1.0
/////////////////////////////////////////////////
/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Text.h

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.2

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


#include "../../Core/Core.h"




class CText
{
public:
	CText();
	~CText();

	// обновить
	virtual		bool Update(float Time);
	// прорисовка
	virtual		void Draw();

	// собственно сам текст
	char *DrawText;
	// положение текста
	int PosX;
	int PosY;
	// цвет
	int Color;

	// жизнь в секундах (-1.0 - бесконечно)
	float Lifetime;


	// последнее время обновления системы
	float TimeLastUpdate;



	CText *Next;
	CText *Prev;


};







//-----------------------------------------------------------------------------
// Менеджер
//-----------------------------------------------------------------------------

// Включаем в список
void	AttachText(CText * NewText);
// Исключаем из списка
void	DetachText(CText * OldText);
// Удаляем все объекты в списке
void	ReleaseAllText();
// Прорисовываем все объекты
void	DrawAllText();
//	Проверяем все объекты, обновляем данные
void	UpdateAllText(float Time);








#endif //TEXT_H
