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


#ifndef GAMELVLTEXT_H
#define GAMELVLTEXT_H


#include "../../Core/Core.h"




class CGameLvlText
{
public:
	CGameLvlText();
	~CGameLvlText();

	// обновить
	bool Update(float Time);
	// прорисовка
	void Draw();

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



	CGameLvlText *Next;
	CGameLvlText *Prev;


};







//-----------------------------------------------------------------------------
// Менеджер
//-----------------------------------------------------------------------------

// Включаем в список
void	AttachGameLvlText(CGameLvlText * NewText);
// Исключаем из списка
void	DetachGameLvlText(CGameLvlText * OldText);
// Удаляем все объекты в списке
void	ReleaseAllGameLvlText();
// Прорисовываем все объекты
void	DrawAllGameLvlText();
//	Проверяем все объекты, обновляем данные
void	UpdateAllGameLvlText(float Time);








#endif //GAMELVLTEXT_H
