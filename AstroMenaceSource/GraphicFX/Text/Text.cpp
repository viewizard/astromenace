/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Text.cpp

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


#include "Text.h"

void DrawFont(int X, int Y, float FlattenWidth, float MaxWidth, int CharSet, float Transp, const char *Text, ...);


//-----------------------------------------------------------------------------
// инициализация класса
//-----------------------------------------------------------------------------
CText::CText()
{
	TimeLastUpdate = -1.0f;
	Lifetime = -1.0f;
	DrawText = 0;
	Color = 0;

	PosX = PosY = 0;

	AttachText(this);

}


//-----------------------------------------------------------------------------
//	При разрушении класса
//-----------------------------------------------------------------------------
CText::~CText()
{
	if (DrawText != 0){delete [] DrawText; DrawText = 0;}
	DetachText(this);
}


//-----------------------------------------------------------------------------
// обновление
//-----------------------------------------------------------------------------
bool CText::Update(float Time)
{
	// первый раз... просто берем время
	if (TimeLastUpdate == -1.0f) {TimeLastUpdate = Time;return true;}

	// Time - это абсолютное время, вычисляем дельту
	float TimeDelta = Time - TimeLastUpdate;
	// быстро вызвали еще раз... время не изменилось, или почти не изменилось
	if (TimeDelta == 0.0f) return true;

	TimeLastUpdate = Time;


	// проверяем, сколько объекту жить, если нужно...-1.0f  - проверка не нужна
	if (Lifetime > -1.0f)
	{
		// считаем, сколько осталось жить
		Lifetime -= TimeDelta;
		// если уже ничего не осталось - его нужно уничтожить
		if (Lifetime <= 0.0f) return false;
	}


	if (DrawText == 0) return false;

    return true;
}





//-----------------------------------------------------------------------------
// прорисовка
//-----------------------------------------------------------------------------
void CText::Draw()
{
	if (DrawText != 0)
		DrawFont(PosX, PosY, 0, 0, Color, 1.0f, DrawText);
}









