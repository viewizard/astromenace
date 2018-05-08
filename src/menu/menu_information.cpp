/************************************************************************************

	AstroMenace
	Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
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

#include "../game.h"
#include "../config/config.h"
#include "../gfx/shadow_map.h"
#include "../object3d/object3d.h"
#include "../object3d/weapon/weapon.h"
#include "../object3d/space_ship/alien_space_fighter/alien_space_fighter.h"
#include "../object3d/space_ship/alien_space_mothership/alien_space_mothership.h"
#include "../object3d/space_ship/pirate_ship/pirate_ship.h"
#include "../object3d/ground_object/building/building.h"
#include "../object3d/ground_object/military_building/military_building.h"
#include "../object3d/ground_object/wheeled/wheeled.h"
#include "../object3d/ground_object/tracked/tracked.h"
#include "../object3d/space_ship/earth_space_fighter/earth_space_fighter.h"



//------------------------------------------------------------------------------------
// переменные
//------------------------------------------------------------------------------------
cEarthSpaceFighter *InfoFighter = nullptr;
cWeapon *InfoWeapon = nullptr;
cProjectile *InfoMine = nullptr;
cAlienSpaceFighter *InfoAlien = nullptr;
cAlienSpaceMotherShip *InfoAlienMotherShip = nullptr;
cPirateShip *InfoPirateShip = nullptr;
cBuilding *InfoBuilding = nullptr;
cMilitaryBuilding *InfoMilitaryBuilding = nullptr;
cWheeled *InfoWheeled = nullptr;
cTracked *InfoTracked = nullptr;


int GetWeaponHullDamage(int Num);
int GetWeaponSystemsDamage(int Num);
char *GetWeaponGroupTitle(int Num);
char *GetWeaponName(int Num);
char *GetShipGroupTitle(int Num);
char *GetWorkshopShipName(int Num);




float		LastRotateInfoObject = 0.0f;
sVECTOR3D	PointCamera;
sVECTOR3D	Point(1000,-1000,0);
sVECTOR3D	ObjectBaseLocation(0,0,0);
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








const char *InfoGroupNames[10] = {
	"Earth Spaceships",
	"Weapons",
	"Space Mines",
	"Alien Spaceships",
	"Alien Motherships",
	"Pirate Spaceships",
	"Civilian Buildings",
	"Military Buildings",
	"Wheeled Vehicles",
	"Tracked Vehicles"
};



//------------------------------------------------------------------------------------
// Получаем номер текущего меню, и следующий элемент при нажатии
//------------------------------------------------------------------------------------
int GetInfoNextGroup()
{
	if (CreateNum>=InfoFighterStart && CreateNum<InfoFighterStart+InfoFighterQuant) {
		return 2;
	}
	if (CreateNum>=InfoWeaponStart && CreateNum<InfoWeaponStart+InfoWeaponQuant) {
		return 3;
	}
	if (CreateNum>=InfoMineStart && CreateNum<InfoMineStart+InfoMineQuant) {
		return 4;
	}
	if (CreateNum>=InfoAlienStart && CreateNum<InfoAlienStart+InfoAlienQuant) {
		return 5;
	}
	if (CreateNum>=InfoAlienMotherShipStart && CreateNum<InfoAlienMotherShipStart+InfoAlienMotherShipQuant) {
		return 6;
	}
	if (CreateNum>=InfoPirateShipStart && CreateNum<InfoPirateShipStart+InfoPirateShipQuant) {
		return 7;
	}
	if (CreateNum>=InfoBuildingStart && CreateNum<InfoBuildingStart+InfoBuildingQuant) {
		return 8;
	}
	if (CreateNum>=InfoMilitaryBuildingStart && CreateNum<InfoMilitaryBuildingStart+InfoMilitaryBuildingQuant) {
		return 9;
	}
	if (CreateNum>=InfoWheeledStart && CreateNum<InfoWheeledStart+InfoWheeledQuant) {
		return 10;
	}
	if (CreateNum>=InfoTrackedStart && CreateNum<InfoTrackedStart+InfoTrackedQuant) {
		return 1;
	}

	return 1;
}
int GetInfoPrevGroup()
{
	if (CreateNum>=InfoFighterStart && CreateNum<InfoFighterStart+InfoFighterQuant) {
		return 10;
	}
	if (CreateNum>=InfoWeaponStart && CreateNum<InfoWeaponStart+InfoWeaponQuant) {
		return 1;
	}
	if (CreateNum>=InfoMineStart && CreateNum<InfoMineStart+InfoMineQuant) {
		return 2;
	}
	if (CreateNum>=InfoAlienStart && CreateNum<InfoAlienStart+InfoAlienQuant) {
		return 3;
	}
	if (CreateNum>=InfoAlienMotherShipStart && CreateNum<InfoAlienMotherShipStart+InfoAlienMotherShipQuant) {
		return 4;
	}
	if (CreateNum>=InfoPirateShipStart && CreateNum<InfoPirateShipStart+InfoPirateShipQuant) {
		return 5;
	}
	if (CreateNum>=InfoBuildingStart && CreateNum<InfoBuildingStart+InfoBuildingQuant) {
		return 6;
	}
	if (CreateNum>=InfoMilitaryBuildingStart && CreateNum<InfoMilitaryBuildingStart+InfoMilitaryBuildingQuant) {
		return 7;
	}
	if (CreateNum>=InfoWheeledStart && CreateNum<InfoWheeledStart+InfoWheeledQuant) {
		return 8;
	}
	if (CreateNum>=InfoTrackedStart && CreateNum<InfoTrackedStart+InfoTrackedQuant) {
		return 9;
	}

	return 1;
}


int GetInfoSwitchToGroup(int Group)
{
	switch (Group) {
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
	if (InfoFighter != nullptr) {
		delete InfoFighter;
		InfoFighter = nullptr;
	}
	if (InfoWeapon != nullptr) {
		delete InfoWeapon;
		InfoWeapon = nullptr;
	}
	if (InfoMine != nullptr) {
		delete InfoMine;
		InfoMine = nullptr;
	}
	if (InfoAlien != nullptr) {
		delete InfoAlien;
		InfoAlien = nullptr;
	}
	if (InfoAlienMotherShip != nullptr) {
		delete InfoAlienMotherShip;
		InfoAlienMotherShip = nullptr;
	}
	if (InfoPirateShip != nullptr) {
		delete InfoPirateShip;
		InfoPirateShip = nullptr;
	}
	if (InfoBuilding != nullptr) {
		delete InfoBuilding;
		InfoBuilding = nullptr;
	}
	if (InfoMilitaryBuilding != nullptr) {
		delete InfoMilitaryBuilding;
		InfoMilitaryBuilding = nullptr;
	}
	if (InfoWheeled != nullptr) {
		delete InfoWheeled;
		InfoWheeled = nullptr;
	}
	if (InfoTracked != nullptr) {
		delete InfoTracked;
		InfoTracked = nullptr;
	}
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

	Point = sVECTOR3D(1000,-1000,0);

	if (CreateNum>=InfoFighterStart && CreateNum<InfoFighterStart+InfoFighterQuant) {
		int tmpCreateNum = CreateNum-InfoFighterStart+1;
		InfoFighter = new cEarthSpaceFighter;
		InfoFighter->Create(tmpCreateNum);
		InfoFighter->ID = 1000;
		InfoFighter->EngineDestroyType = true;
		InfoFighter->SetLocation(sVECTOR3D(1000,-1000-InfoFighter->AABB[6].y, 0));
		ObjectBaseLocation = InfoFighter->Location - sVECTOR3D(1000,-1000,0);

		Point = sVECTOR3D(1000,-1000+InfoFighter->Height/3.0f,0);

		PointCamera = sVECTOR3D(0.0f,(InfoFighter->Length
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
		InfoObjectEngineQuantity = InfoFighter->Engines.size();

		for (auto tmpEngine : InfoFighter->Engines) {
			if (auto sharedEngine = tmpEngine.lock())
				sharedEngine->SpeedOnCreation = -1.0f;
		}

		InfoFighter->SetRotation(sVECTOR3D(RotationSumX,RotationSumY,0.0f));
	}
	if (CreateNum>=InfoWeaponStart && CreateNum<InfoWeaponStart+InfoWeaponQuant) {
		int tmpCreateNum = CreateNum-InfoWeaponStart+1;
		InfoWeapon = new cWeapon;
		InfoWeapon->Create(tmpCreateNum);
		InfoWeapon->ID = 1000;
		ObjectBaseLocation = sVECTOR3D(0.0f,-InfoWeapon->AABB[6].y, -(InfoWeapon->Length/2.0f + InfoWeapon->AABB[6].z));

		Point = sVECTOR3D(1000,-1000+InfoWeapon->Height/3.0f,0);

		PointCamera = sVECTOR3D(0.0f,(InfoWeapon->Length
					     +InfoWeapon->Width+
					     InfoWeapon->Height)*0.3f +InfoWeapon->Height*0.3f,
				       -(InfoWeapon->Length
					 +InfoWeapon->Width+
					 InfoWeapon->Height)*0.7f-InfoWeapon->Height*0.7f);
		InfoObjectWidth = InfoWeapon->Width;
		InfoObjectLength = InfoWeapon->Length;
		InfoObjectHeight = InfoWeapon->Height;
		InfoObjectStrength = InfoWeapon->StrengthStart;

		InfoWeapon->SetRotation(sVECTOR3D(RotationSumX,RotationSumY,0.0f));

		InfoObjectEnergyUse = InfoWeapon->EnergyUse;
		InfoObjectAmmo = InfoWeapon->AmmoStart;
		InfoObjectReload = InfoWeapon->NextFireTime;
		InfoObjectRange = GetProjectileRange(tmpCreateNum);
		// убираем источник света
		if (auto sharedFire = InfoWeapon->Fire.lock())
			vw_ReleaseLight(sharedFire->Light);
	}
	if (CreateNum>=InfoMineStart && CreateNum<InfoMineStart+InfoMineQuant) {
		int tmpCreateNum = CreateNum-InfoMineStart+1;
		InfoMine = new cProjectile;
		InfoMine->Create(213+tmpCreateNum);
		InfoMine->ID = 1000;
		InfoMine->ObjectStatus = -1; // чтобы не наводилось
		InfoMine->SpeedStart = InfoMine->SpeedEnd = InfoMine->Speed = 0.0f;
		InfoMine->GraphicFXDestroyType = true;
		InfoMine->SetLocation(sVECTOR3D(1000,-1000-InfoMine->AABB[6].y, 0));
		ObjectBaseLocation = InfoMine->Location - sVECTOR3D(1000,-1000,0);

		Point = sVECTOR3D(1000,-1000+InfoMine->Height/2.0f,0);

		PointCamera = sVECTOR3D(0.0f,(InfoMine->Length
					     +InfoMine->Width+
					     InfoMine->Height)*0.3f +InfoMine->Height*0.3f,
				       -(InfoMine->Length
					 +InfoMine->Width+
					 InfoMine->Height)*0.7f-InfoMine->Height*0.7f);

		InfoObjectWidth = InfoMine->Width;
		InfoObjectLength = InfoMine->Length;
		InfoObjectHeight = InfoMine->Height;
		InfoObjectStrength = InfoMine->StrengthStart;

		InfoMine->SetRotation(sVECTOR3D(RotationSumX,RotationSumY,0.0f));
	}
	if (CreateNum>=InfoAlienStart && CreateNum<InfoAlienStart+InfoAlienQuant) {
		int tmpCreateNum = CreateNum-InfoAlienStart+1;
		InfoAlien = new cAlienSpaceFighter;
		InfoAlien->Create(tmpCreateNum);
		InfoAlien->ID = 1000;
		InfoAlien->EngineDestroyType = true;
		InfoAlien->SetLocation(sVECTOR3D(1000,-1000-InfoAlien->AABB[6].y, 0));
		ObjectBaseLocation = InfoAlien->Location - sVECTOR3D(1000,-1000,0);

		Point = sVECTOR3D(1000,-1000+InfoAlien->Height/3.0f,0);

		PointCamera = sVECTOR3D(0.0f,(InfoAlien->Length
					     +InfoAlien->Width+
					     InfoAlien->Height)*0.24f +InfoAlien->Height*0.3f,
				       -(InfoAlien->Length
					 +InfoAlien->Width+
					 InfoAlien->Height)*0.56f-InfoAlien->Height*0.7f);

		InfoObjectWidth = InfoAlien->Width;
		InfoObjectLength = InfoAlien->Length;
		InfoObjectHeight = InfoAlien->Height;
		InfoObjectStrength = InfoAlien->StrengthStart;

		for (auto tmpEngine : InfoAlien->Engines) {
			if (auto sharedEngine = tmpEngine.lock())
				sharedEngine->SpeedOnCreation = -1.0f;
		}

		InfoAlien->SetRotation(sVECTOR3D(RotationSumX,RotationSumY,0.0f));
	}
	if (CreateNum>=InfoAlienMotherShipStart && CreateNum<InfoAlienMotherShipStart+InfoAlienMotherShipQuant) {
		int tmpCreateNum = CreateNum-InfoAlienMotherShipStart+1;
		InfoAlienMotherShip = new cAlienSpaceMotherShip;
		InfoAlienMotherShip->Create(tmpCreateNum);
		InfoAlienMotherShip->ID = 1000;
		InfoAlienMotherShip->EngineDestroyType = true;
		InfoAlienMotherShip->SetLocation(sVECTOR3D(1000,-1000-InfoAlienMotherShip->AABB[6].y, 0));
		ObjectBaseLocation = InfoAlienMotherShip->Location - sVECTOR3D(1000,-1000,0);

		Point = sVECTOR3D(1000,-1000+InfoAlienMotherShip->Height/3.0f,0);

		PointCamera = sVECTOR3D(0.0f,(InfoAlienMotherShip->Length
					     +InfoAlienMotherShip->Width+
					     InfoAlienMotherShip->Height)*0.24f +InfoAlienMotherShip->Height*0.3f,
				       -(InfoAlienMotherShip->Length
					 +InfoAlienMotherShip->Width+
					 InfoAlienMotherShip->Height)*0.56f-InfoAlienMotherShip->Height*0.7f);
		InfoObjectWidth = InfoAlienMotherShip->Width;
		InfoObjectLength = InfoAlienMotherShip->Length;
		InfoObjectHeight = InfoAlienMotherShip->Height;
		InfoObjectStrength = InfoAlienMotherShip->StrengthStart;

		for (auto tmpEngine : InfoAlienMotherShip->Engines) {
			if (auto sharedEngine = tmpEngine.lock())
				sharedEngine->SpeedOnCreation = -1.0f;
		}

		InfoAlienMotherShip->SetRotation(sVECTOR3D(RotationSumX,RotationSumY,0.0f));
	}
	if (CreateNum>=InfoPirateShipStart && CreateNum<InfoPirateShipStart+InfoPirateShipQuant) {
		int tmpCreateNum = CreateNum-InfoPirateShipStart+1;
		InfoPirateShip = new cPirateShip;
		InfoPirateShip->Create(tmpCreateNum);
		InfoPirateShip->ID = 1000;
		InfoPirateShip->EngineDestroyType = true;
		InfoPirateShip->SetLocation(sVECTOR3D(1000,-1000-InfoPirateShip->AABB[6].y, 0));
		ObjectBaseLocation = InfoPirateShip->Location - sVECTOR3D(1000,-1000,0);

		Point = sVECTOR3D(1000,-1000+InfoPirateShip->Height/3.0f,0);

		PointCamera = sVECTOR3D(0.0f,(InfoPirateShip->Length
					     +InfoPirateShip->Width+
					     InfoPirateShip->Height)*0.24f +InfoPirateShip->Height*0.3f,
				       -(InfoPirateShip->Length
					 +InfoPirateShip->Width+
					 InfoPirateShip->Height)*0.56f-InfoPirateShip->Height*0.7f);
		InfoObjectWidth = InfoPirateShip->Width;
		InfoObjectLength = InfoPirateShip->Length;
		InfoObjectHeight = InfoPirateShip->Height;
		InfoObjectStrength = InfoPirateShip->StrengthStart;

		for (auto tmpEngine : InfoPirateShip->Engines) {
			if (auto sharedEngine = tmpEngine.lock())
				sharedEngine->SpeedOnCreation = -1.0f;
		}

		// убираем поворот турелей
		if (InfoPirateShip->Weapon != nullptr) {
			for (int i = 0; i < InfoPirateShip->WeaponQuantity; i++) {
				if (InfoPirateShip->Weapon[i] != nullptr)
					InfoPirateShip->Weapon[i]->SetRotation(sVECTOR3D(-InfoPirateShip->Weapon[i]->Rotation.x,
											-InfoPirateShip->Weapon[i]->Rotation.y,
											-InfoPirateShip->Weapon[i]->Rotation.z));
			}
		}

		InfoPirateShip->SetRotation(sVECTOR3D(RotationSumX,RotationSumY,0.0f));
	}
	if (CreateNum>=InfoBuildingStart && CreateNum<InfoBuildingStart+InfoBuildingQuant) {
		int tmpCreateNum = CreateNum-InfoBuildingStart+1;
		InfoBuilding = new cBuilding;
		InfoBuilding->Create(tmpCreateNum);
		InfoBuilding->ID = 1000;
		InfoBuilding->SetLocation(sVECTOR3D(1000,-1000-InfoBuilding->AABB[6].y, 0));
		ObjectBaseLocation = InfoBuilding->Location - sVECTOR3D(1000,-1000,0);

		Point = sVECTOR3D(1000,-1000+InfoBuilding->Height/3.0f,0);

		PointCamera = sVECTOR3D(0.0f,(InfoBuilding->Length
					     +InfoBuilding->Width+
					     InfoBuilding->Height)*0.16f +InfoBuilding->Height*0.3f,
				       -(InfoBuilding->Length
					 +InfoBuilding->Width+
					 InfoBuilding->Height)*0.38f-InfoBuilding->Height*0.7f);
		InfoObjectWidth = InfoBuilding->Width;
		InfoObjectLength = InfoBuilding->Length;
		InfoObjectHeight = InfoBuilding->Height;
		InfoObjectStrength = InfoBuilding->StrengthStart;

		InfoBuilding->SetRotation(sVECTOR3D(RotationSumX,RotationSumY,0.0f));
	}
	if (CreateNum>=InfoMilitaryBuildingStart && CreateNum<InfoMilitaryBuildingStart+InfoMilitaryBuildingQuant) {
		int tmpCreateNum = CreateNum-InfoMilitaryBuildingStart+1;
		InfoMilitaryBuilding = new cMilitaryBuilding;
		InfoMilitaryBuilding->Create(tmpCreateNum);
		InfoMilitaryBuilding->ID = 1000;
		InfoMilitaryBuilding->SetLocation(sVECTOR3D(1000,-1000-InfoMilitaryBuilding->AABB[6].y, 0));
		ObjectBaseLocation = InfoMilitaryBuilding->Location - sVECTOR3D(1000,-1000,0);

		Point = sVECTOR3D(1000,-1000+InfoMilitaryBuilding->Height/3.0f,0);

		PointCamera = sVECTOR3D(0.0f,(InfoMilitaryBuilding->Length
					     +InfoMilitaryBuilding->Width+
					     InfoMilitaryBuilding->Height)*0.24f +InfoMilitaryBuilding->Height*0.3f,
				       -(InfoMilitaryBuilding->Length
					 +InfoMilitaryBuilding->Width+
					 InfoMilitaryBuilding->Height)*0.56f-InfoMilitaryBuilding->Height*0.7f);
		InfoObjectWidth = InfoMilitaryBuilding->Width;
		InfoObjectLength = InfoMilitaryBuilding->Length;
		InfoObjectHeight = InfoMilitaryBuilding->Height;
		InfoObjectStrength = InfoMilitaryBuilding->StrengthStart;

		InfoMilitaryBuilding->SetRotation(sVECTOR3D(RotationSumX,RotationSumY,0.0f));
	}
	if (CreateNum>=InfoWheeledStart && CreateNum<InfoWheeledStart+InfoWheeledQuant) {
		int tmpCreateNum = CreateNum-InfoWheeledStart+1;
		InfoWheeled = new cWheeled;
		InfoWheeled->Create(tmpCreateNum);
		InfoWheeled->DeviationOn = true;
		InfoWheeled->ID = 1000;
		InfoWheeled->SetLocation(sVECTOR3D(1000,-1000-InfoWheeled->AABB[6].y, 0));
		ObjectBaseLocation = InfoWheeled->Location - sVECTOR3D(1000,-1000,0);
		InfoWheeled->WheelTrackSpeed = 350.0f;

		Point = sVECTOR3D(1000,-1000+InfoWheeled->Height/3.0f,0);

		PointCamera = sVECTOR3D(0.0f,(InfoWheeled->Length
					     +InfoWheeled->Width+
					     InfoWheeled->Height)*0.24f +InfoWheeled->Height*0.3f,
				       -(InfoWheeled->Length
					 +InfoWheeled->Width+
					 InfoWheeled->Height)*0.56f-InfoWheeled->Height*0.7f);
		InfoObjectWidth = InfoWheeled->Width;
		InfoObjectLength = InfoWheeled->Length;
		InfoObjectHeight = InfoWheeled->Height;
		InfoObjectStrength = InfoWheeled->StrengthStart;

		InfoWheeled->SetRotation(sVECTOR3D(RotationSumX,RotationSumY,0.0f));
	}
	if (CreateNum>=InfoTrackedStart && CreateNum<InfoTrackedStart+InfoTrackedQuant) {
		int tmpCreateNum = CreateNum-InfoTrackedStart+1;
		InfoTracked = new cTracked;
		InfoTracked->Create(tmpCreateNum);
		InfoTracked->DeviationOn = true;
		InfoTracked->ID = 1000;
		InfoTracked->SetLocation(sVECTOR3D(1000,-1000-InfoTracked->AABB[6].y, 0));
		ObjectBaseLocation = InfoTracked->Location - sVECTOR3D(1000,-1000,0);
		InfoTracked->WheelTrackSpeed = 350.0f;

		Point = sVECTOR3D(1000,-1000+InfoTracked->Height/3.0f,0);

		PointCamera = sVECTOR3D(0.0f,(InfoTracked->Length
					     +InfoTracked->Width+
					     InfoTracked->Height)*0.24f +InfoTracked->Height*0.3f,
				       -(InfoTracked->Length
					 +InfoTracked->Width+
					 InfoTracked->Height)*0.56f-InfoTracked->Height*0.7f);
		InfoObjectWidth = InfoTracked->Width;
		InfoObjectLength = InfoTracked->Length;
		InfoObjectHeight = InfoTracked->Height;
		InfoObjectStrength = InfoTracked->StrengthStart;

		InfoTracked->SetRotation(sVECTOR3D(RotationSumX,RotationSumY,0.0f));
	}

	GameNPCArmorPenalty = TMPGameNPCArmorPenalty;

	LastRotateInfoObject = vw_GetTimeThread(0);
}






void InformationObject3DText(int ObjectNum)
{

	int X1 = GameConfig().InternalWidth / 2 + 68;
	int Y1 = 130;
	int Offset = 30;
	int Size = 194;
	float WScale = -177;


	// общее для файтеров землян
	if (CreateNum>=InfoFighterStart && CreateNum<InfoFighterStart+InfoFighterQuant) {
		Y1 += Offset+5;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Object Class:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Space Ship"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Ship Type:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText(GetShipGroupTitle(ObjectNum-InfoFighterStart+1)));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Codename:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText(GetWorkshopShipName(ObjectNum-InfoFighterStart+1)));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Developer:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Earth Federation"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Status:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Ally"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Armor:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i %s", (int)InfoObjectStrength, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Width:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp,  "%2.1f %s", InfoObjectWidth, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Length:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%2.1f %s", InfoObjectLength, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Height:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%2.1f %s", InfoObjectHeight, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, "%s:", vw_GetText("Weapons"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i %s", InfoObjectWeaponQuantity, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Engines:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i %s", InfoObjectEngineQuantity, vw_GetText("units"));
	}
	// оружие для кораблей землян
	if (CreateNum>=InfoWeaponStart && CreateNum<InfoWeaponStart+InfoWeaponQuant) {
		Y1 += Offset+5;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Object Class:"));
		if (CreateNum<InfoWeaponStart+15)
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Weapon, Cannon"));
		else
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Weapon, Launcher"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Type:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText(GetWeaponGroupTitle(CreateNum-InfoWeaponStart+1)));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Codename:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText(GetWeaponName(CreateNum-InfoWeaponStart+1)));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Developer:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Earth Federation"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Armor:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i %s", (int)InfoObjectStrength, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Damage, Hull:"));
		if ((CreateNum-InfoWeaponStart+1 == 11) |
		    (CreateNum-InfoWeaponStart+1 == 12) |
		    (CreateNum-InfoWeaponStart+1 == 14))
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i %s", GetWeaponHullDamage(CreateNum-InfoWeaponStart+1), vw_GetText("units/sec"));
		else
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i %s", GetWeaponHullDamage(CreateNum-InfoWeaponStart+1), vw_GetText("units/shot"));

		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Damage, Systems:"));
		if ((CreateNum-InfoWeaponStart+1 == 11) |
		    (CreateNum-InfoWeaponStart+1 == 12) |
		    (CreateNum-InfoWeaponStart+1 == 14))
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i %s", GetWeaponSystemsDamage(CreateNum-InfoWeaponStart+1), vw_GetText("units/sec"));
		else
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i %s", GetWeaponSystemsDamage(CreateNum-InfoWeaponStart+1), vw_GetText("units/shot"));

		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Energy Use:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%3.1f %s", InfoObjectEnergyUse, vw_GetText("units per shot"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Ammo:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i %s", InfoObjectAmmo, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Reload:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%3.1f %s", InfoObjectReload, vw_GetText("seconds"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Range:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%3.1f %s", InfoObjectRange, vw_GetText("units"));

	}
	// мины
	if (CreateNum>=InfoMineStart && CreateNum<InfoMineStart+InfoMineQuant) {
		Y1 += Offset+5;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Object Class:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Weapon"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Type:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Space Mine"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Developer:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Pirates"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Armor:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i %s", (int)InfoObjectStrength, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Damage, Hull:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "100 %s", vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Damage, Systems:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "0 %s", vw_GetText("units"));
		Y1 += Offset;


		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, "%s:", vw_GetText("Targeting System"));
		if (CreateNum == InfoMineStart)
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("No"));
		else
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Yes"));
		Y1 += Offset;

		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Auto Cannon:"));
		if (CreateNum == InfoMineStart+2)
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Yes"));
		else
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("No"));
		Y1 += Offset;

		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Auto Launcher:"));
		if (CreateNum == InfoMineStart+3)
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Yes"));
		else
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("No"));
		Y1 += Offset;

	}
	// общее для файтеров пришельцев
	if (CreateNum>=InfoAlienStart && CreateNum<InfoAlienStart+InfoAlienQuant) {
		Y1 += Offset+5;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Object Class:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Space Ship"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Ship Type:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Unknown"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Developer:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Unknown"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Status:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Enemy"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Armor:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i %s", (int)InfoObjectStrength, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Width:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%2.1f %s", InfoObjectWidth, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Length:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%2.1f %s", InfoObjectLength, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Height:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%2.1f %s", InfoObjectHeight, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, "%s:", vw_GetText("Weapons"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Unknown"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Damage:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 10, 200, vw_GetText("units"));
	}
	// общее для MotherShip пришельцев
	if (CreateNum>=InfoAlienMotherShipStart && CreateNum<InfoAlienMotherShipStart+InfoAlienMotherShipQuant) {
		Y1 += Offset+5;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Object Class:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("MotherShip"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Ship Type:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Unknown"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Developer:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Unknown"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Status:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Enemy"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Armor:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i %s", (int)InfoObjectStrength, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Width:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%2.1f %s", InfoObjectWidth, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Length:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%2.1f %s", InfoObjectLength, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Height:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%2.1f %s", InfoObjectHeight, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, "%s:", vw_GetText("Weapons"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Unknown"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Damage:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 50, 500, vw_GetText("units"));
	}
	// общее для пиратов
	if (CreateNum>=InfoPirateShipStart && CreateNum<InfoPirateShipStart+InfoPirateShipQuant) {
		Y1 += Offset+5;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Object Class:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Pirate Ship"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Ship Type:"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Developer:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Pirates"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Status:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Enemy"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Armor:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i %s", (int)InfoObjectStrength, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Width:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%2.1f %s", InfoObjectWidth, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Length:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%2.1f %s", InfoObjectLength, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Height:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%2.1f %s", InfoObjectHeight, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, "%s:", vw_GetText("Weapons"));
		//	vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%s, %s", vw_GetText("Kinetic"), vw_GetText("Propelled"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Damage:"));
		//	vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 10, 200, vw_GetText("units"));
	}
	// общее для зданий
	if (CreateNum>=InfoBuildingStart && CreateNum<InfoBuildingStart+InfoBuildingQuant) {
		Y1 += Offset+5;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Object Class:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Civilian Building"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Developer:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Old Republic"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Status:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Enemy"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Armor:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Indestructible"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Width:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%2.1f %s", InfoObjectWidth, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Length:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%2.1f %s", InfoObjectLength, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Height:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%2.1f %s", InfoObjectHeight, vw_GetText("units"));
	}
	// общее для наземных войск
	if (CreateNum>=InfoMilitaryBuildingStart && CreateNum<InfoMilitaryBuildingStart+InfoMilitaryBuildingQuant) {
		Y1 += Offset+5;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Object Class:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Military Building"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Type:"));
		//	vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "Civilian Building");
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Developer:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Pirates"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Status:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Enemy"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Armor:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i %s", (int)InfoObjectStrength, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Width:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%2.1f %s", InfoObjectWidth, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Length:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%2.1f %s", InfoObjectLength, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Height:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%2.1f %s", InfoObjectHeight, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, "%s:", vw_GetText("Weapons"));
		//	vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "Kinetic");
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Damage:"));
		//	vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 10, 500, vw_GetText("units"));
	}
	// общее для колесного
	if (CreateNum>=InfoWheeledStart && CreateNum<InfoWheeledStart+InfoWheeledQuant) {
		Y1 += Offset+5;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Object Class:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Wheeled Vehicle"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Type:"));
		//	vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "Civilian Building");
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Developer:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Pirates"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Status:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Enemy"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Armor:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i %s", (int)InfoObjectStrength, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Width:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%2.1f %s", InfoObjectWidth, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Length:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%2.1f %s", InfoObjectLength, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Height:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%2.1f %s", InfoObjectHeight, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, "%s:", vw_GetText("Weapons"));
		//	vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "Kinetic");
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Damage:"));
		//	vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 10, 300, vw_GetText("units"));
	}
	// общее для гусенечного
	if (CreateNum>=InfoTrackedStart && CreateNum<InfoTrackedStart+InfoTrackedQuant) {
		Y1 += Offset+5;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Object Class:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Tracked Vehicle"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Type:"));
		//	vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "Civilian Building");
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Developer:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Pirates"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Status:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Enemy"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Armor:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i %s", (int)InfoObjectStrength, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Width:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%2.1f %s", InfoObjectWidth, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Length:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%2.1f %s", InfoObjectLength, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Height:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%2.1f %s", InfoObjectHeight, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, "%s:", vw_GetText("Weapons"));
		//	vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "Kinetic");
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("Damage:"));
		//	vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 10, 500, vw_GetText("units"));
	}



	Y1 = 130;



	switch (ObjectNum) {


	// для пиратов
	case InfoPirateShipStart:
	case InfoPirateShipStart+1:
	case InfoPirateShipStart+2:
	case InfoPirateShipStart+3:
		Y1 += Offset+Offset+5;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Marauder"));
		Y1 += Offset*7;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%s, %s", vw_GetText("Kinetic"), vw_GetText("Propelled"));
		Y1 += Offset;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 10, 100, vw_GetText("units"));
		break;
	case InfoPirateShipStart+4:
		Y1 += Offset+Offset+5;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Warder"));
		Y1 += Offset*7;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%s, %s", vw_GetText("Kinetic"), vw_GetText("Propelled"));
		Y1 += Offset;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 10, 200, vw_GetText("units"));
		break;
	case InfoPirateShipStart+5:
		Y1 += Offset+Offset+5;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Raider"));
		Y1 += Offset*7;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%s, %s", vw_GetText("Kinetic"), vw_GetText("Propelled"));
		Y1 += Offset;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 10, 300, vw_GetText("units"));
		break;
	case InfoPirateShipStart+6:
	case InfoPirateShipStart+7:
		Y1 += Offset+Offset+5;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Missile Carrier"));
		Y1 += Offset*7;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%s, %s", vw_GetText("Kinetic"), vw_GetText("Propelled"));
		Y1 += Offset;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 10, 500, vw_GetText("units"));
		break;
	case InfoPirateShipStart+8:
	case InfoPirateShipStart+9:
		Y1 += Offset+Offset+5;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Battle Freighter"));
		Y1 += Offset*7;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%s, %s", vw_GetText("Kinetic"), vw_GetText("Propelled"));
		Y1 += Offset;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 10, 300, vw_GetText("units"));
		break;
	case InfoPirateShipStart+10:
	case InfoPirateShipStart+11:
	case InfoPirateShipStart+12:
		Y1 += Offset+Offset+5;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Missile Carrier"));
		Y1 += Offset*7;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Propelled"));
		Y1 += Offset;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 50, 300, vw_GetText("units"));
		break;
	case InfoPirateShipStart+13:
	case InfoPirateShipStart+14:
		Y1 += Offset+Offset+5;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Battle Freighter"));
		Y1 += Offset*7;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Propelled"));
		Y1 += Offset;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 50, 200, vw_GetText("units"));
		break;





	// для наземных войск
	case InfoMilitaryBuildingStart:
	case InfoMilitaryBuildingStart+5:
		Y1 += Offset+Offset+5;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Cannon"));
		Y1 += Offset*7;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Kinetic"));
		Y1 += Offset;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 10, 30, vw_GetText("units"));
		break;
	case InfoMilitaryBuildingStart+3:
		Y1 += Offset+Offset+5;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("MachineGun"));
		Y1 += Offset*7;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Kinetic"));
		Y1 += Offset;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 10, 50, vw_GetText("units"));
		break;
	case InfoMilitaryBuildingStart+1:
	case InfoMilitaryBuildingStart+6:
		Y1 += Offset+Offset+5;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Turbo Cannon"));
		Y1 += Offset*7;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Kinetic"));
		Y1 += Offset;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 10, 100, vw_GetText("units"));
		break;
	case InfoMilitaryBuildingStart+2:
		Y1 += Offset+Offset+5;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Missile Launcher"));
		Y1 += Offset*7;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Propelled"));
		Y1 += Offset;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 10, 200, vw_GetText("units"));
		break;
	case InfoMilitaryBuildingStart+4:
		Y1 += Offset+Offset+5;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Cannon Turret"));
		Y1 += Offset*7;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Kinetic"));
		Y1 += Offset;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 10, 100, vw_GetText("units"));
		break;


	// для колесного транспорта
	case InfoWheeledStart:
		Y1 += Offset+Offset+5;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Armored Jeep"));
		Y1 += Offset*7;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Propelled"));
		Y1 += Offset;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 10, 40, vw_GetText("units"));
		break;
	case InfoWheeledStart+1:
		Y1 += Offset+Offset+5;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Jeep"));
		Y1 += Offset*7;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Propelled"));
		Y1 += Offset;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 10, 100, vw_GetText("units"));
		break;
	case InfoWheeledStart+2:
		Y1 += Offset+Offset+5;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Armored Jeep"));
		Y1 += Offset*7;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Kinetic"));
		Y1 += Offset;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 10, 40, vw_GetText("units"));
		break;
	case InfoWheeledStart+3:
		Y1 += Offset+Offset+5;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Jeep"));
		Y1 += Offset*7;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Ion"));
		Y1 += Offset;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 10, 50, vw_GetText("units"));
		break;
	case InfoWheeledStart+4:
		Y1 += Offset+Offset+5;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Armored Jeep"));
		Y1 += Offset*7;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Antimatter"));
		Y1 += Offset;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 10, 100, vw_GetText("units"));
		break;
	case InfoWheeledStart+5:
	case InfoWheeledStart+6:
		Y1 += Offset+Offset+5;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("APC"));
		Y1 += Offset*7;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Kinetic"));
		Y1 += Offset;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 10, 40, vw_GetText("units"));
		break;
	case InfoWheeledStart+7:
		Y1 += Offset+Offset+5;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Missile Launcher"));
		Y1 += Offset*7;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Propelled"));
		Y1 += Offset;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 10, 400, vw_GetText("units"));
		break;


	// для гусенечного транспорта
	case InfoTrackedStart:
	case InfoTrackedStart+1:
	case InfoTrackedStart+5:
	case InfoTrackedStart+6:
	case InfoTrackedStart+8:
		Y1 += Offset+Offset+5;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Tank"));
		Y1 += Offset*7;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Kinetic"));
		Y1 += Offset;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 10, 50, vw_GetText("units"));
		break;
	case InfoTrackedStart+2:
		Y1 += Offset+Offset+5;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Tank"));
		Y1 += Offset*7;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Plasma"));
		Y1 += Offset;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 10, 100, vw_GetText("units"));
		break;
	case InfoTrackedStart+3:
	case InfoTrackedStart+4:
	case InfoTrackedStart+7:
		Y1 += Offset+Offset+5;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Tank"));
		Y1 += Offset*7;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Antimatter"));
		Y1 += Offset;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 10, 200, vw_GetText("units"));
		break;
	case InfoTrackedStart+9:
		Y1 += Offset+Offset+5;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("APC"));
		Y1 += Offset*7;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Kinetic"));
		Y1 += Offset;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 10, 50, vw_GetText("units"));
		break;
	case InfoTrackedStart+10:
	case InfoTrackedStart+11:
	case InfoTrackedStart+12:
		Y1 += Offset+Offset+5;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Missile Launcher"));
		Y1 += Offset*7;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Propelled"));
		Y1 += Offset;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i - %i %s", 10, 400, vw_GetText("units"));
		break;
	case InfoTrackedStart+13:
		Y1 += Offset+Offset+5;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Engineer"));
		Y1 += Offset*7;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Not combatant"));
		Y1 += Offset;
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i %s", 0, vw_GetText("units"));
		break;


	}
}











//------------------------------------------------------------------------------------
// Основная процедура прорисовки
//------------------------------------------------------------------------------------
void InformationMenu()
{
	sRECT SrcRect{0, 0, 2, 2};
	sRECT DstRect{0, 0, static_cast<int>(GameConfig().InternalWidth), 768};
	vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/blackpoint.tga"), true, 0.5f * MenuContentTransp);


	SrcRect(2,2,464-2,353-2);
	DstRect((GameConfig().InternalWidth/2-432)-8,80-8+80,(GameConfig().InternalWidth/2-432)-8+464-4, 80-8+353-4+80);
	vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/panel444_333_back.tga"), true, 0.9f * MenuContentTransp);



	// выводим кол-во и текущую страницу
	int Size = vw_FontSize(vw_GetText("Page %i of %i"), CreateNum, InfoEnd);
	int SizeI = (GameConfig().InternalWidth - Size) / 2;
	vw_DrawText(SizeI, 50+30*16+15, 0, 0, 1.0f, sRGBCOLOR{0.5f, 0.5f, 0.5f}, 0.6f * MenuContentTransp, vw_GetText("Page %i of %i"), CreateNum, InfoEnd);


//	DrawFont(760.0f, 660.0f, 0, 0, 1, 0.2f*MenuContentTransp, "PgUp - Previous Page");
//	DrawFont(760.0f, 680.0f, 0, 0, 1, 0.2f*MenuContentTransp, "PgDown - Next Page");



//PgUp – Previous Page
//PgDown – Next Page

	InformationObject3DText(CreateNum);



	// проверяем колесо мышки
	DstRect(GameConfig().InternalWidth / 2 - 440, 80, GameConfig().InternalWidth / 2 + 440,590);
	if (vw_MouseOverRect(DstRect)) {
		if (vw_GetWheelStatus() != 0 && !isDialogBoxDrawing()) {
			CreateNum += vw_GetWheelStatus();

			if (CreateNum<1) CreateNum = InfoEnd;
			if (CreateNum>InfoEnd) CreateNum = 1;
			CreateInfoObject();

			vw_ResetWheelStatus();
		}
	} else if (vw_GetWheelStatus() != 0) {
		vw_ResetWheelStatus();
	}



	int Prir = 100;
	int X = GameConfig().InternalWidth / 2 - 432;
	int Y = 165+Prir*4;
	if (DrawButton200_2(X,Y+28, vw_GetText(InfoGroupNames[GetInfoPrevGroup()-1]), MenuContentTransp, false)) {
		CreateNum = GetInfoSwitchToGroup(GetInfoPrevGroup());
		CreateInfoObject();
	}


	X = GameConfig().InternalWidth / 2 - 209;
	if (DrawButton200_2(X,Y+28, vw_GetText("Page Up"), MenuContentTransp, false)) {
		CreateNum--;
		if (CreateNum < 1)
			CreateNum = InfoEnd;
		CreateInfoObject();
	}
	if (vw_GetKeyStatus(SDLK_PAGEUP)) {
		CreateNum--;
		if (CreateNum < 1)
			CreateNum = InfoEnd;
		CreateInfoObject();
		vw_SetKeyStatus(SDLK_PAGEUP, false);
	}

	X = GameConfig().InternalWidth / 2 + 9;
	if (DrawButton200_2(X,Y+28, vw_GetText("Page Down"), MenuContentTransp, false)) {
		CreateNum++;
		if (CreateNum > InfoEnd)
			CreateNum = 1;
		CreateInfoObject();
	}
	if (vw_GetKeyStatus(SDLK_PAGEDOWN)) {
		CreateNum++;
		if (CreateNum > InfoEnd)
			CreateNum = 1;
		CreateInfoObject();
		vw_SetKeyStatus(SDLK_PAGEDOWN, false);
	}


	X = GameConfig().InternalWidth / 2 + 432 - 200;
	if (DrawButton200_2(X,Y+28, vw_GetText(InfoGroupNames[GetInfoNextGroup()-1]), MenuContentTransp, false)) {
		CreateNum = GetInfoSwitchToGroup(GetInfoNextGroup());
		CreateInfoObject();
	}


	X = (GameConfig().InternalWidth - 384) / 2;
	Y = Y + Prir;
	if (DrawButton384(X,Y, vw_GetText("MAIN MENU"), MenuContentTransp, &Button1Transp, &LastButton1UpdateTime)) {
		DestroyInfoObject();
		ComBuffer = eCommand::SWITCH_TO_MAIN_MENU;
	}

}













//------------------------------------------------------------------------------------
// Прорисовка 3д части
//------------------------------------------------------------------------------------
void InformationDrawObject()
{

	int MouseX, MouseY;
	vw_GetMousePos(MouseX, MouseY);

	float tmpViewportX, tmpViewportY, tmpViewportWidth, tmpViewportHeight;
	vw_GetViewport(&tmpViewportX, &tmpViewportY, &tmpViewportWidth, &tmpViewportHeight);

	vw_SetViewport((GLint)((GameConfig().InternalWidth / 2 - 432) / (GameConfig().InternalWidth / tmpViewportWidth)), (GLint)(80+80 / (GameConfig().InternalHeight / tmpViewportHeight)),
		       (GLsizei)(444 / (GameConfig().InternalWidth / tmpViewportWidth)), (GLsizei)(333 / (GameConfig().InternalHeight / tmpViewportHeight)));
	vw_ResizeScene(45.0f, 444.0f/333.0f, 1.0f, 2000.0f);
	vw_Clear(RI_DEPTH_BUFFER);


	vw_LoadIdentity();
	vw_SetCameraLocation(sVECTOR3D(1000+PointCamera.x,-1000+PointCamera.y,PointCamera.z));
	vw_SetCameraMoveAroundPoint(Point, 0.0f, sVECTOR3D(0.0f, 0.0f, 0.0f));
	vw_CameraLookAt();






	// вращение объекта

	float RotateInfoObjectY = 15.0f*(vw_GetTimeThread(0) - LastRotateInfoObject);
	float tmpRotateInfoObjectX = 15.0f*(vw_GetTimeThread(0) - LastRotateInfoObject);
	float RotateInfoObjectX = 0;
	LastRotateInfoObject = vw_GetTimeThread(0);

	sRECT DstRectLeft, DstRectRight, DstRectUp, DstRectDown;
	float fLeft, fRight, fUp, fDown;
	fLeft = fRight = fUp = fDown = 0.15f;


	DstRectLeft((GameConfig().InternalWidth/2-432)+10,
		80+80+(333-32)/2,
		(GameConfig().InternalWidth/2-432)+32+10,
		80+80+(333+32)/2);
	DstRectRight((GameConfig().InternalWidth/2-432)+444-32-10,
		80+80+(333-32)/2,
		(GameConfig().InternalWidth/2-432)+444-10,
		80+80+(333+32)/2);
	DstRectUp((GameConfig().InternalWidth/2-432)+(444-32)/2,
		80+80+333-32-10,
		(GameConfig().InternalWidth/2-432)+(444+32)/2,
		80+80+333-10);
	DstRectDown((GameConfig().InternalWidth/2-432)+(444-32)/2,
		80+80+10,
		(GameConfig().InternalWidth/2-432)+(444+32)/2,
		80+80+32+10);

	// для вращения объекта, только если мышка стоит над выводом 3д модели
	sRECT DstRect;
	DstRect((GameConfig().InternalWidth/2-432),
		80+80,
		(GameConfig().InternalWidth/2-432)+444,
		80+80+333);
	if  (((DstRect.right  >= MouseX) &&
	      (DstRect.left<= MouseX) &&
	      (DstRect.bottom >= MouseY) &&
	      (DstRect.top<= MouseY)) && !isDialogBoxDrawing()) {
		fLeft = fRight = fUp = fDown = 0.35f;

		if  (((DstRectLeft.right  >= MouseX) &&
		      (DstRectLeft.left<= MouseX) &&
		      (DstRectLeft.bottom >= MouseY) &&
		      (DstRectLeft.top<= MouseY)) && !isDialogBoxDrawing()) {
			fLeft = 0.8f;
			RotateInfoObjectY = RotateInfoObjectY*3;
		} else if  (((DstRectRight.right  >= MouseX) &&
			     (DstRectRight.left<= MouseX) &&
			     (DstRectRight.bottom >= MouseY) &&
			     (DstRectRight.top<= MouseY)) && !isDialogBoxDrawing()) {
			fRight = 0.8f;
			RotateInfoObjectY = -RotateInfoObjectY*3;
		} else if  (((DstRectUp.right  >= MouseX) &&
			     (DstRectUp.left<= MouseX) &&
			     (DstRectUp.bottom >= MouseY) &&
			     (DstRectUp.top<= MouseY)) && !isDialogBoxDrawing()) {
			fUp = 0.8f;
			RotateInfoObjectX = -tmpRotateInfoObjectX*3;
			RotateInfoObjectY = 0.0f;
		} else if  (((DstRectDown.right  >= MouseX) &&
			     (DstRectDown.left<= MouseX) &&
			     (DstRectDown.bottom >= MouseY) &&
			     (DstRectDown.top<= MouseY)) && !isDialogBoxDrawing()) {
			fDown = 0.8f;
			RotateInfoObjectX = tmpRotateInfoObjectX*3;
			RotateInfoObjectY = 0.0f;
		} else
			RotateInfoObjectY = 0;

		if (vw_GetMouseLeftClick(true)) {
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
	sVECTOR3D TMPLocation = ObjectBaseLocation;
	float tmp_matrix[9];
	vw_Matrix33CreateRotate(tmp_matrix, sVECTOR3D(RotationSumX, RotationSumY, 0));
	vw_Matrix33CalcPoint(TMPLocation, tmp_matrix);
	TMPLocation += sVECTOR3D(1000,-1000,0);







	// рисуем линии

	// делаем массив для всех элементов RI_3f_XYZ | RI_2f_TEX
	float *tmpDATA = new float[4*(3+2)];


	int SizeCell = (int)(vw_sqrtf((InfoObjectLength*InfoObjectLength/4.0f+
				       InfoObjectWidth*InfoObjectWidth/4.0f+
				       InfoObjectHeight*InfoObjectHeight/4.0f)));

	float LineSize = 0.004f * vw_sqrtf(PointCamera.x*PointCamera.x+PointCamera.y*PointCamera.y+
					   PointCamera.z*PointCamera.z);

	if (SizeCell<2) SizeCell = 2;


	vw_PushMatrix();
	vw_CullFace(eCullFace::NONE);

	vw_Translate(sVECTOR3D(1000.0f, -1000.0f, 0.0f));
	vw_Rotate(RotationSumY, 0.0f, 1.0f, 0.0f);
	vw_Rotate(RotationSumX, 1.0f, 0.0f, 0.0f);

	vw_SetColor(0.7f, 0.7f, 1.0f, 0.3f*MenuContentTransp);
	vw_SetTextureBlend(true, eTextureBlendFactor::SRC_ALPHA, eTextureBlendFactor::ONE);
	vw_BindTexture(0, vw_FindTextureByName("menu/line.tga"));
	for (int i=-SizeCell; i<SizeCell+2; i+=2) {
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

		vw_Draw3D(ePrimitiveType::TRIANGLE_STRIP, 4, RI_3f_XYZ | RI_1_TEX, tmpDATA, 5 * sizeof(tmpDATA[0]));


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

		vw_Draw3D(ePrimitiveType::TRIANGLE_STRIP, 4, RI_3f_XYZ | RI_1_TEX, tmpDATA, 5 * sizeof(tmpDATA[0]));
	}
	if (tmpDATA != nullptr)
		delete [] tmpDATA;
	vw_SetTextureBlend(false, eTextureBlendFactor::ONE, eTextureBlendFactor::ZERO);
	vw_BindTexture(0, 0);
	vw_SetColor(1.0f, 1.0f, 1.0f, 1.0f);

	vw_CullFace(eCullFace::BACK);
	vw_PopMatrix();






	bool ShadowMap = false;

	if (GameConfig().ShadowMap > 0) {
		float EffectiveDistance = PointCamera.Length();
		ShadowMap_StartRenderToFBO(sVECTOR3D(0,0,0), EffectiveDistance, EffectiveDistance*2);

		// since some 3D models don't have 'back' sides - tank's tracks, covers elements for tires
		// and tracks, etc., we are forced to disable face's culling during shadows map generation
		vw_CullFace(eCullFace::NONE);

		if (InfoFighter != nullptr) {
			InfoFighter->SetLocation(TMPLocation);
			InfoFighter->Draw(true);
		}
		if (InfoWeapon != nullptr) {
			InfoWeapon->SetLocation(TMPLocation);
			InfoWeapon->Draw(true);
		}
		if (InfoMine != nullptr) {
			InfoMine->SetLocation(TMPLocation);
			InfoMine->Draw(true);
		}
		if (InfoAlien != nullptr) {
			InfoAlien->SetLocation(TMPLocation);
			InfoAlien->Draw(true);
		}
		if (InfoAlienMotherShip != nullptr) {
			InfoAlienMotherShip->SetLocation(TMPLocation);
			InfoAlienMotherShip->Draw(true);
		}
		if (InfoPirateShip != nullptr) {
			InfoPirateShip->SetLocation(TMPLocation);
			InfoPirateShip->Draw(true);
			// рисуем оружие
			if (InfoPirateShip->Weapon != nullptr) {
				for (int i = 0; i < InfoPirateShip->WeaponQuantity; i++) {
					if (InfoPirateShip->Weapon[i] != nullptr)
						InfoPirateShip->Weapon[i]->Draw(true);
				}
			}
		}
		if (InfoBuilding != nullptr) {
			InfoBuilding->SetLocation(TMPLocation);
			InfoBuilding->Draw(true);
		}
		if (InfoMilitaryBuilding != nullptr) {
			InfoMilitaryBuilding->SetLocation(TMPLocation);
			InfoMilitaryBuilding->Draw(true);
		}
		if (InfoWheeled != nullptr) {
			InfoWheeled->SetLocation(TMPLocation);
			InfoWheeled->Draw(true);
		}
		if (InfoTracked != nullptr) {
			InfoTracked->SetLocation(TMPLocation);
			InfoTracked->Draw(true);
		}

		vw_CullFace(eCullFace::BACK);
		ShadowMap_EndRenderToFBO();

		ShadowMap = true;
		ShadowMap_StartFinalRender();
	}


	if (InfoFighter != nullptr) {
		InfoFighter->SetLocation(TMPLocation);
		InfoFighter->SetRotation(sVECTOR3D(RotateInfoObjectX,RotateInfoObjectY,0.0f));
		InfoFighter->Draw(false, ShadowMap);
		// рисуем эффекты двигателей только для этой модели
		vw_DrawParticleSystems(InfoFighter->Engines);
	}
	if (InfoWeapon != nullptr) {
		InfoWeapon->SetLocation(TMPLocation);
		InfoWeapon->SetRotation(sVECTOR3D(RotateInfoObjectX,RotateInfoObjectY,0.0f));
		InfoWeapon->Draw(false, ShadowMap);
	}
	if (InfoMine != nullptr) {
		InfoMine->SetLocation(TMPLocation);
		InfoMine->SetRotation(sVECTOR3D(RotateInfoObjectX,RotateInfoObjectY,0.0f));
		InfoMine->Draw(false, ShadowMap);
		// рисуем эффекты двигателей только для этой модели
		vw_DrawParticleSystems(InfoMine->GraphicFX);
	}
	if (InfoAlien != nullptr) {
		InfoAlien->SetLocation(TMPLocation);
		InfoAlien->SetRotation(sVECTOR3D(RotateInfoObjectX,RotateInfoObjectY,0.0f));
		InfoAlien->Draw(false, ShadowMap);
		// рисуем эффекты двигателей только для этой модели
		vw_DrawParticleSystems(InfoAlien->Engines);
	}
	if (InfoAlienMotherShip != nullptr) {
		InfoAlienMotherShip->SetLocation(TMPLocation);
		InfoAlienMotherShip->SetRotation(sVECTOR3D(RotateInfoObjectX,RotateInfoObjectY,0.0f));
		InfoAlienMotherShip->Draw(false, ShadowMap);
		// рисуем эффекты двигателей только для этой модели
		vw_DrawParticleSystems(InfoAlienMotherShip->Engines);
	}
	if (InfoPirateShip != nullptr) {
		InfoPirateShip->SetLocation(TMPLocation);
		InfoPirateShip->SetRotation(sVECTOR3D(RotateInfoObjectX,RotateInfoObjectY,0.0f));
		InfoPirateShip->Draw(false, ShadowMap);
		// рисуем оружие
		if (InfoPirateShip->Weapon != nullptr) {
			for (int i = 0; i < InfoPirateShip->WeaponQuantity; i++) {
				if (InfoPirateShip->Weapon[i] != nullptr)
					InfoPirateShip->Weapon[i]->Draw(false, ShadowMap);
			}
		}
		// рисуем эффекты двигателей только для этой модели
		vw_DrawParticleSystems(InfoPirateShip->Engines);
	}
	if (InfoBuilding != nullptr) {
		InfoBuilding->SetLocation(TMPLocation);
		InfoBuilding->SetRotation(sVECTOR3D(RotateInfoObjectX,RotateInfoObjectY,0.0f));
		InfoBuilding->Draw(false, ShadowMap);
	}
	if (InfoMilitaryBuilding != nullptr) {
		InfoMilitaryBuilding->SetLocation(TMPLocation);
		InfoMilitaryBuilding->SetRotation(sVECTOR3D(RotateInfoObjectX,RotateInfoObjectY,0.0f));
		InfoMilitaryBuilding->Draw(false, ShadowMap);
	}
	if (InfoWheeled != nullptr) {
		InfoWheeled->SetLocation(TMPLocation);
		InfoWheeled->SetRotation(sVECTOR3D(RotateInfoObjectX,RotateInfoObjectY,0.0f));
		InfoWheeled->Draw(false, ShadowMap);
	}
	if (InfoTracked != nullptr) {
		InfoTracked->SetLocation(TMPLocation);
		InfoTracked->SetRotation(sVECTOR3D(RotateInfoObjectX,RotateInfoObjectY,0.0f));
		InfoTracked->Draw(false, ShadowMap);
	}


	if (GameConfig().ShadowMap > 0) {
		ShadowMap_EndFinalRender();
	}





	vw_SetCameraLocation(sVECTOR3D(-50,30,-50));
	vw_SetViewport(tmpViewportX, tmpViewportY, tmpViewportWidth, tmpViewportHeight);
	vw_ResizeScene(45.0f, GameConfig().InternalWidth / GameConfig().InternalHeight, 1.0f, 2000.0f);


	// бордюр с тенью
	vw_Start2DMode(-1,1);
	sRECT SrcRect;
	SrcRect(2,2,482,371);
	DstRect(GameConfig().InternalWidth/2-450, 80-18+80, GameConfig().InternalWidth/2+30, 80+351+80);
	vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/panel444_333_border.tga"), true, 1.0f*MenuContentTransp);

	// отрисовка стрелок
	SrcRect(32,0,64,32);
	vw_Draw2D(DstRectLeft, SrcRect, vw_FindTextureByName("menu/arrows_blue.tga"), true, fLeft*MenuContentTransp);
	SrcRect(96,0,128,32);
	vw_Draw2D(DstRectRight, SrcRect, vw_FindTextureByName("menu/arrows_blue.tga"), true, fRight*MenuContentTransp);
	SrcRect(0,0,32,32);
	vw_Draw2D(DstRectUp, SrcRect, vw_FindTextureByName("menu/arrows_blue.tga"), true, fUp*MenuContentTransp);
	SrcRect(64,0,96,32);
	vw_Draw2D(DstRectDown, SrcRect, vw_FindTextureByName("menu/arrows_blue.tga"), true, fDown*MenuContentTransp);

	vw_End2DMode();

}



