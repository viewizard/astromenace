/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2013 Michael Kurinnoy, Viewizard


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
		Texture[i] = vw_FindTextureByName("DATA/MODELS/SPACE/asteroid-01.tga");
	}



	//Speed = 25.0f;
	RotationSpeed.x = 100.0f + 50.0f*vw_Randf0;
	RotationSpeed.y = 20.0f + 20.0f*vw_Randf0;



	// небольшая поправка... в зависимости от размеров объекта
	Strength = StrengthStart = Radius*StrengthStart;

    // ставим правильный радиус
	Radius = Width/2.0f;
}

