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

#ifndef GAMELVLTEXT_H
#define GAMELVLTEXT_H

#include "../../core/core.h"


class cGameLvlText
{
public:
	cGameLvlText();
	~cGameLvlText();

	// обновить
	bool	Update(float Time);
	// прорисовка
	void	Draw();

	// собственно сам текст
	char	*DrawText{nullptr};
	// положение текста
	int	PosX{0};
	int	PosY{0};
	// цвет
	int	Color{0};

	// жизнь в секундах (-1.0 - бесконечно)
	float	Lifetime{-1.0f};

	// последнее время обновления системы
	float	TimeLastUpdate{-1.0f};

	cGameLvlText	*Next{nullptr};
	cGameLvlText	*Prev{nullptr};
};


//-----------------------------------------------------------------------------
// Менеджер
//-----------------------------------------------------------------------------

// Включаем в список
void	AttachGameLvlText(cGameLvlText * NewText);
// Исключаем из списка
void	DetachGameLvlText(cGameLvlText * OldText);
// Удаляем все объекты в списке
void	ReleaseAllGameLvlText();
// Прорисовываем все объекты
void	DrawAllGameLvlText();
//	Проверяем все объекты, обновляем данные
void	UpdateAllGameLvlText(float Time);

#endif //GAMELVLTEXT_H
