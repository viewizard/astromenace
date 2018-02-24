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


#include "game_level_text.h"



//-----------------------------------------------------------------------------
// инициализация класса
//-----------------------------------------------------------------------------
CGameLvlText::CGameLvlText()
{
	AttachGameLvlText(this);

}


//-----------------------------------------------------------------------------
//	При разрушении класса
//-----------------------------------------------------------------------------
CGameLvlText::~CGameLvlText()
{
	if (DrawText != nullptr)
		delete [] DrawText;
	DetachGameLvlText(this);
}


//-----------------------------------------------------------------------------
// обновление
//-----------------------------------------------------------------------------
bool CGameLvlText::Update(float Time)
{
	// первый раз... просто берем время
	if (TimeLastUpdate == -1.0f) {
		TimeLastUpdate = Time;
		return true;
	}

	// Time - это абсолютное время, вычисляем дельту
	float TimeDelta = Time - TimeLastUpdate;
	// быстро вызвали еще раз... время не изменилось, или почти не изменилось
	if (TimeDelta == 0.0f) return true;

	TimeLastUpdate = Time;


	// проверяем, сколько объекту жить, если нужно...-1.0f  - проверка не нужна
	if (Lifetime > -1.0f) {
		// считаем, сколько осталось жить
		Lifetime -= TimeDelta;
		// если уже ничего не осталось - его нужно уничтожить
		if (Lifetime <= 0.0f) return false;
	}


	if (DrawText == nullptr)
		return false;

	return true;
}





//-----------------------------------------------------------------------------
// прорисовка
//-----------------------------------------------------------------------------
void CGameLvlText::Draw()
{
	if (DrawText != nullptr) {
		float R, G, B;

		switch (Color) {
		default: // белый
			R=1.0f;
			G=1.0f;
			B=1.0f;
			break;
		case 1: // желтый
			R=1.0f;
			G=1.0f;
			B=0.0f;
			break;
		case 2: // красный
			R=1.0f;
			G=0.0f;
			B=0.0f;
			break;
		case 3: // зеленый
			R=0.0f;
			G=1.0f;
			B=0.0f;
			break;
		case 4: // оранжевый
			R=1.0f;
			G=0.5f;
			B=0.0f;
			break;
		case 5: // серый
			R=0.5f;
			G=0.5f;
			B=0.5f;
			break;
		}

		vw_DrawFont(PosX, PosY, 0, 0, 1.0f, R,G,B, 1.0f, DrawText);
	}
}









