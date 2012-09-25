/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Menu_Information.cpp

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



#include "../Game.h"
#include "../Defines.h"


// в демо версии вообще тут ничего нет



//------------------------------------------------------------------------------------
// переменные
//------------------------------------------------------------------------------------
CEarthSpaceFighter *InfoFighter = 0;
CWeapon *InfoWeapon = 0;
CProjectile *InfoMine = 0;
VECTOR3D	InfoWeaponPoint;
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
float GetProjectileRange(int Num);




float		LastRotateInfoObject = 0.0f;
VECTOR3D	PointCamera;
VECTOR3D	Point(1000,-1000,0);
int			CreateNum = 1;
float		RotationSum =0.0f;
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
#ifndef DEMO_VERSION

	DestroyInfoObject();

	int TMPGameNPCArmorPenalty = GameNPCArmorPenalty;
	GameNPCArmorPenalty = 1;



	Point = VECTOR3D(1000,-1000,0);

	if (CreateNum>=InfoFighterStart && CreateNum<InfoFighterStart+InfoFighterQuant)
	{
		int tmpCreateNum = CreateNum-InfoFighterStart+1;
		InfoFighter = new CEarthSpaceFighter;
		InfoFighter->Create(tmpCreateNum);
		InfoFighter->ID = 1000;
		InfoFighter->EngineDestroyType = true;
		InfoFighter->SetLocation(VECTOR3D(1000,-1000-InfoFighter->AABB[6].y, 0));

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

		InfoFighter->SetRotation(VECTOR3D(0.0f,RotationSum,0.0f));
	}
	if (CreateNum>=InfoWeaponStart && CreateNum<InfoWeaponStart+InfoWeaponQuant)
	{
		int tmpCreateNum = CreateNum-InfoWeaponStart+1;
		InfoWeapon = new CWeapon;
		InfoWeapon->Create(tmpCreateNum);
		InfoWeapon->ID = 1000;
		InfoWeaponPoint = VECTOR3D(0.0f,-InfoWeapon->AABB[6].y, -(InfoWeapon->Length/2.0f + InfoWeapon->AABB[6].z));

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

		InfoWeapon->SetRotation(VECTOR3D(0.0f,RotationSum,0.0f));
		RotatePoint(&InfoWeaponPoint, VECTOR3D(0.0f, RotationSum, 0.0f));
		InfoWeapon->SetLocation(VECTOR3D(1000+InfoWeaponPoint.x, -1000+InfoWeaponPoint.y, InfoWeaponPoint.z));

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

		InfoMine->SetRotation(VECTOR3D(0.0f,RotationSum,0.0f));

	}
	if (CreateNum>=InfoAlienStart && CreateNum<InfoAlienStart+InfoAlienQuant)
	{
		int tmpCreateNum = CreateNum-InfoAlienStart+1;
		InfoAlien = new CAlienSpaceFighter;
		InfoAlien->Create(tmpCreateNum);
		InfoAlien->ID = 1000;
		InfoAlien->EngineDestroyType = true;
		InfoAlien->SetLocation(VECTOR3D(1000,-1000-InfoAlien->AABB[6].y, 0));

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

		InfoAlien->SetRotation(VECTOR3D(0.0f,RotationSum,0.0f));
	}
	if (CreateNum>=InfoAlienMotherShipStart && CreateNum<InfoAlienMotherShipStart+InfoAlienMotherShipQuant)
	{
		int tmpCreateNum = CreateNum-InfoAlienMotherShipStart+1;
		InfoAlienMotherShip = new CAlienSpaceMotherShip;
		InfoAlienMotherShip->Create(tmpCreateNum);
		InfoAlienMotherShip->ID = 1000;
		InfoAlienMotherShip->EngineDestroyType = true;
		InfoAlienMotherShip->SetLocation(VECTOR3D(1000,-1000-InfoAlienMotherShip->AABB[6].y, 0));

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

		InfoAlienMotherShip->SetRotation(VECTOR3D(0.0f,RotationSum,0.0f));
	}
	if (CreateNum>=InfoPirateShipStart && CreateNum<InfoPirateShipStart+InfoPirateShipQuant)
	{
		int tmpCreateNum = CreateNum-InfoPirateShipStart+1;
		InfoPirateShip = new CPirateShip;
		InfoPirateShip->Create(tmpCreateNum);
		InfoPirateShip->ID = 1000;
		InfoPirateShip->EngineDestroyType = true;
		InfoPirateShip->SetLocation(VECTOR3D(1000,-1000-InfoPirateShip->AABB[6].y, 0));

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

		InfoPirateShip->SetRotation(VECTOR3D(0.0f,RotationSum,0.0f));
	}
	if (CreateNum>=InfoBuildingStart && CreateNum<InfoBuildingStart+InfoBuildingQuant)
	{
		int tmpCreateNum = CreateNum-InfoBuildingStart+1;
		InfoBuilding = new CBuilding;
		InfoBuilding->Create(tmpCreateNum);
		InfoBuilding->ID = 1000;
		InfoBuilding->SetLocation(VECTOR3D(1000,-1000-InfoBuilding->AABB[6].y, 0));


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

		InfoBuilding->SetRotation(VECTOR3D(0.0f,RotationSum,0.0f));
	}
	if (CreateNum>=InfoMilitaryBuildingStart && CreateNum<InfoMilitaryBuildingStart+InfoMilitaryBuildingQuant)
	{
		int tmpCreateNum = CreateNum-InfoMilitaryBuildingStart+1;
		InfoMilitaryBuilding = new CMilitaryBuilding;
		InfoMilitaryBuilding->Create(tmpCreateNum);
		InfoMilitaryBuilding->ID = 1000;
		InfoMilitaryBuilding->SetLocation(VECTOR3D(1000,-1000-InfoMilitaryBuilding->AABB[6].y, 0));

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

		InfoMilitaryBuilding->SetRotation(VECTOR3D(0.0f,RotationSum,0.0f));
	}
	if (CreateNum>=InfoWheeledStart && CreateNum<InfoWheeledStart+InfoWheeledQuant)
	{
		int tmpCreateNum = CreateNum-InfoWheeledStart+1;
		InfoWheeled = new CWheeled;
		InfoWheeled->Create(tmpCreateNum);
		InfoWheeled->DeviationOn = true;
		InfoWheeled->ID = 1000;
		InfoWheeled->SetLocation(VECTOR3D(1000,-1000-InfoWheeled->AABB[6].y, 0));
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

		InfoWheeled->SetRotation(VECTOR3D(0.0f,RotationSum,0.0f));
	}
	if (CreateNum>=InfoTrackedStart && CreateNum<InfoTrackedStart+InfoTrackedQuant)
	{
		int tmpCreateNum = CreateNum-InfoTrackedStart+1;
		InfoTracked = new CTracked;
		InfoTracked->Create(tmpCreateNum);
		InfoTracked->DeviationOn = true;
		InfoTracked->ID = 1000;
		InfoTracked->SetLocation(VECTOR3D(1000,-1000-InfoTracked->AABB[6].y, 0));
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

		InfoTracked->SetRotation(VECTOR3D(0.0f,RotationSum,0.0f));
	}

	GameNPCArmorPenalty = TMPGameNPCArmorPenalty;

	LastRotateInfoObject = vw_GetTime();

#endif // #ifndef DEMO_VERSION
}






void InformationObject3DText(int ObjectNum)
{
#ifndef DEMO_VERSION

	int X1 = Setup.iAspectRatioWidth/2 + 68;
	int Y1 = 50;
	int Offset = 30;
	int Size = 170;

	int SizeB = FontSize(GetText("3_GAME_OBJECTS_INFORMATION"));
	int SizeI = (Setup.iAspectRatioWidth-SizeB)/2;
	DrawFont(SizeI, Y1-14, 0, 0, 0, MenuContentTransp, GetText("3_GAME_OBJECTS_INFORMATION"));



	// общее для файтеров землян
	if (CreateNum>=InfoFighterStart && CreateNum<InfoFighterStart+InfoFighterQuant)
	{
		Y1 += Offset+5;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Object_Class:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Space_Ship"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Ship_Type:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText(GetShipGroupTitle(ObjectNum-InfoFighterStart+1)));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Codename:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText(GetWorkshopShipName(ObjectNum-InfoFighterStart+1)));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Developer:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Earth_Federation"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Status:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Ally"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Armor:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i %s", (int)InfoObjectStrength, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Width:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp,  "%2.1f %s", InfoObjectWidth, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Length:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%2.1f %s", InfoObjectLength, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Height:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%2.1f %s", InfoObjectHeight, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, "%s:", GetText("4_Weapons"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i %s", InfoObjectWeaponQuantity, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Engines:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i %s", InfoObjectEngineQuantity, GetText("4_units"));
	}
	// оружие для кораблей землян
	if (CreateNum>=InfoWeaponStart && CreateNum<InfoWeaponStart+InfoWeaponQuant)
	{
		Y1 += Offset+5;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Object_Class:"));
		if (CreateNum<InfoWeaponStart+15)
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Weapon,_Cannon"));
		else
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Weapon,_Launcher"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Type:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText(GetWeaponGroupTitle(CreateNum-InfoWeaponStart+1)));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Codename:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText(GetWeaponName(CreateNum-InfoWeaponStart+1)));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Developer:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Earth_Federation"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Armor:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i %s", (int)InfoObjectStrength, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Damage,_Hull:"));
		if (CreateNum-InfoWeaponStart+1 == 11 |
			CreateNum-InfoWeaponStart+1 == 12 |
			CreateNum-InfoWeaponStart+1 == 14)
			DrawFont(X1+Size+40, Y1, 0, 0, 0, MenuContentTransp, "%i %s", GetWeaponHullDamage(CreateNum-InfoWeaponStart+1), GetText("4_units/sec"));
		else
			DrawFont(X1+Size+40, Y1, 0, 0, 0, MenuContentTransp, "%i %s", GetWeaponHullDamage(CreateNum-InfoWeaponStart+1), GetText("4_units/shot"));

		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Damage,_Systems:"));
		if (CreateNum-InfoWeaponStart+1 == 11 |
			CreateNum-InfoWeaponStart+1 == 12 |
			CreateNum-InfoWeaponStart+1 == 14)
			DrawFont(X1+Size+40, Y1, 0, 0, 0, MenuContentTransp, "%i %s", GetWeaponSystemsDamage(CreateNum-InfoWeaponStart+1), GetText("4_units/sec"));
		else
			DrawFont(X1+Size+40, Y1, 0, 0, 0, MenuContentTransp, "%i %s", GetWeaponSystemsDamage(CreateNum-InfoWeaponStart+1), GetText("4_units/shot"));

		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Energy_Use:"));
		DrawFont(X1+Size+40, Y1, 0, 0, 0, MenuContentTransp, "%3.1f %s", InfoObjectEnergyUse, GetText("4_units_per_shot"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Ammo:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i %s", InfoObjectAmmo, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Reload:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%3.1f %s", InfoObjectReload, GetText("4_seconds"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Range:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%3.1f %s", InfoObjectRange, GetText("4_units"));

	}
	// мины
	if (CreateNum>=InfoMineStart && CreateNum<InfoMineStart+InfoMineQuant)
	{
		Y1 += Offset+5;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Object_Class:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Weapon"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Type:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Space_Mine"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Developer:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Pirates"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Armor:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i %s", (int)InfoObjectStrength, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Damage,_Hull:"));
		DrawFont(X1+Size+65, Y1, 0, 0, 0, MenuContentTransp, "100 %s", GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Damage,_Systems:"));
		DrawFont(X1+Size+65, Y1, 0, 0, 0, MenuContentTransp, "0 %s", GetText("4_units"));
		Y1 += Offset;


		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, "%s:", GetText("4_Targeting_System"));
		if (CreateNum == InfoMineStart)
			DrawFont(X1+Size+65, Y1, 0, 0, 0, MenuContentTransp, GetText("4_No"));
		else
			DrawFont(X1+Size+65, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Yes"));
		Y1 += Offset;

		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Auto_Cannon:"));
		if (CreateNum == InfoMineStart+2)
			DrawFont(X1+Size+65, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Yes"));
		else
			DrawFont(X1+Size+65, Y1, 0, 0, 0, MenuContentTransp, GetText("4_No"));
		Y1 += Offset;

		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Auto_Launcher:"));
		if (CreateNum == InfoMineStart+3)
			DrawFont(X1+Size+65, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Yes"));
		else
			DrawFont(X1+Size+65, Y1, 0, 0, 0, MenuContentTransp, GetText("4_No"));
		Y1 += Offset;

	}
	// общее для файтеров пришельцев
	if (CreateNum>=InfoAlienStart && CreateNum<InfoAlienStart+InfoAlienQuant)
	{
		Y1 += Offset+5;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Object_Class:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Space_Ship"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Ship_Type:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Unknown"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Developer:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Unknown"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Status:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Enemy"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Armor:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i %s", (int)InfoObjectStrength, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Width:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%2.1f %s", InfoObjectWidth, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Length:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%2.1f %s", InfoObjectLength, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Height:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%2.1f %s", InfoObjectHeight, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, "%s:", GetText("4_Weapons"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Unknown"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Damage:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 10, 200, GetText("4_units"));
	}
	// общее для MotherShip пришельцев
	if (CreateNum>=InfoAlienMotherShipStart && CreateNum<InfoAlienMotherShipStart+InfoAlienMotherShipQuant)
	{
		Y1 += Offset+5;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Object_Class:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_MotherShip"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Ship_Type:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Unknown"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Developer:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Unknown"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Status:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Enemy"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Armor:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i %s", (int)InfoObjectStrength, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Width:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%2.1f %s", InfoObjectWidth, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Length:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%2.1f %s", InfoObjectLength, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Height:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%2.1f %s", InfoObjectHeight, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, "%s:", GetText("4_Weapons"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Unknown"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Damage:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 50, 500, GetText("4_units"));
	}
	// общее для пиратов
	if (CreateNum>=InfoPirateShipStart && CreateNum<InfoPirateShipStart+InfoPirateShipQuant)
	{
		Y1 += Offset+5;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Object_Class:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Pirate_Ship"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Ship_Type:"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Developer:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Pirates"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Status:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Enemy"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Armor:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i %s", (int)InfoObjectStrength, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Width:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%2.1f %s", InfoObjectWidth, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Length:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%2.1f %s", InfoObjectLength, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Height:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%2.1f %s", InfoObjectHeight, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, "%s:", GetText("4_Weapons"));
	//	DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%s, %s", GetText("4_Kinetic"), GetText("4_Propelled"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Damage:"));
	//	DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 10, 200, GetText("4_units"));
	}
	// общее для зданий
	if (CreateNum>=InfoBuildingStart && CreateNum<InfoBuildingStart+InfoBuildingQuant)
	{
		Y1 += Offset+5;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Object_Class:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Civilian_Building"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Developer:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Old_Republic"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Status:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Enemy"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Armor:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Indestructible"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Width:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%2.1f %s", InfoObjectWidth, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Length:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%2.1f %s", InfoObjectLength, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Height:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%2.1f %s", InfoObjectHeight, GetText("4_units"));
	}
	// общее для наземных войск
	if (CreateNum>=InfoMilitaryBuildingStart && CreateNum<InfoMilitaryBuildingStart+InfoMilitaryBuildingQuant)
	{
		Y1 += Offset+5;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Object_Class:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Military_Building"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Type:"));
	//	DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "Civilian Building");
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Developer:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Pirates"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Status:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Enemy"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Armor:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i %s", (int)InfoObjectStrength, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Width:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%2.1f %s", InfoObjectWidth, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Length:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%2.1f %s", InfoObjectLength, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Height:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%2.1f %s", InfoObjectHeight, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, "%s:", GetText("4_Weapons"));
	//	DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "Kinetic");
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Damage:"));
	//	DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 10, 500, GetText("4_units"));
	}
	// общее для колесного
	if (CreateNum>=InfoWheeledStart && CreateNum<InfoWheeledStart+InfoWheeledQuant)
	{
		Y1 += Offset+5;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Object_Class:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Wheeled_Vehicle"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Type:"));
	//	DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "Civilian Building");
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Developer:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Pirates"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Status:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Enemy"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Armor:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i %s", (int)InfoObjectStrength, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Width:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%2.1f %s", InfoObjectWidth, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Length:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%2.1f %s", InfoObjectLength, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Height:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%2.1f %s", InfoObjectHeight, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, "%s:", GetText("4_Weapons"));
	//	DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "Kinetic");
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Damage:"));
	//	DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 10, 300, GetText("4_units"));
	}
	// общее для гусенечного
	if (CreateNum>=InfoTrackedStart && CreateNum<InfoTrackedStart+InfoTrackedQuant)
	{
		Y1 += Offset+5;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Object_Class:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Tracked_Vehicle"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Type:"));
	//	DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "Civilian Building");
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Developer:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Pirates"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Status:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Enemy"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Armor:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i %s", (int)InfoObjectStrength, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Width:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%2.1f %s", InfoObjectWidth, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Length:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%2.1f %s", InfoObjectLength, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Height:"));
		DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%2.1f %s", InfoObjectHeight, GetText("4_units"));
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, "%s:", GetText("4_Weapons"));
	//	DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "Kinetic");
		Y1 += Offset;
		DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, GetText("4_Damage:"));
	//	DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 10, 500, GetText("4_units"));
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
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Marauder"));
			Y1 += Offset*7;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%s, %s", GetText("4_Kinetic"), GetText("4_Propelled"));
			Y1 += Offset;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 10, 100, GetText("4_units"));
			break;
		case InfoPirateShipStart+4:
			Y1 += Offset+Offset+5;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Warder"));
			Y1 += Offset*7;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%s, %s", GetText("4_Kinetic"), GetText("4_Propelled"));
			Y1 += Offset;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 10, 200, GetText("4_units"));
			break;
		case InfoPirateShipStart+5:
			Y1 += Offset+Offset+5;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Raider"));
			Y1 += Offset*7;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%s, %s", GetText("4_Kinetic"), GetText("4_Propelled"));
			Y1 += Offset;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 10, 300, GetText("4_units"));
			break;
		case InfoPirateShipStart+6:
		case InfoPirateShipStart+7:
			Y1 += Offset+Offset+5;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Missile_Carrier"));
			Y1 += Offset*7;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%s, %s", GetText("4_Kinetic"), GetText("4_Propelled"));
			Y1 += Offset;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 10, 500, GetText("4_units"));
			break;
		case InfoPirateShipStart+8:
		case InfoPirateShipStart+9:
			Y1 += Offset+Offset+5;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Battle_Freighter"));
			Y1 += Offset*7;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%s, %s", GetText("4_Kinetic"), GetText("4_Propelled"));
			Y1 += Offset;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 10, 300, GetText("4_units"));
			break;
		case InfoPirateShipStart+10:
		case InfoPirateShipStart+11:
		case InfoPirateShipStart+12:
			Y1 += Offset+Offset+5;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Missile_Carrier"));
			Y1 += Offset*7;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Propelled"));
			Y1 += Offset;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 50, 300, GetText("4_units"));
			break;
		case InfoPirateShipStart+13:
		case InfoPirateShipStart+14:
			Y1 += Offset+Offset+5;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Battle_Freighter"));
			Y1 += Offset*7;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Propelled"));
			Y1 += Offset;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 50, 200, GetText("4_units"));
			break;





	// для наземных войск
		case InfoMilitaryBuildingStart:
		case InfoMilitaryBuildingStart+5:
			Y1 += Offset+Offset+5;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Cannon"));
			Y1 += Offset*7;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Kinetic"));
			Y1 += Offset;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 10, 30, GetText("4_units"));
			break;
		case InfoMilitaryBuildingStart+3:
			Y1 += Offset+Offset+5;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_MachineGun"));
			Y1 += Offset*7;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Kinetic"));
			Y1 += Offset;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 10, 50, GetText("4_units"));
			break;
		case InfoMilitaryBuildingStart+1:
		case InfoMilitaryBuildingStart+6:
			Y1 += Offset+Offset+5;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Turbo_Cannon"));
			Y1 += Offset*7;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Kinetic"));
			Y1 += Offset;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 10, 100, GetText("4_units"));
			break;
		case InfoMilitaryBuildingStart+2:
			Y1 += Offset+Offset+5;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Missile_Launcher"));
			Y1 += Offset*7;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Propelled"));
			Y1 += Offset;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 10, 200, GetText("4_units"));
			break;
		case InfoMilitaryBuildingStart+4:
			Y1 += Offset+Offset+5;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Cannon_Turret"));
			Y1 += Offset*7;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Kinetic"));
			Y1 += Offset;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 10, 100, GetText("4_units"));
			break;


	// для колесного транспорта
		case InfoWheeledStart:
			Y1 += Offset+Offset+5;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Armored_Jeep"));
			Y1 += Offset*7;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Propelled"));
			Y1 += Offset;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 10, 40, GetText("4_units"));
			break;
		case InfoWheeledStart+1:
			Y1 += Offset+Offset+5;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Jeep"));
			Y1 += Offset*7;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Propelled"));
			Y1 += Offset;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 10, 100, GetText("4_units"));
			break;
		case InfoWheeledStart+2:
			Y1 += Offset+Offset+5;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Armored_Jeep"));
			Y1 += Offset*7;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Kinetic"));
			Y1 += Offset;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 10, 40, GetText("4_units"));
			break;
		case InfoWheeledStart+3:
			Y1 += Offset+Offset+5;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Jeep"));
			Y1 += Offset*7;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Ion"));
			Y1 += Offset;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 10, 50, GetText("4_units"));
			break;
		case InfoWheeledStart+4:
			Y1 += Offset+Offset+5;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Armored_Jeep"));
			Y1 += Offset*7;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Antimatter"));
			Y1 += Offset;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 10, 100, GetText("4_units"));
			break;
		case InfoWheeledStart+5:
		case InfoWheeledStart+6:
			Y1 += Offset+Offset+5;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_APC"));
			Y1 += Offset*7;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Kinetic"));
			Y1 += Offset;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 10, 40, GetText("4_units"));
			break;
		case InfoWheeledStart+7:
			Y1 += Offset+Offset+5;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Missile_Launcher"));
			Y1 += Offset*7;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Propelled"));
			Y1 += Offset;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 10, 400, GetText("4_units"));
			break;


	// для гусенечного транспорта
		case InfoTrackedStart:
		case InfoTrackedStart+1:
		case InfoTrackedStart+5:
		case InfoTrackedStart+6:
		case InfoTrackedStart+8:
			Y1 += Offset+Offset+5;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Tank"));
			Y1 += Offset*7;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Kinetic"));
			Y1 += Offset;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 10, 50, GetText("4_units"));
			break;
		case InfoTrackedStart+2:
			Y1 += Offset+Offset+5;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Tank"));
			Y1 += Offset*7;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Plasma"));
			Y1 += Offset;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 10, 100, GetText("4_units"));
			break;
		case InfoTrackedStart+3:
		case InfoTrackedStart+4:
		case InfoTrackedStart+7:
			Y1 += Offset+Offset+5;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Tank"));
			Y1 += Offset*7;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Antimatter"));
			Y1 += Offset;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 10, 200, GetText("4_units"));
			break;
		case InfoTrackedStart+9:
			Y1 += Offset+Offset+5;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_APC"));
			Y1 += Offset*7;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Kinetic"));
			Y1 += Offset;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 10, 50, GetText("4_units"));
			break;
		case InfoTrackedStart+10:
		case InfoTrackedStart+11:
		case InfoTrackedStart+12:
			Y1 += Offset+Offset+5;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Missile_Launcher"));
			Y1 += Offset*7;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Propelled"));
			Y1 += Offset;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i - %i %s", 10, 400, GetText("4_units"));
			break;
		case InfoTrackedStart+13:
			Y1 += Offset+Offset+5;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Engineer"));
			Y1 += Offset*7;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, GetText("4_Not_combatant"));
			Y1 += Offset;
			DrawFont(X1+Size, Y1, 0, 0, 0, MenuContentTransp, "%i %s", 0, GetText("4_units"));
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
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_FighterLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_FighterLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_FighterLine3"));
			break;

  		case InfoFighterStart+1:
		case InfoFighterStart+4:
		case InfoFighterStart+18:
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_InterceptorLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_InterceptorLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_InterceptorLine3"));
			break;

  		case InfoFighterStart+2:
		case InfoFighterStart+15:
		case InfoFighterStart+17:
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_BomberLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_BomberLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_BomberLine3"));
			break;

		case InfoFighterStart+6:
		case InfoFighterStart+12:
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_AttackShipLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_AttackShipLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_AttackShipLine3"));
			break;

  		case InfoFighterStart+7:
		case InfoFighterStart+8:
		case InfoFighterStart+10:
		case InfoFighterStart+20:
		case InfoFighterStart+21:
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_ScoutLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_ScoutLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_ScoutLine3"));
			break;





  		case InfoWeaponStart:
		case InfoWeaponStart+1:
		case InfoWeaponStart+2:
		case InfoWeaponStart+3:
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_KineticLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_KineticLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_KineticLine3"));
			break;

		case InfoWeaponStart+4:
		case InfoWeaponStart+5:
		case InfoWeaponStart+6:
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_IonLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_IonLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_IonLine3"));
			break;

		case InfoWeaponStart+7:
		case InfoWeaponStart+8:
		case InfoWeaponStart+9:
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_PlasmaLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_PlasmaLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_PlasmaLine3"));
			break;

		case InfoWeaponStart+10:
		case InfoWeaponStart+11:
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_MaserLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_MaserLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_MaserLine3"));
			break;

		case InfoWeaponStart+12:
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_AntimatterLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_AntimatterLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_AntimatterLine3"));
			break;

		case InfoWeaponStart+13:
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_LaserLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_LaserLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_LaserLine3"));
			break;

		case InfoWeaponStart+14:
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_GaussLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_GaussLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_GaussLine3"));
			break;

		case InfoWeaponStart+15:
		case InfoWeaponStart+16:
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_MissileLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_MissileLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_MissileLine3"));
			break;

		case InfoWeaponStart+17:
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_TorpedoLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_TorpedoLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_TorpedoLine3"));
			break;

		case InfoWeaponStart+18:
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_NukeBombLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_NukeBombLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_NukeBombLine3"));
			break;


		case InfoMineStart:
		case InfoMineStart+1:
		case InfoMineStart+2:
		case InfoMineStart+3:
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_MineLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_MineLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_MineLine3"));
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
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_UnknownLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_UnknownLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_UnknownLine3"));
			break;





		case InfoPirateShipStart:
		case InfoPirateShipStart+1:
		case InfoPirateShipStart+2:
		case InfoPirateShipStart+3:
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_MarauderLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_MarauderLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_MarauderLine3"));
			break;
		case InfoPirateShipStart+4:
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_WarderLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_WarderLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_WarderLine3"));
			break;
		case InfoPirateShipStart+5:
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_RaiderLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_RaiderLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_RaiderLine3"));
			break;
		case InfoPirateShipStart+6:
		case InfoPirateShipStart+7:
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_MissileCarrierLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_MissileCarrierLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_MissileCarrierLine3"));
			break;
		case InfoPirateShipStart+8:
		case InfoPirateShipStart+9:
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_BattleFreighterLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_BattleFreighterLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_BattleFreighterLine3"));
			break;
		case InfoPirateShipStart+10:
		case InfoPirateShipStart+11:
		case InfoPirateShipStart+12:
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_MissileCarrierLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_MissileCarrierLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_MissileCarrierLine3"));
			break;
		case InfoPirateShipStart+13:
		case InfoPirateShipStart+14:
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_BattleFreighterLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_BattleFreighterLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_BattleFreighterLine3"));
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
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_CivilianBuildingLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_CivilianBuildingLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_CivilianBuildingLine3"));
			break;

		case InfoMilitaryBuildingStart:
		case InfoMilitaryBuildingStart+1:
		case InfoMilitaryBuildingStart+2:
		case InfoMilitaryBuildingStart+3:
		case InfoMilitaryBuildingStart+4:
		case InfoMilitaryBuildingStart+5:
		case InfoMilitaryBuildingStart+6:
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_WeaponturretLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_WeaponturretLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_WeaponturretLine3"));
			break;

		case InfoWheeledStart:
		case InfoWheeledStart+2:
		case InfoWheeledStart+4:
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_ArmoredJeepLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_ArmoredJeepLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_ArmoredJeepLine3"));
			break;

		case InfoWheeledStart+1:
		case InfoWheeledStart+3:
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_JeepLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_JeepLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_JeepLine3"));
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
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_TankLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_TankLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_TankLine3"));
			break;

		case InfoWheeledStart+5:
		case InfoWheeledStart+6:
		case InfoTrackedStart+9:
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_APCLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_APCLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_APCLine3"));
			break;

		case InfoWheeledStart+7:
		case InfoTrackedStart+10:
		case InfoTrackedStart+11:
		case InfoTrackedStart+12:
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_MissileLauncherLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_MissileLauncherLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_MissileLauncherLine3"));
			break;

		case InfoTrackedStart+13:
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_EngineerLine1")); Y1 += Offset;
			DrawFont(X1, Y1, W, 0, 0, MenuContentTransp, GetText("5_EngineerLine2")); Y1 += Offset;
			DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("5_EngineerLine3"));
			break;


	}



#endif // #ifndef DEMO_VERSION
}











//------------------------------------------------------------------------------------
// Основная процедура прорисовки
//------------------------------------------------------------------------------------
void InformationMenu()
{

	RECT SrcRest, DstRest;
	SetRect(&SrcRest,0,0,2,2);
	SetRect(&DstRest,0,0,Setup.iAspectRatioWidth,768);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/blackpoint.tga"), true, 0.5f*MenuContentTransp);

#ifdef DEMO_VERSION

	int Y1 = 50;

	int SizeI = (Setup.iAspectRatioWidth-FontSize(GetText("3_GAME_OBJECTS_INFORMATION")))/2;
	DrawFont(SizeI, Y1-14, 0, 0, 0, MenuContentTransp, GetText("3_GAME_OBJECTS_INFORMATION"));

	Y1 += 20;
	SizeI = (Setup.iAspectRatioWidth-FontSize(GetText("7_Not_Available_in_Demo_Version")))/2;
	DrawFont(SizeI, Y1, 0, 0, 4, MenuContentTransp, GetText("7_Not_Available_in_Demo_Version"));





	int Start = Setup.iAspectRatioWidth/2-436;
	SetRect(&SrcRest,0,0,256,205);
	SetRect(&DstRest,Start,110,Start+256,110+205);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/DEMO/i_image1.tga"), false, MenuContentTransp);

	Start += 256 + 26*2;
	SetRect(&SrcRest,0,0,256,205);
	SetRect(&DstRest,Start,110,Start+256,110+205);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/DEMO/i_image2.tga"), false, MenuContentTransp);

	Start += 256 + 26*2;
	SetRect(&SrcRest,0,0,256,205);
	SetRect(&DstRest,Start,110,Start+256,110+205);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/DEMO/i_image3.tga"), false, MenuContentTransp);


	Start = Setup.iAspectRatioWidth/2-436;
	SetRect(&SrcRest,0,0,256,205);
	SetRect(&DstRest,Start,340,Start+256,340+205);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/DEMO/i_image4.tga"), false, MenuContentTransp);

	Start += 256 + 26*2;
	SetRect(&SrcRest,0,0,256,205);
	SetRect(&DstRest,Start,340,Start+256,340+205);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/DEMO/i_image5.tga"), false, MenuContentTransp);

	Start += 256 + 26*2;
	SetRect(&SrcRest,0,0,256,205);
	SetRect(&DstRest,Start,340,Start+256,340+205);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/DEMO/i_image6.tga"), false, MenuContentTransp);




	Y1 = 570;
	SizeI = (Setup.iAspectRatioWidth-FontSize(GetText("7_Not_Available_in_Demo_Version")))/2;
	DrawFont(SizeI, Y1, 0, 0, 4, MenuContentTransp, GetText("7_Not_Available_in_Demo_Version"));


	Y1 += 35;
	int Size = FontSize(GetText("8_Line11"))/2;
	DrawFont(Setup.iAspectRatioWidth/2-Size, Y1, 0, 0, 2, MenuContentTransp, GetText("8_Line11"));




	int	X = Setup.iAspectRatioWidth/2-284;
	int Y = 165+100*5;

	if (DrawButton256(X,Y, GetText("1_MAIN_MENU"), MenuContentTransp, &Button10Transp, &LastButton10UpdateTime))
	{
		ComBuffer = MAIN_MENU;
	}
	X = Setup.iAspectRatioWidth/2+28;
	if (DrawButton256(X,Y, GetText("1_ORDER_NOW"), MenuContentTransp, &Button11Transp, &LastButton11UpdateTime))
	{
		// нужно свернуть игру, запустить броузер и выйти в основное меню
		SDL_WM_IconifyWindow();

#ifdef EN
		vw_OpenBrouser("http://www.viewizard.com/buynow.php?forceos=windows&forcegame=astromenace");
#endif // EN
#ifdef DE
		vw_OpenBrouser("http://www.viewizard.com/de/buynow.php?forceos=windows&forcegame=astromenace");
#endif // DE
#ifdef RU
		vw_OpenBrouser("http://www.viewizard.com/ru/buynow.php?forceos=windows&forcegame=astromenace");
#endif // RU
	}




#else



	SetRect(&SrcRest,2,2,464-2,353-2);
	SetRect(&DstRest,(Setup.iAspectRatioWidth/2-432)-8,80-8,(Setup.iAspectRatioWidth/2-432)-8+464-4,80-8+353-4);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/panel444_333_back.tga"), true, 0.9f*MenuContentTransp);



	// выводим кол-во и текущую страницу
	int	Size = FontSize(GetText("3_Page_%i_of_%i"), CreateNum, InfoEnd);
	int SizeI = (Setup.iAspectRatioWidth-Size)/2;
	DrawFont(SizeI, 50+30*16+15, 0, 0, 5, 0.6f*MenuContentTransp, GetText("3_Page_%i_of_%i"), CreateNum, InfoEnd);


//	DrawFont(760.0f, 660.0f, 0, 0, 1, 0.2f*MenuContentTransp, "PgUp - Previous Page");
//	DrawFont(760.0f, 680.0f, 0, 0, 1, 0.2f*MenuContentTransp, "PgDown - Next Page");



//PgUp – Previous Page
//PgDown – Next Page

	InformationObject3DText(CreateNum);




	int Prir = 100;
	int X = Setup.iAspectRatioWidth/2 - 432;
	int Y = 165+Prir*4;
	if (DrawButton200_2(X,Y+28, GetText(InfoGroupNames[GetInfoPrevGroup()-1]), MenuContentTransp, false))
	{
		CreateNum = GetInfoSwitchToGroup(GetInfoPrevGroup());
		CreateInfoObject();
	}


	X = Setup.iAspectRatioWidth/2 + 432 - 200;
	if (DrawButton200_2(X,Y+28, GetText(InfoGroupNames[GetInfoNextGroup()-1]), MenuContentTransp, false))
	{
		CreateNum = GetInfoSwitchToGroup(GetInfoNextGroup());
		CreateInfoObject();
	}


	X = Setup.iAspectRatioWidth/2 - 209;
	if (DrawButton200_2(X,Y+28, GetText("1_Page_Up"), MenuContentTransp, false))
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
	if (DrawButton200_2(X,Y+28, GetText("1_Page_Down"), MenuContentTransp, false))
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



	X = (Setup.iAspectRatioWidth - 384)/2;
	Y = Y+Prir;
	if (DrawButton384(X,Y, GetText("1_MAIN_MENU"), MenuContentTransp, &Button1Transp, &LastButton1UpdateTime))
	{
		DestroyInfoObject();
		ComBuffer = MAIN_MENU;
	}

#endif // DEMO_VERSION

}













//------------------------------------------------------------------------------------
// Прорисовка 3д части
//------------------------------------------------------------------------------------
void InformationDrawObject()
{
#ifndef DEMO_VERSION

	int x, y, width, height;
	float znear, zfar;
	vw_GetViewport(&x, &y, &width, &height, &znear, &zfar);


	int W, H;
	vw_GetViewport(0, 0, &W, &H);
	float AWw = W*1.0f;
	float AHw = H*1.0f;
	float AW;
	float AH;
	bool ASpresent=false;
	ASpresent = vw_GetAspectWH(&AW, &AH);


	vw_SetViewport((int)((Setup.iAspectRatioWidth/2-432)/(AW/AWw)), (int)(80/(AH/AHw)), (int)(444/(AW/AWw)), (int)(333/(AH/AHw)), 0.0f, 0.3f, RI_UL_CORNER);
	vw_ResizeScene(45.0f, 444.0f/333.0f, 1.0f, 10000.0f);



	vw_LoadIdentity();
	vw_SetCameraLocation(VECTOR3D(1000+PointCamera.x,-1000+PointCamera.y,PointCamera.z));
	vw_SetCameraMoveAroundPoint(Point, 0.0f, VECTOR3D(0.0f, 0.0f, 0.0f));
	vw_CameraLookAt();



	// рисуем линии

	// буфер для последовательности RI_LINES
	float *tmpDATA = 0;
	// делаем массив для всех элементов RI_3f_XYZ | RI_4f_COLOR | RI_2f_TEX
	tmpDATA = new float[4*(3+4+2)];


	int SizeCell = (int)(vw_sqrtf((InfoObjectLength*InfoObjectLength/4.0f+
	InfoObjectWidth*InfoObjectWidth/4.0f+
	InfoObjectHeight*InfoObjectHeight/4.0f)));
	float Alpha = 0.3f*MenuContentTransp;

	float LineSize = 0.004f * vw_sqrtf(PointCamera.x*PointCamera.x+PointCamera.y*PointCamera.y+
		PointCamera.z*PointCamera.z);

	if (SizeCell<2) SizeCell = 2;

	// особенные для ракетных систем
	if (CreateNum == InfoWeaponStart+17) SizeCell = 3;
	if (CreateNum == InfoWeaponStart+18) SizeCell = 3;


	vw_SetTexBlend(RI_BLEND_SRCALPHA, RI_BLEND_ONE);
	vw_SetTextureT(0, vw_FindTextureByName("DATA/MENU/line.tga"), 1);
	vw_DepthTest(false, -1);
	for (int i=-SizeCell; i<SizeCell+2; i+=2)
	{
		// номер float'а в последовательности
		int k=0;

			tmpDATA[k++] = 1000.0f + i*1.0f+LineSize;
			tmpDATA[k++] = -1000.0f;
			tmpDATA[k++] = SizeCell*1.0f;
			tmpDATA[k++] = 0.7f;
			tmpDATA[k++] = 0.7f;
			tmpDATA[k++] = 1.0f;
			tmpDATA[k++] = Alpha;
			tmpDATA[k++] = 1.0f;
			tmpDATA[k++] = 1.0f;

			tmpDATA[k++] = 1000.0f + i*1.0f+LineSize;
			tmpDATA[k++] = -1000.0f;
			tmpDATA[k++] = -SizeCell*1.0f;
			tmpDATA[k++] = 0.7f;
			tmpDATA[k++] = 0.7f;
			tmpDATA[k++] = 1.0f;
			tmpDATA[k++] = Alpha;
			tmpDATA[k++] = 1.0f;
			tmpDATA[k++] = 0.0f;

			tmpDATA[k++] = 1000.0f + i*1.0f-LineSize;
			tmpDATA[k++] = -1000.0f;
			tmpDATA[k++] = SizeCell*1.0f;
			tmpDATA[k++] = 0.7f;
			tmpDATA[k++] = 0.7f;
			tmpDATA[k++] = 1.0f;
			tmpDATA[k++] = Alpha;
			tmpDATA[k++] = 0.0f;
			tmpDATA[k++] = 1.0f;

			tmpDATA[k++] = 1000.0f + i*1.0f-LineSize;
			tmpDATA[k++] = -1000.0f;
			tmpDATA[k++] = -SizeCell*1.0f;
			tmpDATA[k++] = 0.7f;
			tmpDATA[k++] = 0.7f;
			tmpDATA[k++] = 1.0f;
			tmpDATA[k++] = Alpha;
			tmpDATA[k++] = 0.0f;
			tmpDATA[k++] = 0.0f;

			vw_SendVertices(RI_TRIANGLE_STRIP, 4, RI_3f_XYZ | RI_4f_COLOR | RI_1_TEX, tmpDATA, 9*sizeof(float));


			k = 0;

			tmpDATA[k++] = SizeCell*1.0f;
			tmpDATA[k++] = 0.0f;
			tmpDATA[k++] = LineSize;
			tmpDATA[k++] = 0.7f;
			tmpDATA[k++] = 0.7f;
			tmpDATA[k++] = 1.0f;
			tmpDATA[k++] = Alpha;
			tmpDATA[k++] = 0.0f;
			tmpDATA[k++] = 0.0f;

			tmpDATA[k++] = SizeCell*1.0f;
			tmpDATA[k++] = 0.0f;
			tmpDATA[k++] = -LineSize;
			tmpDATA[k++] = 0.7f;
			tmpDATA[k++] = 0.7f;
			tmpDATA[k++] = 1.0f;
			tmpDATA[k++] = Alpha;
			tmpDATA[k++] = 1.0f;
			tmpDATA[k++] = 0.0f;

			tmpDATA[k++] = - SizeCell*1.0f;
			tmpDATA[k++] = 0.0f;
			tmpDATA[k++] = LineSize;
			tmpDATA[k++] = 0.7f;
			tmpDATA[k++] = 0.7f;
			tmpDATA[k++] = 1.0f;
			tmpDATA[k++] = Alpha;
			tmpDATA[k++] = 0.0f;
			tmpDATA[k++] = 1.0f;

			tmpDATA[k++] = - SizeCell*1.0f;
			tmpDATA[k++] = 0.0f;
			tmpDATA[k++] = -LineSize;
			tmpDATA[k++] = 0.7f;
			tmpDATA[k++] = 0.7f;
			tmpDATA[k++] = 1.0f;
			tmpDATA[k++] = Alpha;
			tmpDATA[k++] = 1.0f;
			tmpDATA[k++] = 1.0f;

			vw_PushMatrix();
			vw_Translate(VECTOR3D(1000.0f, -1000.0f, i*1.0f));
			vw_Rotate(-70.0f,0.0f,0.0f);
			vw_SendVertices(RI_TRIANGLE_STRIP, 4, RI_3f_XYZ | RI_4f_COLOR | RI_1_TEX, tmpDATA, 9*sizeof(float));
			vw_PopMatrix();

	}
	vw_DepthTest(true, 4);
	if (tmpDATA != 0) {delete [] tmpDATA; tmpDATA = 0;}
	vw_SetTextureDef(0);









	vw_LoadIdentity();
	VECTOR3D PointCameraTMP = PointCamera;
	RotatePoint(&PointCameraTMP, VECTOR3D(0.0f, 0.0f, 0.0f));
	vw_SetCameraLocation(VECTOR3D(1000+PointCameraTMP.x,-1000+PointCameraTMP.y,PointCameraTMP.z));
	vw_SetCameraMoveAroundPoint(Point, 0.0f, VECTOR3D(0.0f, 0.0f, 0.0f));
	vw_CameraLookAt();

	float RotateInfoObject = 15.0f*(vw_GetTime() - LastRotateInfoObject);
	LastRotateInfoObject = vw_GetTime();
	RotationSum += RotateInfoObject;





	if (InfoFighter != 0)
	{
		InfoFighter->SetRotation(VECTOR3D(0.0f,RotateInfoObject,0.0f));
		InfoFighter->Draw();
	}
	if (InfoWeapon != 0)
	{
		RotatePointInv(&InfoWeaponPoint, VECTOR3D(0.0f, -(RotationSum-RotateInfoObject), 0.0f));
		RotatePoint(&InfoWeaponPoint, VECTOR3D(0.0f, RotationSum, 0.0f));
	//	InfoWeapon->SetLocation(InfoWeaponPoint);
		InfoWeapon->SetLocation(VECTOR3D(1000+InfoWeaponPoint.x, -1000+InfoWeaponPoint.y, InfoWeaponPoint.z));
		InfoWeapon->SetRotation(VECTOR3D(0.0f,RotateInfoObject,0.0f));
		InfoWeapon->Draw();
	}
	if (InfoMine != 0)
	{
		InfoMine->SetRotation(VECTOR3D(0.0f,RotateInfoObject,0.0f));
		InfoMine->Draw();
	}
	if (InfoAlien != 0)
	{
		InfoAlien->SetRotation(VECTOR3D(0.0f,RotateInfoObject,0.0f));
		InfoAlien->Draw();
	}
	if (InfoAlienMotherShip != 0)
	{
		InfoAlienMotherShip->SetRotation(VECTOR3D(0.0f,RotateInfoObject,0.0f));
		InfoAlienMotherShip->Draw();
	}
	if (InfoPirateShip != 0)
	{
		InfoPirateShip->SetRotation(VECTOR3D(0.0f,RotateInfoObject,0.0f));
		InfoPirateShip->Draw();
		// рисуем оружие
		if (InfoPirateShip->Weapon != 0)
		for (int i=0; i<InfoPirateShip->WeaponQuantity; i++)
		if (InfoPirateShip->Weapon[i] != 0)
		{
			InfoPirateShip->Weapon[i]->Draw();
		}
	}
	if (InfoBuilding != 0)
	{
		InfoBuilding->SetRotation(VECTOR3D(0.0f,RotateInfoObject,0.0f));
		InfoBuilding->Draw();
	}
	if (InfoMilitaryBuilding != 0)
	{
		InfoMilitaryBuilding->SetRotation(VECTOR3D(0.0f,RotateInfoObject,0.0f));
		InfoMilitaryBuilding->Draw();
	}
	if (InfoWheeled != 0)
	{
		InfoWheeled->SetRotation(VECTOR3D(0.0f,RotateInfoObject,0.0f));
		InfoWheeled->Draw();
	}
	if (InfoTracked != 0)
	{
		InfoTracked->SetRotation(VECTOR3D(0.0f,RotateInfoObject,0.0f));
		InfoTracked->Draw();
	}




	vw_DrawAllParticleSystems();



	vw_SetCameraLocation(VECTOR3D(-50,30,-50));
	vw_SetViewport(x, y, width, height, znear, zfar);
	vw_ResizeScene(45.0f, Setup.fAspectRatioWidth/Setup.fAspectRatioHeight, 1.0f, 10000.0f);


	// бордюр с тенью
	vw_Start2DMode(-1,1);
	RECT SrcRest, DstRest;
	SetRect(&SrcRest,2,2,484-2,373-2);
	SetRect(&DstRest,(Setup.iAspectRatioWidth/2-432)+2-20,80+2-20,(Setup.iAspectRatioWidth/2-432)-2+484-20,80-2+373-20);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/panel444_333_border.tga"), true, 1.0f*MenuContentTransp);
	vw_End2DMode();

#endif // #ifndef DEMO_VERSION
}



