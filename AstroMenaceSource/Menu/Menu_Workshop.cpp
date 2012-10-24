/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Menu_Workshop.cpp

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


#include "../Game.h"




//------------------------------------------------------------------------------------
// переменные
//------------------------------------------------------------------------------------
CEarthSpaceFighter *WorkshopFighterGame = 0;
CEarthSpaceFighter *WorkshopNewFighter = 0;
CWeapon *WorkshopNewWeapon = 0;
int	CurrentWorkshopNewFighter = 1;
int	CurrentWorkshopNewWeapon = 1;


VECTOR3D WorkShopPointCamera;
void WorkshopDrawShip(CEarthSpaceFighter *WorkshopFighter, int Mode);
void WorkshopDrawWeapon(CWeapon *Weapon);
int CurrentWorkshop = 3;


// небольшая девиация оружия в слотах
float CurentDeviation = 0.0f;
float NeedDeviation = vw_Randf0*5.0f;
float CurentDeviationSum = 0.0f;
float CurentTime = 0.0f;

// для индикации нужных надписей
float CurrentAlert2 = 1.0f;
float CurrentAlert3 = 1.0f;

// можем рисовать
bool CanDrawWorkshop = false;


void Workshop_Shipyard();
void Workshop_Workshop();
void Workshop_Weaponry();


extern bool DragWeapon;
extern int DragWeaponNum;
extern int DragWeaponLevel;
extern int WeaponSetupSlot;



//------------------------------------------------------------------------------------
// создание корабля игрока, в меню (не при покупке нового корпуса!)
//------------------------------------------------------------------------------------
void WorkshopCreateShip(int Num)
{
	// создаем объект
	if (WorkshopFighterGame != 0){delete WorkshopFighterGame; WorkshopFighterGame = 0;}

	int TMPGameNPCArmorPenalty = GameNPCArmorPenalty;
	GameNPCArmorPenalty = 1;

	WorkshopFighterGame = new CEarthSpaceFighter;
	WorkshopFighterGame->Create(Setup.Profile[CurrentProfile].Ship);
	WorkshopFighterGame->DeviationOn = true;

	WorkshopFighterGame->ID = 1000;
	WorkshopFighterGame->EngineDestroyType = true;
	WorkshopFighterGame->ShowStrength = false;

	WorkshopFighterGame->StrengthStart *= Setup.Profile[CurrentProfile].ShipHullUpgrade;
	WorkshopFighterGame->Strength = Setup.Profile[CurrentProfile].ShipHullCurrentStrength;


    // создаем оружие
	for (int i=0; i<WorkshopFighterGame->WeaponQuantity; i++)
	{
		if (Setup.Profile[CurrentProfile].Weapon[i] != 0)
		{
			if (SetEarthSpaceFighterWeapon(WorkshopFighterGame, i+1, Setup.Profile[CurrentProfile].Weapon[i]))
			{
				// убираем источник света
				if (WorkshopFighterGame->Weapon[i]->Fire != 0)
				if (WorkshopFighterGame->Weapon[i]->Fire->Light != 0){vw_ReleaseLight(WorkshopFighterGame->Weapon[i]->Fire->Light); WorkshopFighterGame->Weapon[i]->Fire->Light = 0;}

				WorkshopFighterGame->Weapon[i]->Ammo = Setup.Profile[CurrentProfile].WeaponAmmo[i];
				WorkshopFighterGame->WeaponYAngle[i] = -Setup.Profile[CurrentProfile].WeaponSlotYAngle[i];

				VECTOR3D NeedAngle = WorkshopFighterGame->Rotation;
				NeedAngle.y += WorkshopFighterGame->WeaponYAngle[i];
				WorkshopFighterGame->Weapon[i]->SetRotation(NeedAngle);
			}
		}
	}


	// создаем системы (визуальные)
	SetEarthSpaceFighterEngine(WorkshopFighterGame, Setup.Profile[CurrentProfile].EngineSystem);
	if (Setup.Profile[CurrentProfile].AdvancedProtectionSystem == 2)
		SetEarthSpaceFighterArmour(WorkshopFighterGame, 7);
	else
		SetEarthSpaceFighterArmour(WorkshopFighterGame, Setup.Profile[CurrentProfile].ShipHullUpgrade-1);

	GameNPCArmorPenalty = TMPGameNPCArmorPenalty;
	WorkshopFighterGame->SetLocation(VECTOR3D(1000,-1000-(WorkshopFighterGame->Height/2.0f + WorkshopFighterGame->AABB[6].y), -(WorkshopFighterGame->Length/2.0f + WorkshopFighterGame->AABB[6].z)));

	if (Num == 1)
		WorkshopFighterGame->SetRotation(VECTOR3D(0.0f,150.0f,0.0f));
	if (Num == 2)
		WorkshopFighterGame->SetRotation(VECTOR3D(0.0f,170.0f,0.0f));
	if (Num == 3)
		WorkshopFighterGame->SetRotation(VECTOR3D(0.0f,180.0f,0.0f));
}






//------------------------------------------------------------------------------------
// создание корабля для покупки
//------------------------------------------------------------------------------------
void WorkshopCreateNewShip()
{
	// создаем объект
	if (WorkshopNewFighter != 0){delete WorkshopNewFighter; WorkshopNewFighter = 0;}

	int TMPGameNPCArmorPenalty = GameNPCArmorPenalty;
	GameNPCArmorPenalty = 1;

	WorkshopNewFighter = new CEarthSpaceFighter;
	WorkshopNewFighter->Create(CurrentWorkshopNewFighter);
	WorkshopNewFighter->DeviationOn = true;

	WorkshopNewFighter->ID = 1001;
	WorkshopNewFighter->EngineDestroyType = true;

	GameNPCArmorPenalty = TMPGameNPCArmorPenalty;
	WorkshopNewFighter->SetLocation(VECTOR3D(2000,-2000-(WorkshopNewFighter->Height/2.0f + WorkshopNewFighter->AABB[6].y), -(WorkshopNewFighter->Length/2.0f + WorkshopNewFighter->AABB[6].z)));

	WorkshopNewFighter->SetRotation(VECTOR3D(0.0f,-45.0f,0.0f));
}




//------------------------------------------------------------------------------------
// создание пушки для покупки
//------------------------------------------------------------------------------------
void WorkshopCreateNewWeapon()
{
	// создаем объект
	if (WorkshopNewWeapon != 0){delete WorkshopNewWeapon; WorkshopNewWeapon = 0;}

	int TMPGameNPCArmorPenalty = GameNPCArmorPenalty;
	GameNPCArmorPenalty = 1;

	WorkshopNewWeapon = new CWeapon;
	WorkshopNewWeapon->Create(CurrentWorkshopNewWeapon);

	GameNPCArmorPenalty = TMPGameNPCArmorPenalty;

	VECTOR3D Ptmp = VECTOR3D(0,-(WorkshopNewWeapon->Height/2.0f + WorkshopNewWeapon->AABB[6].y), -(WorkshopNewWeapon->Length/2.0f + WorkshopNewWeapon->AABB[6].z)-0.5f);
	RotatePoint(&Ptmp, VECTOR3D(0.0f, -45.0f, 0.0f));

	WorkshopNewWeapon->SetLocation(VECTOR3D(3000+Ptmp.x, -3000+Ptmp.y, Ptmp.z));

	WorkshopNewWeapon->SetRotation(VECTOR3D(0.0f,-45.0f,0.0f));

	// убираем источник света
	if (WorkshopNewWeapon->Fire != 0)
	if (WorkshopNewWeapon->Fire->Light != 0){vw_ReleaseLight(WorkshopNewWeapon->Fire->Light); WorkshopNewWeapon->Fire->Light = 0;}
}







//------------------------------------------------------------------------------------
// общая процедура создания
//------------------------------------------------------------------------------------
void WorkshopCreate()
{
	// все установки в исходные
	CurrentWorkshopNewFighter = 1;
	CurrentWorkshopNewWeapon = 1;
	CurentDeviation = 0.0f;
	NeedDeviation = vw_Randf0*5.0f;
	CurentDeviationSum = 0.0f;
	CurentTime = 0.0f;
	CurrentAlert2 = 1.0f;
	CurrentAlert3 = 1.0f;

	// чтобы только при старте не повоторялись!
	if (CurrentWorkshopNewFighter == Setup.Profile[CurrentProfile].Ship) CurrentWorkshopNewFighter ++;
	if (CurrentWorkshopNewFighter > 22) CurrentWorkshopNewFighter = 1;

	WorkshopCreateShip(CurrentWorkshop);
	WorkshopCreateNewShip();
	WorkshopCreateNewWeapon();

	// нужно для получение кнопки в веапон
	NeedCheck = 0;

	CanDrawWorkshop = true;
}









//------------------------------------------------------------------------------------
// удаление всего...
//------------------------------------------------------------------------------------
void WorkshopDestroyData()
{
	if (WorkshopFighterGame != 0){delete WorkshopFighterGame; WorkshopFighterGame = 0;}
	if (WorkshopNewFighter != 0){delete WorkshopNewFighter;	WorkshopNewFighter = 0;}
	if (WorkshopNewWeapon != 0){delete WorkshopNewWeapon; WorkshopNewWeapon = 0;}
}










//------------------------------------------------------------------------------------
// покупка-установка и ремонт систем корабля
//------------------------------------------------------------------------------------
void WorkshopMenu()
{

	// небольшое качение... девиация
	float TimeDelta = vw_GetTime() - CurentTime;
	if (CurentTime==0.0f)
	{
		CurentTime = vw_GetTime();
	}
	else
	{
		CurentTime = vw_GetTime();
		float Sign = 1.0f;
		// нужно двигать
		if (NeedDeviation < 0.0f) Sign = -1.0f;
		if (Sign == 1.0f)
		{if (NeedDeviation < CurentDeviationSum) Sign = -1.0f;}
		else
		{if (NeedDeviation > CurentDeviationSum) Sign = 1.0f;}

		CurentDeviation = Sign*0.7f*TimeDelta;

		if (Sign == 1.0f)
		{
			if (NeedDeviation <= CurentDeviationSum+CurentDeviation)
			{
				CurentDeviation -= CurentDeviationSum+CurentDeviation-NeedDeviation;
				CurentDeviationSum += CurentDeviation;
				NeedDeviation = vw_Randf0*5.0f;
			}
			else CurentDeviationSum += CurentDeviation;
		}
		else
		{
			if (NeedDeviation >= CurentDeviationSum+CurentDeviation)
			{
				CurentDeviation += CurentDeviationSum+CurentDeviation-NeedDeviation;
				CurentDeviationSum += CurentDeviation;
				NeedDeviation = vw_Randf0*5.0f;
			}
			else CurentDeviationSum += CurentDeviation;
		}
	}



	// просчитываем индикацию
	CurrentAlert2 += 0.4f*TimeDelta;
	if (CurrentAlert2 > 1.0f) CurrentAlert2 = 0.3f;
	CurrentAlert3 -= 1.9f*TimeDelta;
	if (CurrentAlert3 < 0.1f) CurrentAlert3 = 1.0f;








	// прорисовка 3д части
	switch (CurrentWorkshop)
	{
// покупка - ремонт корабля
		case 1:
			Workshop_Shipyard();
			break;


// покупка - внутренних систем корабля
		case 2:
			Workshop_Workshop();
			break;

// покупка - оружия корабля
		case 3:
			Workshop_Weaponry();
			break;
	}






	// кнопки

	int X = Setup.iAspectRatioWidth/2-482;
	int Y = 180+100*5;
	if (DrawButton128_2(X,Y, GetText("1_BACK"), MenuContentTransp, false))
	{
		ComBuffer = MISSION;
		CanDrawWorkshop = false;
		// ничего не тянем... только включили меню
		DragWeaponNum = 0;
		DragWeaponLevel = 0;
		DragWeapon = false;
		WeaponSetupSlot = -1;
	}



	X = Setup.iAspectRatioWidth/2-320;
	bool Off = false;
	if (CurrentWorkshop == 1) Off = true;
	if (DrawButton200_2(X,Y, GetText("1_Shipyard"), MenuContentTransp, Off))
	{
		CurrentWorkshop = 1;
		// используем разные повороты объектов, нужно пересоздать объект
		WorkshopCreateShip(CurrentWorkshop);
		WorkshopCreateNewShip();
		// ничего не тянем... только включили меню
		DragWeaponNum = 0;
		DragWeaponLevel = 0;
		DragWeapon = false;
		WeaponSetupSlot = -1;

		if (Setup.NeedShowHint[1]) SetCurrentDialogBox(10);
	}


	X = Setup.iAspectRatioWidth/2-100;
	Off = false;
	if (CurrentWorkshop == 2) Off = true;
	if (DrawButton200_2(X,Y, GetText("1_Workshop"), MenuContentTransp, Off))
	{
		CurrentWorkshop = 2;
		// используем разные повороты объектов, нужно пересоздать объект
		WorkshopCreateShip(CurrentWorkshop);
		// ничего не тянем... только включили меню
		DragWeaponNum = 0;
		DragWeaponLevel = 0;
		DragWeapon = false;
		WeaponSetupSlot = -1;

		if (Setup.NeedShowHint[2]) SetCurrentDialogBox(11);
	}


	X = Setup.iAspectRatioWidth/2+120;
	Off = false;
	if (CurrentWorkshop == 3) Off = true;
	if (DrawButton200_2(X,Y, GetText("1_Weaponry"), MenuContentTransp, Off))
	{
		CurrentWorkshop = 3;
		// используем разные повороты объектов, нужно пересоздать объект
		WorkshopCreateShip(CurrentWorkshop);
		WorkshopCreateNewWeapon();
		// ничего не тянем... только включили меню
		DragWeaponNum = 0;
		DragWeaponLevel = 0;
		DragWeapon = false;
		WeaponSetupSlot = -1;

		if (Setup.NeedShowHint[3]) SetCurrentDialogBox(12);
	}



	X = Setup.iAspectRatioWidth/2+354;
	if (DrawButton128_2(X,Y, GetText("1_START"), MenuContentTransp, false))
	{
		if (Setup.NeedShowHint[4]) SetCurrentDialogBox(13);
		else
		{
			MenuContentTransp = 0.98f; // небольшая "защелка" от быстрых двойных нажатий на кнопку
		//	WorkshopDestroyData();
		//	CanDrawWorkshop = false;
			// ничего не тянем... только включили меню
			DragWeaponNum = 0;
			DragWeaponLevel = 0;
			DragWeapon = false;
			WeaponSetupSlot = -1;
			//
			ComBuffer = 99;
		}
	}
}










//------------------------------------------------------------------------------------
// Прорисовка 3д части
//------------------------------------------------------------------------------------
void WorkshopDrawShip(CEarthSpaceFighter *SpaceFighter, int Mode)
{
	if (!CanDrawWorkshop) return;
	if (SpaceFighter == 0) return;

	int x, y, width, height;
	float znear, zfar;
	vw_GetViewport(&x, &y, &width, &height, &znear, &zfar);

	int W, H;
	vw_GetViewport(0, 0, &W, &H);
	float AWw = W*1.0f;
	float AHw = H*1.0f;
	float AW;
	float AH;
	bool ASpresent = false;
	ASpresent = vw_GetAspectWH(&AW, &AH);


	if (Mode == 1)
	{
		WorkShopPointCamera = VECTOR3D(0.0f, 4.0f, -32.0f);
		SpaceFighter->SetRotation(VECTOR3D(0.0f, 0.0f, CurentDeviation));
		SpaceFighter->SetRotation(VECTOR3D(0.0f,CurentDeviation/2.0f,0.0f));

		vw_SetViewport((int)((Setup.iAspectRatioWidth/2-512)/(AW/AWw)), 0, (int)(1024/(AW/AWw)), (int)(768/(AH/AHw)), 0.0f, 1.0f, RI_UL_CORNER);
		vw_ResizeScene(45.0f, 1024.0f/768.0f, 1.0f, 2000.0f);

		vw_LoadIdentity();
		vw_SetCameraLocation(VECTOR3D(1000+WorkShopPointCamera.x,-1000+WorkShopPointCamera.y,WorkShopPointCamera.z));
		vw_SetCameraMoveAroundPoint(VECTOR3D(1000,-1000-6.0f,0), 0.0f, VECTOR3D(0.0f, 0.0f, 0.0f));
		vw_CameraLookAt();


		unsigned int ShadowMapStage = 0;

		if (Setup.ShadowMap > 0)
		{
			float EffectiveDistance = 20.0f;
			ShadowMap_StartRenderToFBO(VECTOR3D(0,5,0), EffectiveDistance, EffectiveDistance*2);

			SpaceFighter->Draw(true);
			if (SpaceFighter->Weapon != 0)
			for (int i=0; i<SpaceFighter->WeaponQuantity; i++)
			{
				if (SpaceFighter->Weapon[i] != 0)
					SpaceFighter->Weapon[i]->Draw(true);
			}

			ShadowMap_EndRenderToFBO();
			ShadowMapStage = 2;
			ShadowMap_StartFinalRender(ShadowMapStage);
		}

		SpaceFighter->Draw(false, ShadowMapStage);
		if (SpaceFighter->Weapon != 0)
		for (int i=0; i<SpaceFighter->WeaponQuantity; i++)
		{
			if (SpaceFighter->Weapon[i] != 0)
				SpaceFighter->Weapon[i]->Draw(false, ShadowMapStage);
		}

		if (Setup.ShadowMap > 0)
		{
			ShadowMap_EndFinalRender(ShadowMapStage);
		}

		vw_DrawAllParticleSystems();

		vw_SetCameraLocation(VECTOR3D(-50,30,-50));
		vw_SetViewport(x, y, width, height, znear, zfar);
		vw_ResizeScene(45.0f, Setup.fAspectRatioWidth/Setup.fAspectRatioHeight, 1.0f, 2000.0f);
		return;
	}



	if (Mode == 4)
	{
		WorkShopPointCamera = VECTOR3D(0.0f, 35.0f, -0.01f);
		SpaceFighter->SetRotation(VECTOR3D(0.0f, 0.0f, CurentDeviation));
		vw_SetViewport((int)((Setup.iAspectRatioWidth/2)/(AW/AWw)), 30, (int)(512/(AW/AWw)), (int)(638/(AH/AHw)), 0.0f, 0.3f, RI_UL_CORNER);
		vw_ResizeScene(45.0f, 512.0f/608.0f, 1.0f, 2000.0f);
		vw_LoadIdentity();
		vw_SetCameraLocation(VECTOR3D(1000+WorkShopPointCamera.x,-1000+WorkShopPointCamera.y,WorkShopPointCamera.z));
		vw_SetCameraMoveAroundPoint(VECTOR3D(1000,-1000, 0.0f), 0.0f, VECTOR3D(0.0f, 180.0f, 0.0f));
		vw_CameraLookAt();


		unsigned int ShadowMapStage = 0;

		if (Setup.ShadowMap > 0)
		{
			float EffectiveDistance = 20.0f;
			ShadowMap_StartRenderToFBO(VECTOR3D(0,0,0), EffectiveDistance, EffectiveDistance*2);

			SpaceFighter->Draw(true);
			if (SpaceFighter->Weapon != 0)
			for (int i=0; i<SpaceFighter->WeaponQuantity; i++)
			{
				if (SpaceFighter->Weapon[i] != 0)
					SpaceFighter->Weapon[i]->Draw(true);
			}

			ShadowMap_EndRenderToFBO();
			ShadowMapStage = 2;
			ShadowMap_StartFinalRender(ShadowMapStage);
		}

		SpaceFighter->Draw(false, ShadowMapStage);
		if (SpaceFighter->Weapon != 0)
		for (int i=0; i<SpaceFighter->WeaponQuantity; i++)
		{
			if (SpaceFighter->Weapon[i] != 0)
				SpaceFighter->Weapon[i]->Draw(false, ShadowMapStage);
		}

		if (Setup.ShadowMap > 0)
		{
			ShadowMap_EndFinalRender(ShadowMapStage);
		}

		vw_DrawAllParticleSystems();

		vw_SetCameraLocation(VECTOR3D(-50,30,-50));
		vw_SetViewport(x, y, width, height, znear, zfar);
		vw_ResizeScene(45.0f, Setup.fAspectRatioWidth/Setup.fAspectRatioHeight, 1.0f, 2000.0f);
		return;
	}






	if (Mode == 3)
	{
		WorkShopPointCamera = VECTOR3D(0.0f, 10.0f, -34.0f);
		SpaceFighter->SetRotation(VECTOR3D(0.0f,CurentDeviation/2.0f,0.0f));
		vw_SetViewport((int)((Setup.iAspectRatioWidth/2)/(AW/AWw)), (int)(0/(AH/AHw)), (int)(512/(AW/AWw)), (int)(512/(AH/AHw)), 0.0f, 0.3f, RI_UL_CORNER);
		vw_ResizeScene(45.0f, 512.0f/512.0f, 1.0f, 2000.0f);
		vw_LoadIdentity();
		vw_SetCameraLocation(VECTOR3D(1000+WorkShopPointCamera.x/1.2f,-1000+WorkShopPointCamera.y/1.2f,WorkShopPointCamera.z/1.2f));
		vw_SetCameraMoveAroundPoint(VECTOR3D(1000,-1000-SpaceFighter->AABB[6].y-SpaceFighter->Height/3,0), 0.0f, VECTOR3D(0.0f, 0.0f, 0.0f));
	}
	if (Mode == 2)
	{
		WorkShopPointCamera = VECTOR3D(0.0f, 10.0f, -34.0f);
		VECTOR3D PointCameraTMP = WorkShopPointCamera;
		RotatePoint(&PointCameraTMP, VECTOR3D(0.0f, -90.0f, 0.0f));
		SpaceFighter->SetRotation(VECTOR3D(0.0f,CurentDeviation/2.0f,0.0f));
		vw_SetViewport((int)((Setup.iAspectRatioWidth/2-512)/(AW/AWw)), (int)(0/(AH/AHw)), (int)(512/(AW/AWw)), (int)(512/(AH/AHw)), 0.0f, 0.3f, RI_UL_CORNER);
		vw_ResizeScene(45.0f, 512.0f/512.0f, 1.0f, 2000.0f);
		vw_LoadIdentity();
		vw_SetCameraLocation(VECTOR3D(2000+PointCameraTMP.x/1.2f,-2000+PointCameraTMP.y/1.2f,PointCameraTMP.z/1.2f));
		vw_SetCameraMoveAroundPoint(VECTOR3D(2000,-2000-SpaceFighter->AABB[6].y-SpaceFighter->Height/3,0), 0.0f, VECTOR3D(0.0f, 170.0f, 0.0f));
	}
	vw_CameraLookAt();


	unsigned int ShadowMapStage = 0;

	if (Setup.ShadowMap > 0)
	{
		float EffectiveDistance = 20.0f;
		ShadowMap_StartRenderToFBO(VECTOR3D(0,-2,0), EffectiveDistance, EffectiveDistance*2);

		SpaceFighter->Draw(true);
		if (SpaceFighter->Weapon != 0)
		for (int i=0; i<SpaceFighter->WeaponQuantity; i++)
		{
			if (SpaceFighter->Weapon[i] != 0)
				SpaceFighter->Weapon[i]->Draw(true);
		}

		ShadowMap_EndRenderToFBO();
		ShadowMapStage = 2;
		ShadowMap_StartFinalRender(ShadowMapStage);
	}

	SpaceFighter->Draw(false, ShadowMapStage);

	if (SpaceFighter->Weapon != 0)
	for (int i=0; i<SpaceFighter->WeaponQuantity; i++)
	{
		if (SpaceFighter->Weapon[i] != 0)
			SpaceFighter->Weapon[i]->Draw(false, ShadowMapStage);
	}

	if (Setup.ShadowMap > 0)
	{
		ShadowMap_EndFinalRender(ShadowMapStage);
	}

	vw_DrawAllParticleSystems();

	vw_SetCameraLocation(VECTOR3D(-50,30,-50));
	vw_SetViewport(x, y, width, height, znear, zfar);
	vw_ResizeScene(45.0f, Setup.fAspectRatioWidth/Setup.fAspectRatioHeight, 1.0f, 2000.0f);

}


void WorkshopDrawWeapon(CWeapon *Weapon)
{
	if (!CanDrawWorkshop) return;
	if (Weapon == 0) return;

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


	// удаление относительно размеров оружия
	if (Weapon->ObjectCreationType == 18 || Weapon->ObjectCreationType == 19)
		WorkShopPointCamera = VECTOR3D(0.0f, 1.25f, -5.0f);
	else
		WorkShopPointCamera = VECTOR3D(0.0f, 1.0f, -4.0f);

	VECTOR3D PointCameraTMP = WorkShopPointCamera;
	RotatePoint(&PointCameraTMP, VECTOR3D(0.0f, -90.0f, 0.0f));

	Weapon->SetRotation(VECTOR3D(0.0f,CurentDeviation/2.0f,0.0f));
	vw_SetViewport((int)((Setup.iAspectRatioWidth/2-448)/(AW/AWw)), (int)(105/(AH/AHw)), (int)(384/(AW/AWw)), (int)(350/(AH/AHw)), 0.0f, 0.3f, RI_UL_CORNER);
	vw_ResizeScene(45.0f, 384.0f/350.0f, 1.0f, 2000.0f);
	vw_LoadIdentity();
	vw_SetCameraLocation(VECTOR3D(3000+PointCameraTMP.x,-3000+PointCameraTMP.y,PointCameraTMP.z));
	vw_SetCameraMoveAroundPoint(VECTOR3D(3000,-3000,0), 0.0f, VECTOR3D(0.0f, 170.0f, 0.0f));
	vw_CameraLookAt();

	Weapon->Draw(false);

	vw_SetCameraLocation(VECTOR3D(-50,30,-50));
	vw_ResizeScene(45.0f, Setup.fAspectRatioWidth/Setup.fAspectRatioHeight, 1.0f, 2000.0f);
	vw_SetViewport(x, y, width, height, znear, zfar);
}

