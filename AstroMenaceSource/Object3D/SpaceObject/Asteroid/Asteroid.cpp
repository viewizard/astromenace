/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Asteroid.cpp

	Copyright (c) 2006-2012 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.3

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


/// подключаем нужные файлы
#include "Asteroid.h"






//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
void CAsteroid::Create(int SpaceObjectNum)
{
	ObjectType = 7;
	ObjectCreationType = SpaceObjectNum;

	Strength = StrengthStart = 10.0f/GameNPCArmorPenalty;


	int Num = (int)(19*vw_Randf1);
	if (Num>19) Num=19;
	if (Num<0) Num=0;
	size_t SizeB = strlen("DATA/MODELS/SPACE/asteroid-01XX.VW3D")+1;
	char *NameTMP = new char[SizeB];
	char buffer[10];
	itoa10(Num, buffer);
	strcpy(NameTMP, "DATA/MODELS/SPACE/asteroid-01");
	strcat(NameTMP, buffer);
	strcat(NameTMP, ".VW3D");


	LoadObjectData(NameTMP, this, 0, -1.0f);
	// находим все данные по геометрии
	::CObject3D::InitByDrawObjectList();


	if (NameTMP != 0){delete [] NameTMP; NameTMP = 0;}

	// всегда только эти текстуры и 1 объект
	for (int i=0; i<DrawObjectQuantity; i++)
	{
		Texture[i] = vw_FindTextureByName("DATA/MODELS/SPACE/asteroid-01.jpg");
	}



	//Speed = 25.0f;
	RotationSpeed.x = 100.0f + 50.0f*vw_Randf0;
	RotationSpeed.y = 20.0f + 20.0f*vw_Randf0;



	// небольшая поправка... в зависимости от размеров объекта
	Strength = StrengthStart = Radius*StrengthStart;

    // ставим правильный радиус
	Radius = Width/2.0f;
}

