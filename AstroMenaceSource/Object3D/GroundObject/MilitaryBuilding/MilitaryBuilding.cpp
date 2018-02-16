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
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/


/// подключаем нужные файлы
#include "MilitaryBuilding.h"



struct MilitaryBuildingData
{
	float	Strength;
	int		WeaponQuantity;
	const	char *Name;
	const	char *TextureName;
};

static MilitaryBuildingData PresetMilitaryBuildingData[] =
{
	{80, 2,	"DATA/MODELS/MILITARYBUILDING/aa-gun-01.VW3D", "DATA/MODELS/gr-01.VW2D"},
	{120, 1,	"DATA/MODELS/MILITARYBUILDING/aa-gun-02.VW3D", "DATA/MODELS/gr-01.VW2D"},
	{80, 2,	"DATA/MODELS/MILITARYBUILDING/aa-gun-03.VW3D", "DATA/MODELS/gr-02.VW2D"},
	{150, 2,	"DATA/MODELS/MILITARYBUILDING/aa-gun-04.VW3D", "DATA/MODELS/gr-01.VW2D"},
	{250, 2, "DATA/MODELS/MILITARYBUILDING/aa-gun-05.VW3D", "DATA/MODELS/gr-03.VW2D"},
	{50, 1,	"DATA/MODELS/MILITARYBUILDING/artiler-gun-01.VW3D", "DATA/MODELS/gr-03.VW2D"},
	{200, 1,	"DATA/MODELS/MILITARYBUILDING/artiler-gun-02.VW3D", "DATA/MODELS/gr-01.VW2D"}
};
#define PresetMilitaryBuildingDataCount sizeof(PresetMilitaryBuildingData)/sizeof(PresetMilitaryBuildingData[0])



//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
void CMilitaryBuilding::Create(int	MilitaryBuildingNum)
{
	if ((MilitaryBuildingNum <= 0) || ((unsigned int)MilitaryBuildingNum > PresetMilitaryBuildingDataCount))
	{
		fprintf(stderr, "!!! Couldn't init CMilitaryBuilding object with Number %i.\n", MilitaryBuildingNum);
		return;
	}

	ObjectType = 6;
	ObjectCreationType = MilitaryBuildingNum;

	LoadObjectData(PresetMilitaryBuildingData[MilitaryBuildingNum-1].Name, this, 0, 2.0f);


	for (int i=0; i<DrawObjectQuantity; i++)
	{
		Texture[i] = vw_FindTextureByName(PresetMilitaryBuildingData[MilitaryBuildingNum-1].TextureName);
	}


	ResistanceHull = 1.0f;
	ResistanceSystems = 1.0f;


	Strength = StrengthStart = PresetMilitaryBuildingData[MilitaryBuildingNum-1].Strength/GameNPCArmorPenalty;


	WeaponQuantity = PresetMilitaryBuildingData[MilitaryBuildingNum-1].WeaponQuantity;
	// начальные установки для оружия
	WeaponSetFire = new bool[WeaponQuantity];
	WeaponLocation = new VECTOR3D[WeaponQuantity];
	Weapon = new CWeapon*[WeaponQuantity];
	WeaponBound = new VECTOR3D[WeaponQuantity];
	for (int i=0; i<WeaponQuantity; i++)
	{
		WeaponSetFire[i] = false;
		Weapon[i] = 0;
	}



	// установка доп. текстуры и других настроек для каждой модели
	switch (MilitaryBuildingNum)
	{
		case 1:
			WeaponLocation[0] = VECTOR3D(0.3f, 4.6f, 2.6f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(204);
			WeaponLocation[1] = VECTOR3D(-0.3f, 4.6f, 2.6f);
			Weapon[1] = new CWeapon;
			Weapon[1]->Create(204);
			WeaponFireType = 2;

			TargetHorizObjectQuantity = 4;
			TargetHorizObject = new int[TargetHorizObjectQuantity];
			TargetHorizObject[0] = 1;
			TargetHorizObject[1] = 2;
			TargetHorizObject[2] = 3;
			TargetHorizObject[3] = 4;

			TargetVertObjectQuantity = 3;
			TargetVertObject = new int[TargetVertObjectQuantity];
			TargetVertObject[0] = 2;
			TargetVertObject[1] = 3;
			TargetVertObject[2] = 4;
			TargetVertObjectMaxAngle = 60.0f;
			break;

		case 2:
			WeaponLocation[0] = VECTOR3D(0.0f, 4.6f, 4.5f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(204);
			Weapon[0]->NextFireTime = Weapon[0]->NextFireTime / 3.0f;
			WeaponFireType = 2;

			TargetHorizObjectQuantity = 4;
			TargetHorizObject = new int[TargetHorizObjectQuantity];
			TargetHorizObject[0] = 1;
			TargetHorizObject[1] = 2;
			TargetHorizObject[2] = 3;
			TargetHorizObject[3] = 4;

			TargetVertObjectQuantity = 3;
			TargetVertObject = new int[TargetVertObjectQuantity];
			TargetVertObject[0] = 2;
			TargetVertObject[1] = 3;
			TargetVertObject[2] = 4;
			TargetVertObjectMaxAngle = 60.0f;

			BarrelObjectQuantity = 1;
			BarrelObject = new int[BarrelObjectQuantity];
			BarrelObject[0] = 4;
			break;

		case 3:
			WeaponLocation[0] = VECTOR3D(2.8f, 5.3f, 7.5f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(205);
			Weapon[0]->NextFireTime = Weapon[0]->NextFireTime / 2.0f;
			WeaponLocation[1] = VECTOR3D(-2.8f, 5.3f, 7.5f);
			Weapon[1] = new CWeapon;
			Weapon[1]->Create(205);
			Weapon[1]->NextFireTime = Weapon[1]->NextFireTime / 2.0f;
			WeaponFireType = 3;

			TargetHorizObjectQuantity = 2;
			TargetHorizObject = new int[TargetHorizObjectQuantity];
			TargetHorizObject[0] = 1;
			TargetHorizObject[1] = 2;

			TargetVertObjectQuantity = 1;
			TargetVertObject = new int[TargetVertObjectQuantity];
			TargetVertObject[0] = 2;
			TargetVertObjectMaxAngle = 60.0f;
			break;

		case 4:
			WeaponLocation[0] = VECTOR3D(3.6f, 8.5f, 4.0f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(204);
			Weapon[0]->NextFireTime = Weapon[0]->NextFireTime / 2.0f;
			WeaponLocation[1] = VECTOR3D(-3.6f, 8.5f, 4.0f);
			Weapon[1] = new CWeapon;
			Weapon[1]->NextFireTime = Weapon[1]->NextFireTime / 2.0f;
			Weapon[1]->Create(204);
			WeaponFireType = 2;

			TargetHorizObjectQuantity = 4;
			TargetHorizObject = new int[TargetHorizObjectQuantity];
			TargetHorizObject[0] = 1;
			TargetHorizObject[1] = 2;
			TargetHorizObject[2] = 3;
			TargetHorizObject[3] = 4;

			TargetVertObjectQuantity = 3;
			TargetVertObject = new int[TargetVertObjectQuantity];
			TargetVertObject[0] = 2;
			TargetVertObject[1] = 3;
			TargetVertObject[2] = 4;
			TargetVertObjectMaxAngle = 60.0f;

			BarrelObjectQuantity = 2;
			BarrelObject = new int[BarrelObjectQuantity];
			BarrelObject[0] = 3;
			BarrelObject[1] = 4;
			break;

		case 5:
			WeaponLocation[0] = VECTOR3D(1.95f, 6.5f, 3.7f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(204);
			Weapon[0]->NextFireTime = Weapon[0]->NextFireTime / 3.0f;
			WeaponLocation[1] = VECTOR3D(-1.95f, 6.5f, 3.7f);
			Weapon[1] = new CWeapon;
			Weapon[1]->NextFireTime = Weapon[1]->NextFireTime / 3.0f;
			Weapon[1]->Create(204);
			WeaponFireType = 2;

			TargetHorizObjectQuantity = 2;
			TargetHorizObject = new int[TargetHorizObjectQuantity];
			TargetHorizObject[0] = 1;
			TargetHorizObject[1] = 2;

			TargetVertObjectQuantity = 1;
			TargetVertObject = new int[TargetVertObjectQuantity];
			TargetVertObject[0] = 2;
			TargetVertObjectMaxAngle = 60.0f;
			break;

		case 6:
			WeaponLocation[0] = VECTOR3D(0.0f, 3.0f, 7.0f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(204);
			Weapon[0]->NextFireTime = Weapon[0]->NextFireTime / 3.5f;

			TargetHorizObjectQuantity = 2;
			TargetHorizObject = new int[TargetHorizObjectQuantity];
			TargetHorizObject[0] = 2;
			TargetHorizObject[1] = 1;

			TargetVertObjectQuantity = 1;
			TargetVertObject = new int[TargetVertObjectQuantity];
			TargetVertObject[0] = 1;
			TargetVertObjectMaxAngle = 80.0f;
			break;

		case 7:
			WeaponLocation[0] = VECTOR3D(0.0f, 5.2f, 10.3f);
			Weapon[0] = new CWeapon;
			Weapon[0]->Create(211);
			Weapon[0]->NextFireTime = Weapon[0]->NextFireTime;

			TargetHorizObjectQuantity = 4;
			TargetHorizObject = new int[TargetHorizObjectQuantity];
			TargetHorizObject[0] = 4;
			TargetHorizObject[1] = 2;
			TargetHorizObject[2] = 3;
			TargetHorizObject[3] = 1;

			TargetVertObjectQuantity = 3;
			TargetVertObject = new int[TargetVertObjectQuantity];
			TargetVertObject[0] = 3;
			TargetVertObject[1] = 2;
			TargetVertObject[2] = 1;
			TargetVertObjectMaxAngle = 60.0f;

			BarrelObjectQuantity = 1;
			BarrelObject = new int[BarrelObjectQuantity];
			BarrelObject[0] = 1;
			break;

	}




	// вычисляем данные для нахождения точки стрельбы
	if (TargetHorizObject != 0)
	{
		BaseBound = DrawObjectList[TargetHorizObject[0]].Location;
	}

	if (TargetVertObject != 0)
	{
		if (TargetHorizObject != 0)
		{
			MiddleBound = DrawObjectList[TargetVertObject[0]].Location - DrawObjectList[TargetHorizObject[0]].Location;
		}
		else
		{
			MiddleBound = DrawObjectList[TargetVertObject[0]].Location;
		}
	}

	if (WeaponBound != 0)
	for (int i=0; i<WeaponQuantity; i++)
	{
		if (TargetVertObject != 0)
		{
			WeaponBound[i] = WeaponLocation[i] - DrawObjectList[TargetVertObject[0]].Location;
		}
		else
		if (TargetHorizObject != 0)
		{
			WeaponBound[i] = WeaponLocation[i] - DrawObjectList[TargetHorizObject[0]].Location;
		}
		else
		{
			WeaponBound[i] = WeaponLocation[i];
		}
	}



	// находим все данные по геометрии
	::CObject3D::InitByDrawObjectList();

}

