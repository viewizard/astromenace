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



#include "../Game.h"





//------------------------------------------------------------------------------------
// переменные
//------------------------------------------------------------------------------------
CEarthSpaceFighter *InfoFighter = 0;
CWeapon *InfoWeapon = 0;
CProjectile *InfoMine = 0;
CAlienSpaceFighter *InfoAlien = 0;
CAlienSpaceMotherShip *InfoAlienMotherShip = 0;
CPirateShip *InfoPirateShip = 0;
CBuilding *InfoBuilding = 0;
CMilitaryBuilding *InfoMilitaryBuilding = 0;
CWheeled *InfoWheeled = 0;
CTracked *InfoTracked = 0;


int GetWeaponHullDamage(int Num);
int GetWeaponSystemsDamage(int Num);
char *GetWeaponGroupTitle(int Num);
char *GetWeaponName(int Num);
char *GetShipGroupTitle(int Num);
char *GetWorkshopShipName(int Num);




float		LastRotateInfoObject = 0.0f;
VECTOR3D	PointCamera;
VECTOR3D	Point(1000,-1000,0);
VECTOR3D	ObjectBaseLocation(0,0,0);
int			CreateNum = 1;
float		RotationSumY =0.0f;
float		RotationSumX =0.0f;
float		InfoObjectWidth = 0.0f;
float		InfoObjectLength = 0.0f;
float		InfoObjectHeight = 0.0f;
float		InfoObjectStrength = 0.0f;
int			InfoObjectWeaponQuantity = 0;
int			InfoObjectEngineQuantity = 0;

float		InfoObjectEnergyUse = 0;
int			InfoObjectAmmo = 0;
float		InfoObjectReload = 0;
float		InfoObjectRange = 0;







// кол-во и положение в списке отображения
#define		InfoFighterStart  1
#define		InfoFighterQuant  22

#define		InfoWeaponStart  (InfoFighterStart+InfoFighterQuant)
#define		InfoWeaponQuant  19

#define		InfoMineStart  (InfoWeaponStart+InfoWeaponQuant)
#define		InfoMineQuant  4

#define		InfoAlienStart  (InfoMineStart+InfoMineQuant)
#define		InfoAlienQuant  17

#define		InfoAlienMotherShipStart  (InfoAlienStart+InfoAlienQuant)
#define		InfoAlienMotherShipQuant  8

#define		InfoPirateShipStart  (InfoAlienMotherShipStart+InfoAlienMotherShipQuant)
#define		InfoPirateShipQuant  15

#define		InfoBuildingStart  (InfoPirateShipStart+InfoPirateShipQuant)
#define		InfoBuildingQuant  11

#define		InfoMilitaryBuildingStart  (InfoBuildingStart+InfoBuildingQuant)
#define		InfoMilitaryBuildingQuant  7

#define		InfoWheeledStart  (InfoMilitaryBuildingStart+InfoMilitaryBuildingQuant)
#define		InfoWheeledQuant  8

#define		InfoTrackedStart  (InfoWheeledStart+InfoWheeledQuant)
#define		InfoTrackedQuant  14


#define		InfoEnd  125








const char *InfoGroupNames[10] =
{"4_Earth_Spaceships",
"4_Weapons",
"4_Space_Mines",
"4_Alien_Spaceships",
"4_Alien_Motherships",
"4_Pirate_Spaceships",
"4_Civilian_Buildings",
"4_Military_Buildings",
"4_Wheeled_Vehicles",
"4_Tracked_Vehicles"};



//------------------------------------------------------------------------------------
// Получаем номер текущего меню, и следующий элемент при нажатии
//------------------------------------------------------------------------------------
int GetInfoNextGroup()
{
	if (CreateNum>=InfoFighterStart && CreateNum<InfoFighterStart+InfoFighterQuant)
	{
		return 2;
	}
	if (CreateNum>=InfoWeaponStart && CreateNum<InfoWeaponStart+InfoWeaponQuant)
	{
		return 3;
	}
	if (CreateNum>=InfoMineStart && CreateNum<InfoMineStart+InfoMineQuant)
	{
		return 4;
	}
	if (CreateNum>=InfoAlienStart && CreateNum<InfoAlienStart+InfoAlienQuant)
	{
		return 5;
	}
	if (CreateNum>=InfoAlienMotherShipStart && CreateNum<InfoAlienMotherShipStart+InfoAlienMotherShipQuant)
	{
		return 6;
	}
	if (CreateNum>=InfoPirateShipStart && CreateNum<InfoPirateShipStart+InfoPirateShipQuant)
	{
		return 7;
	}
	if (CreateNum>=InfoBuildingStart && CreateNum<InfoBuildingStart+InfoBuildingQuant)
	{
		return 8;
	}
	if (CreateNum>=InfoMilitaryBuildingStart && CreateNum<InfoMilitaryBuildingStart+InfoMilitaryBuildingQuant)
	{
		return 9;
	}
	if (CreateNum>=InfoWheeledStart && CreateNum<InfoWheeledStart+InfoWheeledQuant)
	{
		return 10;
	}
	if (CreateNum>=InfoTrackedStart && CreateNum<InfoTrackedStart+InfoTrackedQuant)
	{
		return 1;
	}

	return 1;
}
int GetInfoPrevGroup()
{
	if (CreateNum>=InfoFighterStart && CreateNum<InfoFighterStart+InfoFighterQuant)
	{
		return 10;
	}
	if (CreateNum>=InfoWeaponStart && CreateNum<InfoWeaponStart+InfoWeaponQuant)
	{
		return 1;
	}
	if (CreateNum>=InfoMineStart && CreateNum<InfoMineStart+InfoMineQuant)
	{
		return 2;
	}
	if (CreateNum>=InfoAlienStart && CreateNum<InfoAlienStart+InfoAlienQuant)
	{
		return 3;
	}
	if (CreateNum>=InfoAlienMotherShipStart && CreateNum<InfoAlienMotherShipStart+InfoAlienMotherShipQuant)
	{
		return 4;
	}
	if (CreateNum>=InfoPirateShipStart && CreateNum<InfoPirateShipStart+InfoPirateShipQuant)
	{
		return 5;
	}
	if (CreateNum>=InfoBuildingStart && CreateNum<InfoBuildingStart+InfoBuildingQuant)
	{
		return 6;
	}
	if (CreateNum>=InfoMilitaryBuildingStart && CreateNum<InfoMilitaryBuildingStart+InfoMilitaryBuildingQuant)
	{
		return 7;
	}
	if (CreateNum>=InfoWheeledStart && CreateNum<InfoWheeledStart+InfoWheeledQuant)
	{
		return 8;
	}
	if (CreateNum>=InfoTrackedStart && CreateNum<InfoTrackedStart+InfoTrackedQuant)
	{
		return 9;
	}

	return 1;
}


int GetInfoSwitchToGroup(int Group)
{
	switch (Group)
	{
		case 1:
			return InfoFighterStart;
		case 2:
			return InfoWeaponStart;
		case 3:
			return InfoMineStart;
		case 4:
			return InfoAlienStart;
		case 5:
			return InfoAlienMotherShipStart;
		case 6:
			return InfoPirateShipStart;
		case 7:
			return InfoBuildingStart;
		case 8:
			return InfoMilitaryBuildingStart;
		case 9:
			return InfoWheeledStart;
		case 10:
			return InfoTrackedStart;
	}

	return 1;
}











//------------------------------------------------------------------------------------
// Удаление текущего объекта в меню
//------------------------------------------------------------------------------------
void DestroyInfoObject()
{
	if (InfoFighter != 0){delete InfoFighter; InfoFighter = 0;}
	if (InfoWeapon != 0){delete InfoWeapon; InfoWeapon = 0;}
	if (InfoMine != 0){delete InfoMine; InfoMine = 0;}
	if (InfoAlien != 0){delete InfoAlien; InfoAlien = 0;}
	if (InfoAlienMotherShip != 0){delete InfoAlienMotherShip; InfoAlienMotherShip = 0;}
	if (InfoPirateShip != 0){delete InfoPirateShip; InfoPirateShip = 0;}
	if (InfoBuilding != 0){delete InfoBuilding; InfoBuilding = 0;}
	if (InfoMilitaryBuilding != 0){delete InfoMilitaryBuilding; InfoMilitaryBuilding = 0;}
	if (InfoWheeled != 0){delete InfoWheeled; InfoWheeled = 0;}
	if (InfoTracked != 0){delete InfoTracked; InfoTracked = 0;}
}




//------------------------------------------------------------------------------------
// Создание текущего объекта
//------------------------------------------------------------------------------------
void CreateInfoObject()
{

	DestroyInfoObject();

	int TMPGameNPCArmorPenalty = GameNPCArmorPenalty;
	GameNPCArmorPenalty = 1;

	RotationSumY = 140;
	RotationSumX = 0;

	Point = VECTOR3D(1000,-1000,0);

	if (CreateNum>=InfoFighterStart && CreateNum<InfoFighterStart+InfoFighterQuant)
	{
		int tmpCreateNum = CreateNum-InfoFighterStart+1;
		InfoFighter = new CEarthSpaceFighter;
		InfoFighter->Create(tmpCreateNum);
		InfoFighter->ID = 1000;
		InfoFighter->EngineDestroyType = true;
		InfoFighter->SetLocation(VECTOR3D(1000,-1000-InfoFighter->AABB[6].y, 0));
		ObjectBaseLocation = InfoFighter->Location - VECTOR3D(1000,-1000,0);

		Point = VECTOR3D(1000,-1000+InfoFighter->Height/3.0f,0);

		PointCamera = VECTOR3D(0.0f,(InfoFighter->Length
			+InfoFighter->Width+
			InfoFighter->Height)*0.24f +InfoFighter->Height*0.3f,
			-(InfoFighter->Length
			+InfoFighter->Width+
			InfoFighter->Height)*0.56f-InfoFighter->Height*0.7f);
		InfoObjectWidth = InfoFighter->Width;
		InfoObjectLength = InfoFighter->Length;
		InfoObjectHeight = InfoFighter->Height;
		InfoObjectStrength = InfoFighter->StrengthStart;
		InfoObjectWeaponQuantity = InfoFighter->WeaponQuantity;
		InfoObjectEngineQuantity = InfoFighter->EngineQuantity;

		if (InfoFighter->Engine != 0)
		for (int i=0; i<InfoFighter->EngineQuantity; i++)
		if (InfoFighter->Engine[i] != 0)
			InfoFighter->Engine[i]->SpeedOnCreation = -1.0f;

		InfoFighter->SetRotation(VECTOR3D(RotationSumX,RotationSumY,0.0f));
	}
	if (CreateNum>=InfoWeaponStart && CreateNum<InfoWeaponStart+InfoWeaponQuant)
	{
		int tmpCreateNum = CreateNum-InfoWeaponStart+1;
		InfoWeapon = new CWeapon;
		InfoWeapon->Create(tmpCreateNum);
		InfoWeapon->ID = 1000;
		ObjectBaseLocation = VECTOR3D(0.0f,-InfoWeapon->AABB[6].y, -(InfoWeapon->Length/2.0f + InfoWeapon->AABB[6].z));

		Point = VECTOR3D(1000,-1000+InfoWeapon->Height/3.0f,0);

		PointCamera = VECTOR3D(0.0f,(InfoWeapon->Length
			+InfoWeapon->Width+
			InfoWeapon->Height)*0.3f +InfoWeapon->Height*0.3f,
			-(InfoWeapon->Length
			+InfoWeapon->Width+
			InfoWeapon->Height)*0.7f-InfoWeapon->Height*0.7f);
		InfoObjectWidth = InfoWeapon->Width;
		InfoObjectLength = InfoWeapon->Length;
		InfoObjectHeight = InfoWeapon->Height;
		InfoObjectStrength = InfoWeapon->StrengthStart;

		InfoWeapon->SetRotation(VECTOR3D(RotationSumX,RotationSumY,0.0f));

		InfoObjectEnergyUse = InfoWeapon->EnergyUse;
		InfoObjectAmmo = InfoWeapon->AmmoStart;
		InfoObjectReload = InfoWeapon->NextFireTime;
		InfoObjectRange = GetProjectileRange(tmpCreateNum);
		// убираем источник света
		if (InfoWeapon->Fire != 0)
		if (InfoWeapon->Fire->Light != 0){vw_ReleaseLight(InfoWeapon->Fire->Light); InfoWeapon->Fire->Light = 0;}
	}
	if (CreateNum>=InfoMineStart && CreateNum<InfoMineStart+InfoMineQuant)
	{
		int tmpCreateNum = CreateNum-InfoMineStart+1;
		InfoMine = new CProjectile;
		InfoMine->Create(213+tmpCreateNum);
		InfoMine->ID = 1000;
		InfoMine->ObjectStatus = -1; // чтобы не наводилось
		InfoMine->SpeedStart = InfoMine->SpeedEnd = InfoMine->Speed = 0.0f;
		InfoMine->GraphicFXDestroyType = true;
		InfoMine->SetLocation(VECTOR3D(1000,-1000-InfoMine->AABB[6].y, 0));
		ObjectBaseLocation = InfoMine->Location - VECTOR3D(1000,-1000,0);

		Point = VECTOR3D(1000,-1000+InfoMine->Height/2.0f,0);

		PointCamera = VECTOR3D(0.0f,(InfoMine->Length
			+InfoMine->Width+
			InfoMine->Height)*0.3f +InfoMine->Height*0.3f,
			-(InfoMine->Length
			+InfoMine->Width+
			InfoMine->Height)*0.7f-InfoMine->Height*0.7f);

		InfoObjectWidth = InfoMine->Width;
		InfoObjectLength = InfoMine->Length;
		InfoObjectHeight = InfoMine->Height;
		InfoObjectStrength = InfoMine->StrengthStart;

		InfoMine->SetRotation(VECTOR3D(RotationSumX,RotationSumY,0.0f));
	}
	if (CreateNum>=InfoAlienStart && CreateNum<InfoAlienStart+InfoAlienQuant)
	{
		int tmpCreateNum = CreateNum-InfoAlienStart+1;
		InfoAlien = new CAlienSpaceFighter;
		InfoAlien->Create(tmpCreateNum);
		InfoAlien->ID = 1000;
		InfoAlien->EngineDestroyType = true;
		InfoAlien->SetLocation(VECTOR3D(1000,-1000-InfoAlien->AABB[6].y, 0));
		ObjectBaseLocation = InfoAlien->Location - VECTOR3D(1000,-1000,0);

		Point = VECTOR3D(1000,-1000+InfoAlien->Height/3.0f,0);

		PointCamera = VECTOR3D(0.0f,(InfoAlien->Length
			+InfoAlien->Width+
			InfoAlien->Height)*0.24f +InfoAlien->Height*0.3f,
			-(InfoAlien->Length
			+InfoAlien->Width+
			InfoAlien->Height)*0.56f-InfoAlien->Height*0.7f);

		InfoObjectWidth = InfoAlien->Width;
		InfoObjectLength = InfoAlien->Length;
		InfoObjectHeight = InfoAlien->Height;
		InfoObjectStrength = InfoAlien->StrengthStart;

		if (InfoAlien->Engine != 0)
		for (int i=0; i<InfoAlien->EngineQuantity; i++)
		if (InfoAlien->Engine[i] != 0)
			InfoAlien->Engine[i]->SpeedOnCreation = -1.0f;

		InfoAlien->SetRotation(VECTOR3D(RotationSumX,RotationSumY,0.0f));
	}
	if (CreateNum>=InfoAlienMotherShipStart && CreateNum<InfoAlienMotherShipStart+InfoAlienMotherShipQuant)
	{
		int tmpCreateNum = CreateNum-InfoAlienMotherShipStart+1;
		InfoAlienMotherShip = new CAlienSpaceMotherShip;
		InfoAlienMotherShip->Create(tmpCreateNum);
		InfoAlienMotherShip->ID = 1000;
		InfoAlienMotherShip->EngineDestroyType = true;
		InfoAlienMotherShip->SetLocation(VECTOR3D(1000,-1000-InfoAlienMotherShip->AABB[6].y, 0));
		ObjectBaseLocation = InfoAlienMotherShip->Location - VECTOR3D(1000,-1000,0);

		Point = VECTOR3D(1000,-1000+InfoAlienMotherShip->Height/3.0f,0);

		PointCamera = VECTOR3D(0.0f,(InfoAlienMotherShip->Length
			+InfoAlienMotherShip->Width+
			InfoAlienMotherShip->Height)*0.24f +InfoAlienMotherShip->Height*0.3f,
			-(InfoAlienMotherShip->Length
			+InfoAlienMotherShip->Width+
			InfoAlienMotherShip->Height)*0.56f-InfoAlienMotherShip->Height*0.7f);
		InfoObjectWidth = InfoAlienMotherShip->Width;
		InfoObjectLength = InfoAlienMotherShip->Length;
		InfoObjectHeight = InfoAlienMotherShip->Height;
		InfoObjectStrength = InfoAlienMotherShip->StrengthStart;

		if (InfoAlienMotherShip->Engine != 0)
		for (int i=0; i<InfoAlienMotherShip->EngineQuantity; i++)
		if (InfoAlienMotherShip->Engine[i] != 0)
			InfoAlienMotherShip->Engine[i]->SpeedOnCreation = -1.0f;

		InfoAlienMotherShip->SetRotation(VECTOR3D(RotationSumX,RotationSumY,0.0f));
	}
	if (CreateNum>=InfoPirateShipStart && CreateNum<InfoPirateShipStart+InfoPirateShipQuant)
	{
		int tmpCreateNum = CreateNum-InfoPirateShipStart+1;
		InfoPirateShip = new CPirateShip;
		InfoPirateShip->Create(tmpCreateNum);
		InfoPirateShip->ID = 1000;
		InfoPirateShip->EngineDestroyType = true;
		InfoPirateShip->SetLocation(VECTOR3D(1000,-1000-InfoPirateShip->AABB[6].y, 0));
		ObjectBaseLocation = InfoPirateShip->Location - VECTOR3D(1000,-1000,0);

		Point = VECTOR3D(1000,-1000+InfoPirateShip->Height/3.0f,0);

		PointCamera = VECTOR3D(0.0f,(InfoPirateShip->Length
			+InfoPirateShip->Width+
			InfoPirateShip->Height)*0.24f +InfoPirateShip->Height*0.3f,
			-(InfoPirateShip->Length
			+InfoPirateShip->Width+
			InfoPirateShip->Height)*0.56f-InfoPirateShip->Height*0.7f);
		InfoObjectWidth = InfoPirateShip->Width;
		InfoObjectLength = InfoPirateShip->Length;
		InfoObjectHeight = InfoPirateShip->Height;
		InfoObjectStrength = InfoPirateShip->StrengthStart;

		if (InfoPirateShip->Engine != 0)
		for (int i=0; i<InfoPirateShip->EngineQuantity; i++)
		if (InfoPirateShip->Engine[i] != 0)
			InfoPirateShip->Engine[i]->SpeedOnCreation = -1.0f;

		// убираем поворот турелей
		if (InfoPirateShip->Weapon != 0)
		for (int i=0; i<InfoPirateShip->WeaponQuantity; i++)
		if (InfoPirateShip->Weapon[i] != 0)
		{
			InfoPirateShip->Weapon[i]->SetRotation(VECTOR3D(-InfoPirateShip->Weapon[i]->Rotation.x,-InfoPirateShip->Weapon[i]->Rotation.y,-InfoPirateShip->Weapon[i]->Rotation.z));
		}

		InfoPirateShip->SetRotation(VECTOR3D(RotationSumX,RotationSumY,0.0f));
	}
	if (CreateNum>=InfoBuildingStart && CreateNum<InfoBuildingStart+InfoBuildingQuant)
	{
		int tmpCreateNum = CreateNum-InfoBuildingStart+1;
		InfoBuilding = new CBuilding;
		InfoBuilding->Create(tmpCreateNum);
		InfoBuilding->ID = 1000;
		InfoBuilding->SetLocation(VECTOR3D(1000,-1000-InfoBuilding->AABB[6].y, 0));
		ObjectBaseLocation = InfoBuilding->Location - VECTOR3D(1000,-1000,0);

		Point = VECTOR3D(1000,-1000+InfoBuilding->Height/3.0f,0);

		PointCamera = VECTOR3D(0.0f,(InfoBuilding->Length
			+InfoBuilding->Width+
			InfoBuilding->Height)*0.16f +InfoBuilding->Height*0.3f,
			-(InfoBuilding->Length
			+InfoBuilding->Width+
			InfoBuilding->Height)*0.38f-InfoBuilding->Height*0.7f);
		InfoObjectWidth = InfoBuilding->Width;
		InfoObjectLength = InfoBuilding->Length;
		InfoObjectHeight = InfoBuilding->Height;
		InfoObjectStrength = InfoBuilding->StrengthStart;

		InfoBuilding->SetRotation(VECTOR3D(RotationSumX,RotationSumY,0.0f));
	}
	if (CreateNum>=InfoMilitaryBuildingStart && CreateNum<InfoMilitaryBuildingStart+InfoMilitaryBuildingQuant)
	{
		int tmpCreateNum = CreateNum-InfoMilitaryBuildingStart+1;
		InfoMilitaryBuilding = new CMilitaryBuilding;
		InfoMilitaryBuilding->Create(tmpCreateNum);
		InfoMilitaryBuilding->ID = 1000;
		InfoMilitaryBuilding->SetLocation(VECTOR3D(1000,-1000-InfoMilitaryBuilding->AABB[6].y, 0));
		ObjectBaseLocation = InfoMilitaryBuilding->Location - VECTOR3D(1000,-1000,0);

		Point = VECTOR3D(1000,-1000+InfoMilitaryBuilding->Height/3.0f,0);

		PointCamera = VECTOR3D(0.0f,(InfoMilitaryBuilding->Length
			+InfoMilitaryBuilding->Width+
			InfoMilitaryBuilding->Height)*0.24f +InfoMilitaryBuilding->Height*0.3f,
			-(InfoMilitaryBuilding->Length
			+InfoMilitaryBuilding->Width+
			InfoMilitaryBuilding->Height)*0.56f-InfoMilitaryBuilding->Height*0.7f);
		InfoObjectWidth = InfoMilitaryBuilding->Width;
		InfoObjectLength = InfoMilitaryBuilding->Length;
		InfoObjectHeight = InfoMilitaryBuilding->Height;
		InfoObjectStrength = InfoMilitaryBuilding->StrengthStart;

		InfoMilitaryBuilding->SetRotation(VECTOR3D(RotationSumX,RotationSumY,0.0f));
	}
	if (CreateNum>=InfoWheeledStart && CreateNum<InfoWheeledStart+InfoWheeledQuant)
	{
		int tmpCreateNum = CreateNum-InfoWheeledStart+1;
		InfoWheeled = new CWheeled;
		InfoWheeled->Create(tmpCreateNum);
		InfoWheeled->DeviationOn = true;
		InfoWheeled->ID = 1000;
		InfoWheeled->SetLocation(VECTOR3D(1000,-1000-InfoWheeled->AABB[6].y, 0));
		ObjectBaseLocation = InfoWheeled->Location - VECTOR3D(1000,-1000,0);
		InfoWheeled->WheelTrackSpeed = 350.0f;

		Point = VECTOR3D(1000,-1000+InfoWheeled->Height/3.0f,0);

		PointCamera = VECTOR3D(0.0f,(InfoWheeled->Length
			+InfoWheeled->Width+
			InfoWheeled->Height)*0.24f +InfoWheeled->Height*0.3f,
			-(InfoWheeled->Length
			+InfoWheeled->Width+
			InfoWheeled->Height)*0.56f-InfoWheeled->Height*0.7f);
		InfoObjectWidth = InfoWheeled->Width;
		InfoObjectLength = InfoWheeled->Length;
		InfoObjectHeight = InfoWheeled->Height;
		InfoObjectStrength = InfoWheeled->StrengthStart;

		InfoWheeled->SetRotation(VECTOR3D(RotationSumX,RotationSumY,0.0f));
	}
	if (CreateNum>=InfoTrackedStart && CreateNum<InfoTrackedStart+InfoTrackedQuant)
	{
		int tmpCreateNum = CreateNum-InfoTrackedStart+1;
		InfoTracked = new CTracked;
		InfoTracked->Create(tmpCreateNum);
		InfoTracked->DeviationOn = true;
		InfoTracked->ID = 1000;
		InfoTracked->SetLocation(VECTOR3D(1000,-1000-InfoTracked->AABB[6].y, 0));
		ObjectBaseLocation = InfoTracked->Location - VECTOR3D(1000,-1000,0);
		InfoTracked->WheelTrackSpeed = 350.0f;

		Point = VECTOR3D(1000,-1000+InfoTracked->Height/3.0f,0);

		PointCamera = VECTOR3D(0.0f,(InfoTracked->Length
			+InfoTracked->Width+
			InfoTracked->Height)*0.24f +InfoTracked->Height*0.3f,
			-(InfoTracked->Length
			+InfoTracked->Width+
			InfoTracked->Height)*0.56f-InfoTracked->Height*0.7f);
		InfoObjectWidth = InfoTracked->Width;
		InfoObjectLength = InfoTracked->Length;
		InfoObjectHeight = InfoTracked->Height;
		InfoObjectStrength = InfoTracked->StrengthStart;

		InfoTracked->SetRotation(VECTOR3D(RotationSumX,RotationSumY,0.0f));
	}

	GameNPCArmorPenalty = TMPGameNPCArmorPenalty;

	LastRotateInfoObject = vw_GetTime();
}






void InformationObject3DText(int ObjectNum)
{

	int X1 = Setup.iAspectRatioWidth/2 + 68;
	int Y1 = 50;
	int Offset = 30;
	int Size = 194;
	float WScale = -177;

	int SizeB = vw_FontSize(vw_GetText("3_GAME_OBJECTS_INFORMATION"));
	int SizeI = (Setup.iAspectRatioWidth-SizeB)/2;
	vw_DrawFont(SizeI, Y1-14, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("3_GAME_OBJECTS_INFORMATION"));



	// общее для файтеров землян
	if (CreateNum>=InfoFighterStart && CreateNum<InfoFighterStart+InfoFighterQuant)
	{
		Y1 += Offset+5;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Object_Class:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Space_Ship"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Ship_Type:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText(GetShipGroupTitle(ObjectNum-InfoFighterStart+1)));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Codename:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText(GetWorkshopShipName(ObjectNum-InfoFighterStart+1)));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Developer:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Earth_Federation"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Status:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Ally"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Armor:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i %s", (int)InfoObjectStrength, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Width:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp,  "%2.1f %s", InfoObjectWidth, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Length:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%2.1f %s", InfoObjectLength, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Height:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%2.1f %s", InfoObjectHeight, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, "%s:", vw_GetText("4_Weapons"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i %s", InfoObjectWeaponQuantity, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Engines:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i %s", InfoObjectEngineQuantity, vw_GetText("4_units"));
	}
	// оружие для кораблей землян
	if (CreateNum>=InfoWeaponStart && CreateNum<InfoWeaponStart+InfoWeaponQuant)
	{
		Y1 += Offset+5;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Object_Class:"));
		if (CreateNum<InfoWeaponStart+15)
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Weapon,_Cannon"));
		else
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Weapon,_Launcher"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Type:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText(GetWeaponGroupTitle(CreateNum-InfoWeaponStart+1)));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Codename:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText(GetWeaponName(CreateNum-InfoWeaponStart+1)));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Developer:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Earth_Federation"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Armor:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i %s", (int)InfoObjectStrength, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Damage,_Hull:"));
		if ((CreateNum-InfoWeaponStart+1 == 11) |
			(CreateNum-InfoWeaponStart+1 == 12) |
			(CreateNum-InfoWeaponStart+1 == 14))
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i %s", GetWeaponHullDamage(CreateNum-InfoWeaponStart+1), vw_GetText("4_units/sec"));
		else
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i %s", GetWeaponHullDamage(CreateNum-InfoWeaponStart+1), vw_GetText("4_units/shot"));

		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Damage,_Systems:"));
		if ((CreateNum-InfoWeaponStart+1 == 11) |
			(CreateNum-InfoWeaponStart+1 == 12) |
			(CreateNum-InfoWeaponStart+1 == 14))
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i %s", GetWeaponSystemsDamage(CreateNum-InfoWeaponStart+1), vw_GetText("4_units/sec"));
		else
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i %s", GetWeaponSystemsDamage(CreateNum-InfoWeaponStart+1), vw_GetText("4_units/shot"));

		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Energy_Use:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%3.1f %s", InfoObjectEnergyUse, vw_GetText("4_units_per_shot"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Ammo:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i %s", InfoObjectAmmo, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Reload:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%3.1f %s", InfoObjectReload, vw_GetText("4_seconds"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Range:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%3.1f %s", InfoObjectRange, vw_GetText("4_units"));

	}
	// мины
	if (CreateNum>=InfoMineStart && CreateNum<InfoMineStart+InfoMineQuant)
	{
		Y1 += Offset+5;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Object_Class:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Weapon"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Type:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Space_Mine"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Developer:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Pirates"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Armor:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i %s", (int)InfoObjectStrength, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Damage,_Hull:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "100 %s", vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Damage,_Systems:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "0 %s", vw_GetText("4_units"));
		Y1 += Offset;


		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, "%s:", vw_GetText("4_Targeting_System"));
		if (CreateNum == InfoMineStart)
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_No"));
		else
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Yes"));
		Y1 += Offset;

		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Auto_Cannon:"));
		if (CreateNum == InfoMineStart+2)
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Yes"));
		else
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_No"));
		Y1 += Offset;

		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Auto_Launcher:"));
		if (CreateNum == InfoMineStart+3)
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Yes"));
		else
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_No"));
		Y1 += Offset;

	}
	// общее для файтеров пришельцев
	if (CreateNum>=InfoAlienStart && CreateNum<InfoAlienStart+InfoAlienQuant)
	{
		Y1 += Offset+5;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Object_Class:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Space_Ship"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Ship_Type:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Unknown"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Developer:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Unknown"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Status:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Enemy"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Armor:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i %s", (int)InfoObjectStrength, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Width:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%2.1f %s", InfoObjectWidth, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Length:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%2.1f %s", InfoObjectLength, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Height:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%2.1f %s", InfoObjectHeight, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, "%s:", vw_GetText("4_Weapons"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Unknown"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Damage:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 10, 200, vw_GetText("4_units"));
	}
	// общее для MotherShip пришельцев
	if (CreateNum>=InfoAlienMotherShipStart && CreateNum<InfoAlienMotherShipStart+InfoAlienMotherShipQuant)
	{
		Y1 += Offset+5;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Object_Class:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_MotherShip"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Ship_Type:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Unknown"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Developer:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Unknown"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Status:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Enemy"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Armor:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i %s", (int)InfoObjectStrength, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Width:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%2.1f %s", InfoObjectWidth, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Length:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%2.1f %s", InfoObjectLength, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Height:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%2.1f %s", InfoObjectHeight, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, "%s:", vw_GetText("4_Weapons"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Unknown"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Damage:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 50, 500, vw_GetText("4_units"));
	}
	// общее для пиратов
	if (CreateNum>=InfoPirateShipStart && CreateNum<InfoPirateShipStart+InfoPirateShipQuant)
	{
		Y1 += Offset+5;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Object_Class:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Pirate_Ship"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Ship_Type:"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Developer:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Pirates"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Status:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Enemy"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Armor:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i %s", (int)InfoObjectStrength, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Width:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%2.1f %s", InfoObjectWidth, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Length:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%2.1f %s", InfoObjectLength, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Height:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%2.1f %s", InfoObjectHeight, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, "%s:", vw_GetText("4_Weapons"));
	//	vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%s, %s", vw_GetText("4_Kinetic"), vw_GetText("4_Propelled"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Damage:"));
	//	vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 10, 200, vw_GetText("4_units"));
	}
	// общее для зданий
	if (CreateNum>=InfoBuildingStart && CreateNum<InfoBuildingStart+InfoBuildingQuant)
	{
		Y1 += Offset+5;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Object_Class:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Civilian_Building"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Developer:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Old_Republic"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Status:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Enemy"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Armor:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Indestructible"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Width:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%2.1f %s", InfoObjectWidth, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Length:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%2.1f %s", InfoObjectLength, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Height:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%2.1f %s", InfoObjectHeight, vw_GetText("4_units"));
	}
	// общее для наземных войск
	if (CreateNum>=InfoMilitaryBuildingStart && CreateNum<InfoMilitaryBuildingStart+InfoMilitaryBuildingQuant)
	{
		Y1 += Offset+5;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Object_Class:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Military_Building"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Type:"));
	//	vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "Civilian Building");
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Developer:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Pirates"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Status:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Enemy"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Armor:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i %s", (int)InfoObjectStrength, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Width:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%2.1f %s", InfoObjectWidth, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Length:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%2.1f %s", InfoObjectLength, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Height:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%2.1f %s", InfoObjectHeight, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, "%s:", vw_GetText("4_Weapons"));
	//	vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "Kinetic");
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Damage:"));
	//	vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 10, 500, vw_GetText("4_units"));
	}
	// общее для колесного
	if (CreateNum>=InfoWheeledStart && CreateNum<InfoWheeledStart+InfoWheeledQuant)
	{
		Y1 += Offset+5;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Object_Class:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Wheeled_Vehicle"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Type:"));
	//	vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "Civilian Building");
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Developer:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Pirates"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Status:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Enemy"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Armor:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i %s", (int)InfoObjectStrength, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Width:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%2.1f %s", InfoObjectWidth, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Length:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%2.1f %s", InfoObjectLength, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Height:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%2.1f %s", InfoObjectHeight, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, "%s:", vw_GetText("4_Weapons"));
	//	vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "Kinetic");
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Damage:"));
	//	vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 10, 300, vw_GetText("4_units"));
	}
	// общее для гусенечного
	if (CreateNum>=InfoTrackedStart && CreateNum<InfoTrackedStart+InfoTrackedQuant)
	{
		Y1 += Offset+5;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Object_Class:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Tracked_Vehicle"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Type:"));
	//	vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "Civilian Building");
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Developer:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Pirates"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Status:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Enemy"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Armor:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i %s", (int)InfoObjectStrength, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Width:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%2.1f %s", InfoObjectWidth, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Length:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%2.1f %s", InfoObjectLength, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Height:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%2.1f %s", InfoObjectHeight, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, "%s:", vw_GetText("4_Weapons"));
	//	vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "Kinetic");
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("4_Damage:"));
	//	vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 10, 500, vw_GetText("4_units"));
	}



	Y1 = 50;//+Offset;



	switch (ObjectNum)
	{


	// для пиратов
		case InfoPirateShipStart:
		case InfoPirateShipStart+1:
		case InfoPirateShipStart+2:
		case InfoPirateShipStart+3:
			Y1 += Offset+Offset+5;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Marauder"));
			Y1 += Offset*7;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%s, %s", vw_GetText("4_Kinetic"), vw_GetText("4_Propelled"));
			Y1 += Offset;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 10, 100, vw_GetText("4_units"));
			break;
		case InfoPirateShipStart+4:
			Y1 += Offset+Offset+5;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Warder"));
			Y1 += Offset*7;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%s, %s", vw_GetText("4_Kinetic"), vw_GetText("4_Propelled"));
			Y1 += Offset;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 10, 200, vw_GetText("4_units"));
			break;
		case InfoPirateShipStart+5:
			Y1 += Offset+Offset+5;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Raider"));
			Y1 += Offset*7;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%s, %s", vw_GetText("4_Kinetic"), vw_GetText("4_Propelled"));
			Y1 += Offset;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 10, 300, vw_GetText("4_units"));
			break;
		case InfoPirateShipStart+6:
		case InfoPirateShipStart+7:
			Y1 += Offset+Offset+5;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Missile_Carrier"));
			Y1 += Offset*7;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%s, %s", vw_GetText("4_Kinetic"), vw_GetText("4_Propelled"));
			Y1 += Offset;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 10, 500, vw_GetText("4_units"));
			break;
		case InfoPirateShipStart+8:
		case InfoPirateShipStart+9:
			Y1 += Offset+Offset+5;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Battle_Freighter"));
			Y1 += Offset*7;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%s, %s", vw_GetText("4_Kinetic"), vw_GetText("4_Propelled"));
			Y1 += Offset;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 10, 300, vw_GetText("4_units"));
			break;
		case InfoPirateShipStart+10:
		case InfoPirateShipStart+11:
		case InfoPirateShipStart+12:
			Y1 += Offset+Offset+5;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Missile_Carrier"));
			Y1 += Offset*7;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Propelled"));
			Y1 += Offset;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 50, 300, vw_GetText("4_units"));
			break;
		case InfoPirateShipStart+13:
		case InfoPirateShipStart+14:
			Y1 += Offset+Offset+5;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Battle_Freighter"));
			Y1 += Offset*7;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Propelled"));
			Y1 += Offset;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 50, 200, vw_GetText("4_units"));
			break;





	// для наземных войск
		case InfoMilitaryBuildingStart:
		case InfoMilitaryBuildingStart+5:
			Y1 += Offset+Offset+5;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Cannon"));
			Y1 += Offset*7;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Kinetic"));
			Y1 += Offset;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 10, 30, vw_GetText("4_units"));
			break;
		case InfoMilitaryBuildingStart+3:
			Y1 += Offset+Offset+5;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_MachineGun"));
			Y1 += Offset*7;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Kinetic"));
			Y1 += Offset;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 10, 50, vw_GetText("4_units"));
			break;
		case InfoMilitaryBuildingStart+1:
		case InfoMilitaryBuildingStart+6:
			Y1 += Offset+Offset+5;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Turbo_Cannon"));
			Y1 += Offset*7;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Kinetic"));
			Y1 += Offset;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 10, 100, vw_GetText("4_units"));
			break;
		case InfoMilitaryBuildingStart+2:
			Y1 += Offset+Offset+5;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Missile_Launcher"));
			Y1 += Offset*7;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Propelled"));
			Y1 += Offset;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 10, 200, vw_GetText("4_units"));
			break;
		case InfoMilitaryBuildingStart+4:
			Y1 += Offset+Offset+5;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Cannon_Turret"));
			Y1 += Offset*7;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Kinetic"));
			Y1 += Offset;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 10, 100, vw_GetText("4_units"));
			break;


	// для колесного транспорта
		case InfoWheeledStart:
			Y1 += Offset+Offset+5;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Armored_Jeep"));
			Y1 += Offset*7;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Propelled"));
			Y1 += Offset;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 10, 40, vw_GetText("4_units"));
			break;
		case InfoWheeledStart+1:
			Y1 += Offset+Offset+5;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Jeep"));
			Y1 += Offset*7;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Propelled"));
			Y1 += Offset;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 10, 100, vw_GetText("4_units"));
			break;
		case InfoWheeledStart+2:
			Y1 += Offset+Offset+5;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Armored_Jeep"));
			Y1 += Offset*7;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Kinetic"));
			Y1 += Offset;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 10, 40, vw_GetText("4_units"));
			break;
		case InfoWheeledStart+3:
			Y1 += Offset+Offset+5;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Jeep"));
			Y1 += Offset*7;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Ion"));
			Y1 += Offset;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 10, 50, vw_GetText("4_units"));
			break;
		case InfoWheeledStart+4:
			Y1 += Offset+Offset+5;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Armored_Jeep"));
			Y1 += Offset*7;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Antimatter"));
			Y1 += Offset;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 10, 100, vw_GetText("4_units"));
			break;
		case InfoWheeledStart+5:
		case InfoWheeledStart+6:
			Y1 += Offset+Offset+5;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_APC"));
			Y1 += Offset*7;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Kinetic"));
			Y1 += Offset;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 10, 40, vw_GetText("4_units"));
			break;
		case InfoWheeledStart+7:
			Y1 += Offset+Offset+5;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Missile_Launcher"));
			Y1 += Offset*7;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Propelled"));
			Y1 += Offset;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 10, 400, vw_GetText("4_units"));
			break;


	// для гусенечного транспорта
		case InfoTrackedStart:
		case InfoTrackedStart+1:
		case InfoTrackedStart+5:
		case InfoTrackedStart+6:
		case InfoTrackedStart+8:
			Y1 += Offset+Offset+5;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Tank"));
			Y1 += Offset*7;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Kinetic"));
			Y1 += Offset;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 10, 50, vw_GetText("4_units"));
			break;
		case InfoTrackedStart+2:
			Y1 += Offset+Offset+5;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Tank"));
			Y1 += Offset*7;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Plasma"));
			Y1 += Offset;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 10, 100, vw_GetText("4_units"));
			break;
		case InfoTrackedStart+3:
		case InfoTrackedStart+4:
		case InfoTrackedStart+7:
			Y1 += Offset+Offset+5;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Tank"));
			Y1 += Offset*7;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Antimatter"));
			Y1 += Offset;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 10, 200, vw_GetText("4_units"));
			break;
		case InfoTrackedStart+9:
			Y1 += Offset+Offset+5;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_APC"));
			Y1 += Offset*7;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Kinetic"));
			Y1 += Offset;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 10, 50, vw_GetText("4_units"));
			break;
		case InfoTrackedStart+10:
		case InfoTrackedStart+11:
		case InfoTrackedStart+12:
			Y1 += Offset+Offset+5;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Missile_Launcher"));
			Y1 += Offset*7;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Propelled"));
			Y1 += Offset;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i - %i %s", 10, 400, vw_GetText("4_units"));
			break;
		case InfoTrackedStart+13:
			Y1 += Offset+Offset+5;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Engineer"));
			Y1 += Offset*7;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Not_combatant"));
			Y1 += Offset;
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i %s", 0, vw_GetText("4_units"));
			break;


	}





	Y1 = 50+Offset*13;
	float W = 1024.0f-75*2;
	X1 = Setup.iAspectRatioWidth/2 - 437;
	// вывод краткого описания
	switch (ObjectNum)
	{
		case InfoFighterStart:
		case InfoFighterStart+3:
		case InfoFighterStart+5:
		case InfoFighterStart+9:
		case InfoFighterStart+11:
		case InfoFighterStart+13:
		case InfoFighterStart+14:
		case InfoFighterStart+16:
		case InfoFighterStart+19:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_FighterLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_FighterLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_FighterLine3"));
			break;

  		case InfoFighterStart+1:
		case InfoFighterStart+4:
		case InfoFighterStart+18:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_InterceptorLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_InterceptorLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_InterceptorLine3"));
			break;

  		case InfoFighterStart+2:
		case InfoFighterStart+15:
		case InfoFighterStart+17:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_BomberLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_BomberLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_BomberLine3"));
			break;

		case InfoFighterStart+6:
		case InfoFighterStart+12:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_AttackShipLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_AttackShipLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_AttackShipLine3"));
			break;

  		case InfoFighterStart+7:
		case InfoFighterStart+8:
		case InfoFighterStart+10:
		case InfoFighterStart+20:
		case InfoFighterStart+21:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_ScoutLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_ScoutLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_ScoutLine3"));
			break;





  		case InfoWeaponStart:
		case InfoWeaponStart+1:
		case InfoWeaponStart+2:
		case InfoWeaponStart+3:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_KineticLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_KineticLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_KineticLine3"));
			break;

		case InfoWeaponStart+4:
		case InfoWeaponStart+5:
		case InfoWeaponStart+6:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_IonLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_IonLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_IonLine3"));
			break;

		case InfoWeaponStart+7:
		case InfoWeaponStart+8:
		case InfoWeaponStart+9:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_PlasmaLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_PlasmaLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_PlasmaLine3"));
			break;

		case InfoWeaponStart+10:
		case InfoWeaponStart+11:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_MaserLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_MaserLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_MaserLine3"));
			break;

		case InfoWeaponStart+12:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_AntimatterLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_AntimatterLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_AntimatterLine3"));
			break;

		case InfoWeaponStart+13:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_LaserLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_LaserLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_LaserLine3"));
			break;

		case InfoWeaponStart+14:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_GaussLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_GaussLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_GaussLine3"));
			break;

		case InfoWeaponStart+15:
		case InfoWeaponStart+16:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_MissileLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_MissileLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_MissileLine3"));
			break;

		case InfoWeaponStart+17:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_TorpedoLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_TorpedoLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_TorpedoLine3"));
			break;

		case InfoWeaponStart+18:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_NukeBombLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_NukeBombLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_NukeBombLine3"));
			break;


		case InfoMineStart:
		case InfoMineStart+1:
		case InfoMineStart+2:
		case InfoMineStart+3:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_MineLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_MineLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_MineLine3"));
			break;


		case InfoAlienStart:
		case InfoAlienStart+1:
		case InfoAlienStart+2:
		case InfoAlienStart+3:
		case InfoAlienStart+4:
		case InfoAlienStart+5:
		case InfoAlienStart+6:
		case InfoAlienStart+7:
		case InfoAlienStart+8:
		case InfoAlienStart+9:
		case InfoAlienStart+10:
		case InfoAlienStart+11:
		case InfoAlienStart+12:
		case InfoAlienStart+13:
		case InfoAlienStart+14:
		case InfoAlienStart+15:
		case InfoAlienStart+16:
		case InfoAlienMotherShipStart:
		case InfoAlienMotherShipStart+1:
		case InfoAlienMotherShipStart+2:
		case InfoAlienMotherShipStart+3:
		case InfoAlienMotherShipStart+4:
		case InfoAlienMotherShipStart+5:
		case InfoAlienMotherShipStart+6:
		case InfoAlienMotherShipStart+7:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_UnknownLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_UnknownLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_UnknownLine3"));
			break;





		case InfoPirateShipStart:
		case InfoPirateShipStart+1:
		case InfoPirateShipStart+2:
		case InfoPirateShipStart+3:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_MarauderLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_MarauderLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_MarauderLine3"));
			break;
		case InfoPirateShipStart+4:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_WarderLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_WarderLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_WarderLine3"));
			break;
		case InfoPirateShipStart+5:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_RaiderLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_RaiderLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_RaiderLine3"));
			break;
		case InfoPirateShipStart+6:
		case InfoPirateShipStart+7:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_MissileCarrierLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_MissileCarrierLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_MissileCarrierLine3"));
			break;
		case InfoPirateShipStart+8:
		case InfoPirateShipStart+9:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_BattleFreighterLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_BattleFreighterLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_BattleFreighterLine3"));
			break;
		case InfoPirateShipStart+10:
		case InfoPirateShipStart+11:
		case InfoPirateShipStart+12:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_MissileCarrierLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_MissileCarrierLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_MissileCarrierLine3"));
			break;
		case InfoPirateShipStart+13:
		case InfoPirateShipStart+14:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_BattleFreighterLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_BattleFreighterLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_BattleFreighterLine3"));
			break;





		case InfoBuildingStart:
		case InfoBuildingStart+1:
		case InfoBuildingStart+2:
		case InfoBuildingStart+3:
		case InfoBuildingStart+4:
		case InfoBuildingStart+5:
		case InfoBuildingStart+6:
		case InfoBuildingStart+7:
		case InfoBuildingStart+8:
		case InfoBuildingStart+9:
		case InfoBuildingStart+10:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_CivilianBuildingLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_CivilianBuildingLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_CivilianBuildingLine3"));
			break;

		case InfoMilitaryBuildingStart:
		case InfoMilitaryBuildingStart+1:
		case InfoMilitaryBuildingStart+2:
		case InfoMilitaryBuildingStart+3:
		case InfoMilitaryBuildingStart+4:
		case InfoMilitaryBuildingStart+5:
		case InfoMilitaryBuildingStart+6:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_WeaponturretLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_WeaponturretLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_WeaponturretLine3"));
			break;

		case InfoWheeledStart:
		case InfoWheeledStart+2:
		case InfoWheeledStart+4:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_ArmoredJeepLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_ArmoredJeepLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_ArmoredJeepLine3"));
			break;

		case InfoWheeledStart+1:
		case InfoWheeledStart+3:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_JeepLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_JeepLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_JeepLine3"));
			break;

		case InfoTrackedStart:
		case InfoTrackedStart+1:
		case InfoTrackedStart+2:
		case InfoTrackedStart+3:
		case InfoTrackedStart+4:
		case InfoTrackedStart+5:
		case InfoTrackedStart+6:
		case InfoTrackedStart+7:
		case InfoTrackedStart+8:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_TankLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_TankLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_TankLine3"));
			break;

		case InfoWheeledStart+5:
		case InfoWheeledStart+6:
		case InfoTrackedStart+9:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_APCLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_APCLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_APCLine3"));
			break;

		case InfoWheeledStart+7:
		case InfoTrackedStart+10:
		case InfoTrackedStart+11:
		case InfoTrackedStart+12:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_MissileLauncherLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_MissileLauncherLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_MissileLauncherLine3"));
			break;

		case InfoTrackedStart+13:
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_EngineerLine1")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_EngineerLine2")); Y1 += Offset;
			vw_DrawFont(X1, Y1, W*(-1.0f), 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("5_EngineerLine3"));
			break;


	}

}











//------------------------------------------------------------------------------------
// Основная процедура прорисовки
//------------------------------------------------------------------------------------
void InformationMenu()
{

	RECT SrcRect, DstRect;
	SetRect(&SrcRect,0,0,2,2);
	SetRect(&DstRect,0,0,Setup.iAspectRatioWidth,768);
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/blackpoint.tga"), true, 0.5f*MenuContentTransp);


	SetRect(&SrcRect,2,2,464-2,353-2);
	SetRect(&DstRect,(Setup.iAspectRatioWidth/2-432)-8,80-8,(Setup.iAspectRatioWidth/2-432)-8+464-4,80-8+353-4);
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/panel444_333_back.tga"), true, 0.9f*MenuContentTransp);



	// выводим кол-во и текущую страницу
	int	Size = vw_FontSize(vw_GetText("3_Page_%i_of_%i"), CreateNum, InfoEnd);
	int SizeI = (Setup.iAspectRatioWidth-Size)/2;
	vw_DrawFont(SizeI, 50+30*16+15, 0, 0, 1.0f, 0.5f,0.5f,0.5f, 0.6f*MenuContentTransp, vw_GetText("3_Page_%i_of_%i"), CreateNum, InfoEnd);


//	DrawFont(760.0f, 660.0f, 0, 0, 1, 0.2f*MenuContentTransp, "PgUp - Previous Page");
//	DrawFont(760.0f, 680.0f, 0, 0, 1, 0.2f*MenuContentTransp, "PgDown - Next Page");



//PgUp – Previous Page
//PgDown – Next Page

	InformationObject3DText(CreateNum);



	// проверяем колесо мышки
	SetRect(&DstRect,(int)(Setup.iAspectRatioWidth/2-440),80,(int)(Setup.iAspectRatioWidth/2+440),590);
	if (vw_OnRect(&DstRect))
	{
		if (vw_GetWheelStatus() != 0 && !isDialogBoxDrawing())
		{
			CreateNum += vw_GetWheelStatus();

			if (CreateNum<1) CreateNum = InfoEnd;
			if (CreateNum>InfoEnd) CreateNum = 1;
			CreateInfoObject();

			vw_ResetWheelStatus();
		}
	}
	else if (vw_GetWheelStatus() != 0)
	{
		vw_ResetWheelStatus();
	}



	int Prir = 100;
	int X = Setup.iAspectRatioWidth/2 - 432;
	int Y = 165+Prir*4;
	if (DrawButton200_2(X,Y+28, vw_GetText(InfoGroupNames[GetInfoPrevGroup()-1]), MenuContentTransp, false))
	{
		CreateNum = GetInfoSwitchToGroup(GetInfoPrevGroup());
		CreateInfoObject();
	}


	X = Setup.iAspectRatioWidth/2 - 209;
	if (DrawButton200_2(X,Y+28, vw_GetText("1_Page_Up"), MenuContentTransp, false))
	{
		CreateNum --;
		if (CreateNum<1) CreateNum = InfoEnd;
		CreateInfoObject();
	}
	if (vw_GetKeys(SDLK_PAGEUP))
	{
		CreateNum --;
		if (CreateNum<1) CreateNum = InfoEnd;
		CreateInfoObject();
		vw_SetKeys(SDLK_PAGEUP, false);
	}

	X = Setup.iAspectRatioWidth/2 + 9;
	if (DrawButton200_2(X,Y+28, vw_GetText("1_Page_Down"), MenuContentTransp, false))
	{
		CreateNum ++;
		if (CreateNum>InfoEnd) CreateNum = 1;
		CreateInfoObject();
	}
	if (vw_GetKeys(SDLK_PAGEDOWN))
	{
		CreateNum ++;
		if (CreateNum>InfoEnd) CreateNum = 1;
		CreateInfoObject();
		vw_SetKeys(SDLK_PAGEDOWN, false);
	}


	X = Setup.iAspectRatioWidth/2 + 432 - 200;
	if (DrawButton200_2(X,Y+28, vw_GetText(InfoGroupNames[GetInfoNextGroup()-1]), MenuContentTransp, false))
	{
		CreateNum = GetInfoSwitchToGroup(GetInfoNextGroup());
		CreateInfoObject();
	}


	X = (Setup.iAspectRatioWidth - 384)/2;
	Y = Y+Prir;
	if (DrawButton384(X,Y, vw_GetText("1_MAIN_MENU"), MenuContentTransp, &Button1Transp, &LastButton1UpdateTime))
	{
		DestroyInfoObject();
		ComBuffer = MAIN_MENU;
	}

}













//------------------------------------------------------------------------------------
// Прорисовка 3д части
//------------------------------------------------------------------------------------
void InformationDrawObject()
{

	int MouseX, MouseY;
	vw_GetMousePos(&MouseX, &MouseY);

	int x, y, width, height;
	vw_GetViewport(&x, &y, &width, &height);


	float AWw = width*1.0f;
	float AHw = height*1.0f;
	float AW;
	float AH;
	vw_GetAspectWH(&AW, &AH);


	vw_SetViewport((int)((Setup.iAspectRatioWidth/2-432)/(AW/AWw)), (int)(80/(AH/AHw)), (int)(444/(AW/AWw)), (int)(333/(AH/AHw)), 0.0f, 1.0f, RI_UL_CORNER);
	vw_ResizeScene(45.0f, 444.0f/333.0f, 1.0f, 2000.0f);
	vw_Clear(RI_DEPTH_BUFFER);


	vw_LoadIdentity();
	vw_SetCameraLocation(VECTOR3D(1000+PointCamera.x,-1000+PointCamera.y,PointCamera.z));
	vw_SetCameraMoveAroundPoint(Point, 0.0f, VECTOR3D(0.0f, 0.0f, 0.0f));
	vw_CameraLookAt();






	// вращение объекта

	float RotateInfoObjectY = 15.0f*(vw_GetTime() - LastRotateInfoObject);
	float tmpRotateInfoObjectX = 15.0f*(vw_GetTime() - LastRotateInfoObject);
	float RotateInfoObjectX = 0;
	LastRotateInfoObject = vw_GetTime();

	RECT DstRectLeft, DstRectRight, DstRectUp, DstRectDown;
	float fLeft, fRight, fUp, fDown;
	fLeft = fRight = fUp = fDown = 0.15f;


	SetRect(&DstRectLeft,(Setup.iAspectRatioWidth/2-432)+10,
					80+(333-32)/2,
					(Setup.iAspectRatioWidth/2-432)+32+10,
					80+(333+32)/2);
	SetRect(&DstRectRight,(Setup.iAspectRatioWidth/2-432)+444-32-10,
					80+(333-32)/2,
					(Setup.iAspectRatioWidth/2-432)+444-10,
					80+(333+32)/2);
	SetRect(&DstRectUp,(Setup.iAspectRatioWidth/2-432)+(444-32)/2,
					80+333-32-10,
					(Setup.iAspectRatioWidth/2-432)+(444+32)/2,
					80+333-10);
	SetRect(&DstRectDown,(Setup.iAspectRatioWidth/2-432)+(444-32)/2,
					80+10,
					(Setup.iAspectRatioWidth/2-432)+(444+32)/2,
					80+32+10);

	// для вращения объекта, только если мышка стоит над выводом 3д модели
	RECT DstRect;
	SetRect(&DstRect,(Setup.iAspectRatioWidth/2-432),
					80,
					(Setup.iAspectRatioWidth/2-432)+444,
					80+333);
	if  (((DstRect.right  >= MouseX) &&
			(DstRect.left<= MouseX) &&
			(DstRect.bottom >= MouseY) &&
			(DstRect.top<= MouseY)) && !isDialogBoxDrawing())
	{
		fLeft = fRight = fUp = fDown = 0.35f;

		if  (((DstRectLeft.right  >= MouseX) &&
				(DstRectLeft.left<= MouseX) &&
				(DstRectLeft.bottom >= MouseY) &&
				(DstRectLeft.top<= MouseY)) && !isDialogBoxDrawing())
		{
			fLeft = 0.8f;
			RotateInfoObjectY = RotateInfoObjectY*3;
		}
		else
		if  (((DstRectRight.right  >= MouseX) &&
				(DstRectRight.left<= MouseX) &&
				(DstRectRight.bottom >= MouseY) &&
				(DstRectRight.top<= MouseY)) && !isDialogBoxDrawing())
		{
			fRight = 0.8f;
			RotateInfoObjectY = -RotateInfoObjectY*3;
		}
		else
		if  (((DstRectUp.right  >= MouseX) &&
				(DstRectUp.left<= MouseX) &&
				(DstRectUp.bottom >= MouseY) &&
				(DstRectUp.top<= MouseY)) && !isDialogBoxDrawing())
		{
			fUp = 0.8f;
			RotateInfoObjectX = -tmpRotateInfoObjectX*3;
			RotateInfoObjectY = 0.0f;
		}
		else
		if  (((DstRectDown.right  >= MouseX) &&
				(DstRectDown.left<= MouseX) &&
				(DstRectDown.bottom >= MouseY) &&
				(DstRectDown.top<= MouseY)) && !isDialogBoxDrawing())
		{
			fDown = 0.8f;
			RotateInfoObjectX = tmpRotateInfoObjectX*3;
			RotateInfoObjectY = 0.0f;
		}
		else
			RotateInfoObjectY = 0;

		if (vw_GetWindowLBMouse(true))
		{
			RotateInfoObjectY = -RotationSumY+140; // 140 -базовый угол разворота при создании
			RotateInfoObjectX = -RotationSumX;
		}
	}


	RotationSumY += RotateInfoObjectY;
	if (RotationSumY >= 360.0f) RotationSumY -= 360.0f;
	else if (RotationSumY <= -360.0f) RotationSumY += 360.0f;
	RotationSumX += RotateInfoObjectX;
	if (RotationSumX >= 360.0f) RotationSumX -= 360.0f;
	else if (RotationSumX <= -360.0f) RotationSumX += 360.0f;


	// корректируем положение (у нас объекты стоят не в нулевой точке, а со смещением
	VECTOR3D TMPLocation = ObjectBaseLocation;
	float tmp_matrix[33];
	Matrix33CreateRotate(tmp_matrix, VECTOR3D(RotationSumX,RotationSumY,0));
	Matrix33CalcPoint(&TMPLocation, tmp_matrix);
	TMPLocation += VECTOR3D(1000,-1000,0);







	// рисуем линии

	// делаем массив для всех элементов RI_3f_XYZ | RI_2f_TEX
	float *tmpDATA = 0;
	tmpDATA = new float[4*(3+2)];


	int SizeCell = (int)(vw_sqrtf((InfoObjectLength*InfoObjectLength/4.0f+
	InfoObjectWidth*InfoObjectWidth/4.0f+
	InfoObjectHeight*InfoObjectHeight/4.0f)));

	float LineSize = 0.004f * vw_sqrtf(PointCamera.x*PointCamera.x+PointCamera.y*PointCamera.y+
		PointCamera.z*PointCamera.z);

	if (SizeCell<2) SizeCell = 2;


	vw_PushMatrix();
	vw_CullFace(RI_NONE);

	vw_Translate(VECTOR3D(1000.0f, -1000.0f, 0.0f));
	vw_Rotate(RotationSumY, 0.0f, 1.0f, 0.0f);
	vw_Rotate(RotationSumX, 1.0f, 0.0f, 0.0f);

	vw_SetColor(0.7f, 0.7f, 1.0f, 0.3f*MenuContentTransp);
	vw_SetTextureBlend(true, RI_BLEND_SRCALPHA, RI_BLEND_ONE);
	vw_SetTexture(0, vw_FindTextureByName("DATA/MENU/line.tga"));
	for (int i=-SizeCell; i<SizeCell+2; i+=2)
	{
		// номер float'а в последовательности
		int k=0;

		tmpDATA[k++] = i*1.0f+LineSize;
		tmpDATA[k++] = 0.0f;
		tmpDATA[k++] = SizeCell*1.0f;
		tmpDATA[k++] = 1.0f;
		tmpDATA[k++] = 1.0f;

		tmpDATA[k++] = i*1.0f+LineSize;
		tmpDATA[k++] = 0.0f;
		tmpDATA[k++] = -SizeCell*1.0f;
		tmpDATA[k++] = 1.0f;
		tmpDATA[k++] = 0.0f;

		tmpDATA[k++] = i*1.0f-LineSize;
		tmpDATA[k++] = 0.0f;
		tmpDATA[k++] = SizeCell*1.0f;
		tmpDATA[k++] = 0.0f;
		tmpDATA[k++] = 1.0f;

		tmpDATA[k++] = i*1.0f-LineSize;
		tmpDATA[k++] = 0.0f;
		tmpDATA[k++] = -SizeCell*1.0f;
		tmpDATA[k++] = 0.0f;
		tmpDATA[k] = 0.0f;

		vw_SendVertices(RI_TRIANGLE_STRIP, 4, RI_3f_XYZ | RI_1_TEX, tmpDATA, 5*sizeof(float));


		k = 0;

		tmpDATA[k++] = SizeCell*1.0f;
		tmpDATA[k++] = 0.0f;
		tmpDATA[k++] = i*1.0f+LineSize;
		tmpDATA[k++] = 0.0f;
		tmpDATA[k++] = 0.0f;

		tmpDATA[k++] = SizeCell*1.0f;
		tmpDATA[k++] = 0.0f;
		tmpDATA[k++] = i*1.0f-LineSize;
		tmpDATA[k++] = 1.0f;
		tmpDATA[k++] = 0.0f;

		tmpDATA[k++] = - SizeCell*1.0f;
		tmpDATA[k++] = 0.0f;
		tmpDATA[k++] = i*1.0f + LineSize;
		tmpDATA[k++] = 0.0f;
		tmpDATA[k++] = 1.0f;

		tmpDATA[k++] = - SizeCell*1.0f;
		tmpDATA[k++] = 0.0f;
		tmpDATA[k++] = i*1.0f - LineSize;
		tmpDATA[k++] = 1.0f;
		tmpDATA[k] = 1.0f;

		vw_SendVertices(RI_TRIANGLE_STRIP, 4, RI_3f_XYZ | RI_1_TEX, tmpDATA, 5*sizeof(float));
	}
	if (tmpDATA != 0) {delete [] tmpDATA; tmpDATA = 0;}
	vw_SetTextureBlend(false, 0, 0);
	vw_BindTexture(0, 0);
	vw_SetColor(1.0f, 1.0f, 1.0f, 1.0f);

	vw_CullFace(RI_BACK);
	vw_PopMatrix();






	bool ShadowMap = false;

	if (Setup.ShadowMap > 0)
	{
		float EffectiveDistance = PointCamera.Length();
		ShadowMap_StartRenderToFBO(VECTOR3D(0,0,0), EffectiveDistance, EffectiveDistance*2);

		// т.к. модели у нас не были расчитаны на шадов меппинг (не замкнутые контуры, внутренние грани и т.п.)
		// на некоторых моделях нужно делать тень и по лицевым частям тоже, т.е. вообще выключать cull face
		// важно (!) оставляя лицевые части можно получить погрешности прорисовки уже на лицевых частях, надо выбирать,
		// какие визуальные погрешности при прорисовке теней убрать
		if ((CreateNum == 40) || // торпедная пусковая установка
			(CreateNum == 41) || // установка пуска бомб
			(CreateNum == 48) || // 3-й истребитель пришельцев
			((CreateNum >= 86) && (CreateNum <= 90)) || // первые 4 типа зданий
			(CreateNum == 92))   // 6-й тип зданий
			vw_CullFace(RI_NONE);


		if (InfoFighter != 0)
		{
			InfoFighter->SetLocation(TMPLocation);
			InfoFighter->Draw(true);
		}
		if (InfoWeapon != 0)
		{
			InfoWeapon->SetLocation(TMPLocation);
			InfoWeapon->Draw(true);
		}
		if (InfoMine != 0)
		{
			InfoMine->SetLocation(TMPLocation);
			InfoMine->Draw(true);
		}
		if (InfoAlien != 0)
		{
			InfoAlien->SetLocation(TMPLocation);
			InfoAlien->Draw(true);
		}
		if (InfoAlienMotherShip != 0)
		{
			InfoAlienMotherShip->SetLocation(TMPLocation);
			InfoAlienMotherShip->Draw(true);
		}
		if (InfoPirateShip != 0)
		{
			InfoPirateShip->SetLocation(TMPLocation);
			InfoPirateShip->Draw(true);
			// рисуем оружие
			if (InfoPirateShip->Weapon != 0)
			for (int i=0; i<InfoPirateShip->WeaponQuantity; i++)
			if (InfoPirateShip->Weapon[i] != 0)
			{
				InfoPirateShip->Weapon[i]->Draw(true);
			}
		}
		if (InfoBuilding != 0)
		{
			InfoBuilding->SetLocation(TMPLocation);
			InfoBuilding->Draw(true);
		}
		if (InfoMilitaryBuilding != 0)
		{
			InfoMilitaryBuilding->SetLocation(TMPLocation);
			InfoMilitaryBuilding->Draw(true);
		}
		if (InfoWheeled != 0)
		{
			InfoWheeled->SetLocation(TMPLocation);
			InfoWheeled->Draw(true);
		}
		if (InfoTracked != 0)
		{
			InfoTracked->SetLocation(TMPLocation);
			InfoTracked->Draw(true);
		}


		ShadowMap_EndRenderToFBO();

		ShadowMap = true;
		ShadowMap_StartFinalRender();
	}


	if (InfoFighter != 0)
	{
		InfoFighter->SetLocation(TMPLocation);
		InfoFighter->SetRotation(VECTOR3D(RotateInfoObjectX,RotateInfoObjectY,0.0f));
		InfoFighter->Draw(false, ShadowMap);
		// рисуем эффекты двигателей только для этой модели
		vw_DrawParticleSystems(InfoFighter->Engine, InfoFighter->EngineQuantity);
	}
	if (InfoWeapon != 0)
	{
		InfoWeapon->SetLocation(TMPLocation);
		InfoWeapon->SetRotation(VECTOR3D(RotateInfoObjectX,RotateInfoObjectY,0.0f));
		InfoWeapon->Draw(false, ShadowMap);
	}
	if (InfoMine != 0)
	{
		InfoMine->SetLocation(TMPLocation);
		InfoMine->SetRotation(VECTOR3D(RotateInfoObjectX,RotateInfoObjectY,0.0f));
		InfoMine->Draw(false, ShadowMap);
		// рисуем эффекты двигателей только для этой модели
		vw_DrawParticleSystems(InfoMine->GraphicFX, InfoMine->GraphicFXQuantity);
	}
	if (InfoAlien != 0)
	{
		InfoAlien->SetLocation(TMPLocation);
		InfoAlien->SetRotation(VECTOR3D(RotateInfoObjectX,RotateInfoObjectY,0.0f));
		InfoAlien->Draw(false, ShadowMap);
		// рисуем эффекты двигателей только для этой модели
		vw_DrawParticleSystems(InfoAlien->Engine, InfoAlien->EngineQuantity);
	}
	if (InfoAlienMotherShip != 0)
	{
		InfoAlienMotherShip->SetLocation(TMPLocation);
		InfoAlienMotherShip->SetRotation(VECTOR3D(RotateInfoObjectX,RotateInfoObjectY,0.0f));
		InfoAlienMotherShip->Draw(false, ShadowMap);
		// рисуем эффекты двигателей только для этой модели
		vw_DrawParticleSystems(InfoAlienMotherShip->Engine, InfoAlienMotherShip->EngineQuantity);
	}
	if (InfoPirateShip != 0)
	{
		InfoPirateShip->SetLocation(TMPLocation);
		InfoPirateShip->SetRotation(VECTOR3D(RotateInfoObjectX,RotateInfoObjectY,0.0f));
		InfoPirateShip->Draw(false, ShadowMap);
		// рисуем оружие
		if (InfoPirateShip->Weapon != 0)
		for (int i=0; i<InfoPirateShip->WeaponQuantity; i++)
		if (InfoPirateShip->Weapon[i] != 0)
		{
			InfoPirateShip->Weapon[i]->Draw(false, ShadowMap);
		}
		// рисуем эффекты двигателей только для этой модели
		vw_DrawParticleSystems(InfoPirateShip->Engine, InfoPirateShip->EngineQuantity);
	}
	if (InfoBuilding != 0)
	{
		InfoBuilding->SetLocation(TMPLocation);
		InfoBuilding->SetRotation(VECTOR3D(RotateInfoObjectX,RotateInfoObjectY,0.0f));
		InfoBuilding->Draw(false, ShadowMap);
	}
	if (InfoMilitaryBuilding != 0)
	{
		InfoMilitaryBuilding->SetLocation(TMPLocation);
		InfoMilitaryBuilding->SetRotation(VECTOR3D(RotateInfoObjectX,RotateInfoObjectY,0.0f));
		InfoMilitaryBuilding->Draw(false, ShadowMap);
	}
	if (InfoWheeled != 0)
	{
		InfoWheeled->SetLocation(TMPLocation);
		InfoWheeled->SetRotation(VECTOR3D(RotateInfoObjectX,RotateInfoObjectY,0.0f));
		InfoWheeled->Draw(false, ShadowMap);
	}
	if (InfoTracked != 0)
	{
		InfoTracked->SetLocation(TMPLocation);
		InfoTracked->SetRotation(VECTOR3D(RotateInfoObjectX,RotateInfoObjectY,0.0f));
		InfoTracked->Draw(false, ShadowMap);
	}


	if (Setup.ShadowMap > 0)
	{
		ShadowMap_EndFinalRender();
	}





	vw_SetCameraLocation(VECTOR3D(-50,30,-50));
	vw_SetViewport(x, y, width, height, 0.0f, 1.0f);
	vw_ResizeScene(45.0f, Setup.fAspectRatioWidth/Setup.fAspectRatioHeight, 1.0f, 2000.0f);


	// бордюр с тенью
	vw_Start2DMode(-1,1);
	RECT SrcRect;
	SetRect(&SrcRect,2,2,482,371);
	SetRect(&DstRect,Setup.iAspectRatioWidth/2-450,80-18,Setup.iAspectRatioWidth/2+30,80+351);
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/panel444_333_border.tga"), true, 1.0f*MenuContentTransp);

	// отрисовка стрелок
	SetRect(&SrcRect,32,0,64,32);
	vw_DrawTransparent(&DstRectLeft, &SrcRect, vw_FindTextureByName("DATA/MENU/arrows_blue.tga"), true, fLeft*MenuContentTransp);
	SetRect(&SrcRect,96,0,128,32);
	vw_DrawTransparent(&DstRectRight, &SrcRect, vw_FindTextureByName("DATA/MENU/arrows_blue.tga"), true, fRight*MenuContentTransp);
	SetRect(&SrcRect,0,0,32,32);
	vw_DrawTransparent(&DstRectUp, &SrcRect, vw_FindTextureByName("DATA/MENU/arrows_blue.tga"), true, fUp*MenuContentTransp);
	SetRect(&SrcRect,64,0,96,32);
	vw_DrawTransparent(&DstRectDown, &SrcRect, vw_FindTextureByName("DATA/MENU/arrows_blue.tga"), true, fDown*MenuContentTransp);

	vw_End2DMode();

}



