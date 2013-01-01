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


#include "../Game.h"



extern CEarthSpaceFighter *WorkshopFighterGame;
extern CEarthSpaceFighter *WorkshopNewFighter;
extern int	CurrentWorkshopNewFighter;
extern int	CurrentWorkshopNewWeapon;
extern float CurrentAlert2;
extern float CurrentAlert3;

void WorkshopDrawShip(CEarthSpaceFighter *SpaceFighter, int Mode);
void WorkshopCreateNewShip();
int GetSystemCost(int Num);
int GetWeaponCost(int Num, int Ammo, int AmmoStart);
int GetWeaponBaseCost(int Num);

// что рисовать в диалоге 6,7,8
extern CSpaceShip *DialogSpaceShip;






//------------------------------------------------------------------------------------
// название корабля
//------------------------------------------------------------------------------------
const char *GetWorkshopShipName(int Num)
{
	switch (Num)
	{
		case 1: return "7_ship1";
		case 2: return "7_ship2";
		case 3: return "7_ship3";
		case 4: return "7_ship4";
		case 5: return "7_ship5";
		case 6: return "7_ship6";
		case 7: return "7_ship7";
		case 8: return "7_ship8";
		case 9: return "7_ship9";
		case 10: return "7_ship10";
		case 11: return "7_ship11";
		case 12: return "7_ship12";
		case 13: return "7_ship13";
		case 14: return "7_ship14";
		case 15: return "7_ship15";
		case 16: return "7_ship16";
		case 17: return "7_ship17";
		case 18: return "7_ship18";
		case 19: return "7_ship19";
		case 20: return "7_ship20";
		case 21: return "7_ship21";
		case 22: return "7_ship22";
	}

	return 0;
}




//------------------------------------------------------------------------------------
// тип корабля
//------------------------------------------------------------------------------------
const char *GetShipGroupTitle(int Num)
{
	switch (Num)
	{
		case 1:
		case 4:
		case 6:
		case 10:
		case 12:
		case 14:
		case 15:
		case 17:
		case 20:
			return "4_Fighter";
		case 2:
		case 5:
		case 19:
			return "4_Interceptor";
		case 3:
		case 16:
		case 18:
			return "4_Bomber";
		case 7:
		case 13:
			return "4_Attack_Ship";
		case 8:
		case 9:
		case 11:
		case 21:
		case 22:
			return "4_Scout";
	}

	return 0;
}



//------------------------------------------------------------------------------------
// стоимость корпуса
//------------------------------------------------------------------------------------
int GetWorkshopShipCost(int Num)
{
	switch (Num)
	{
		case 1: return 1100;
		case 2: return 900;
		case 3: return 1500;
		case 4: return 1300;
		case 5: return 1350;
		case 6: return 2400;
		case 7: return 3000;
		case 8: return 3500;
		case 9: return 4000;
		case 10: return 6000;
		case 11: return 4500;
		case 12: return 5000;
		case 13: return 9000;
		case 14: return 7500;
		case 15: return 7000;
		case 16: return 9000;
		case 17: return 10000;
		case 18: return 12000;
		case 19: return 5000;
		case 20: return 9000;
		case 21: return 10000;
		case 22: return 8000;
	}

	return 0;
}






int GetWorkshopShipRepairCost(int Num, CEarthSpaceFighter *Fighter)
{
	int ShipCost = GetWorkshopShipCost(Num)*Setup.Profile[CurrentProfile].ShipHullUpgrade;
	// расчет стоимости ремонта корабля
	return ShipCost - (int)(ShipCost*(Fighter->Strength/Fighter->StrengthStart));
}






int GetWorkshopShipFullCost(int Num, CEarthSpaceFighter *Fighter)
{
	// полная стоимость корпуса корабля с повреждениями корабля
	int ShipCost = GetWorkshopShipCost(Num)* Setup.Profile[CurrentProfile].ShipHullUpgrade -
		GetWorkshopShipRepairCost(Num, Fighter);


	// прибавить стоимость оружия
	for (int i=0; i<WorkshopFighterGame->WeaponQuantity; i++)
	if (WorkshopFighterGame->Weapon[i] != 0)
	{
		ShipCost += GetWeaponCost(WorkshopFighterGame->Weapon[i]->ObjectCreationType,
			WorkshopFighterGame->Weapon[i]->Ammo,
			WorkshopFighterGame->Weapon[i]->AmmoStart);
	}

	// прибавить стоимость систем
	if (Setup.Profile[CurrentProfile].EngineSystem != 0)
		ShipCost += GetSystemCost(Setup.Profile[CurrentProfile].EngineSystem);
	if (Setup.Profile[CurrentProfile].PowerSystem != 0)
		ShipCost += GetSystemCost(Setup.Profile[CurrentProfile].PowerSystem+4);
	if (Setup.Profile[CurrentProfile].TargetingSystem != 0)
		ShipCost += GetSystemCost(Setup.Profile[CurrentProfile].TargetingSystem+8);
	if (Setup.Profile[CurrentProfile].TargetingMechanicSystem != 0)
		ShipCost += GetSystemCost(Setup.Profile[CurrentProfile].TargetingMechanicSystem+12);
	if (Setup.Profile[CurrentProfile].AdvancedProtectionSystem != 0)
		ShipCost += GetSystemCost(Setup.Profile[CurrentProfile].AdvancedProtectionSystem+16);

	return ShipCost;
}









//------------------------------------------------------------------------------------
// создание корабля игрока при покупке нового корпуса
//------------------------------------------------------------------------------------
void WorkshopCreateBuyShip()
{
	// продаем все оружие и все системы
	// 1 - Системы
	int EngineSystem = Setup.Profile[CurrentProfile].EngineSystem;
	if (Setup.Profile[CurrentProfile].EngineSystem != 0)
		Setup.Profile[CurrentProfile].Money += GetSystemCost(Setup.Profile[CurrentProfile].EngineSystem);
	Setup.Profile[CurrentProfile].EngineSystem = 0;

	int PowerSystem = Setup.Profile[CurrentProfile].PowerSystem;
	if (Setup.Profile[CurrentProfile].PowerSystem != 0)
		Setup.Profile[CurrentProfile].Money += GetSystemCost(Setup.Profile[CurrentProfile].PowerSystem+4);
	Setup.Profile[CurrentProfile].PowerSystem = 0;

	int TargetingSystem = Setup.Profile[CurrentProfile].TargetingSystem;
	if (Setup.Profile[CurrentProfile].TargetingSystem != 0)
		Setup.Profile[CurrentProfile].Money += GetSystemCost(Setup.Profile[CurrentProfile].TargetingSystem+8);
	Setup.Profile[CurrentProfile].TargetingSystem = 0;

	int TargetingMechanicSystem = Setup.Profile[CurrentProfile].TargetingMechanicSystem;
	if (Setup.Profile[CurrentProfile].TargetingMechanicSystem != 0)
		Setup.Profile[CurrentProfile].Money += GetSystemCost(Setup.Profile[CurrentProfile].TargetingMechanicSystem+12);
	Setup.Profile[CurrentProfile].TargetingMechanicSystem = 0;

	int AdvancedProtectionSystem = Setup.Profile[CurrentProfile].AdvancedProtectionSystem;
	if (Setup.Profile[CurrentProfile].AdvancedProtectionSystem != 0)
		Setup.Profile[CurrentProfile].Money += GetSystemCost(Setup.Profile[CurrentProfile].AdvancedProtectionSystem+16);
	Setup.Profile[CurrentProfile].AdvancedProtectionSystem = 0;

	// 2 - Оружие
	int OldWeaponQuantity = WorkshopFighterGame->WeaponQuantity;
	for (int i=0; i<WorkshopFighterGame->WeaponQuantity; i++)
	if (WorkshopFighterGame->Weapon[i] != 0)
	{
		Setup.Profile[CurrentProfile].Money += GetWeaponCost(WorkshopFighterGame->Weapon[i]->ObjectCreationType,
															WorkshopFighterGame->Weapon[i]->Ammo,
															WorkshopFighterGame->Weapon[i]->AmmoStart);
		delete WorkshopFighterGame->Weapon[i]; WorkshopFighterGame->Weapon[i] = 0;
	}

	// 3 - корпус
	Setup.Profile[CurrentProfile].Money +=
		GetWorkshopShipCost(Setup.Profile[CurrentProfile].Ship)*Setup.Profile[CurrentProfile].ShipHullUpgrade -
		GetWorkshopShipRepairCost(Setup.Profile[CurrentProfile].Ship, WorkshopFighterGame);


	// сейчас у нас все в "деньгах", покупаем корпус
	Setup.Profile[CurrentProfile].Ship = CurrentWorkshopNewFighter;
	Setup.Profile[CurrentProfile].Money -= GetWorkshopShipCost(Setup.Profile[CurrentProfile].Ship);



	// создаем объект
	if (WorkshopFighterGame != 0){delete WorkshopFighterGame; WorkshopFighterGame = 0;}

	int TMPGameNPCArmorPenalty = GameNPCArmorPenalty;
	GameNPCArmorPenalty = 1;

	WorkshopFighterGame = new CEarthSpaceFighter;
	WorkshopFighterGame->Create(Setup.Profile[CurrentProfile].Ship);
	WorkshopFighterGame->ID = 1000;
	WorkshopFighterGame->EngineDestroyType = true;
	WorkshopFighterGame->ShowStrength = false;
	Setup.Profile[CurrentProfile].ShipHullCurrentStrength = WorkshopFighterGame->Strength;
	Setup.Profile[CurrentProfile].ShipHullUpgrade = 1;


	// пытаемся купить

	// 1 - Системы
	if (EngineSystem != 0)
	if (Setup.Profile[CurrentProfile].Money >= GetSystemCost(EngineSystem))
	{
		Setup.Profile[CurrentProfile].Money -= GetSystemCost(EngineSystem);
		Setup.Profile[CurrentProfile].EngineSystem = EngineSystem;
	}
	if (PowerSystem != 0)
	if (Setup.Profile[CurrentProfile].Money >= GetSystemCost(PowerSystem+4))
	{
		Setup.Profile[CurrentProfile].Money -= GetSystemCost(PowerSystem+4);
		Setup.Profile[CurrentProfile].PowerSystem = PowerSystem;
	}
	if (TargetingSystem != 0)
	if (Setup.Profile[CurrentProfile].Money >= GetSystemCost(TargetingSystem+8))
	{
		Setup.Profile[CurrentProfile].Money -= GetSystemCost(TargetingSystem+8);
		Setup.Profile[CurrentProfile].TargetingSystem = TargetingSystem;
	}
	if (TargetingMechanicSystem != 0)
	if (Setup.Profile[CurrentProfile].Money >= GetSystemCost(TargetingMechanicSystem+12))
	{
		Setup.Profile[CurrentProfile].Money -= GetSystemCost(TargetingMechanicSystem+12);
		Setup.Profile[CurrentProfile].TargetingMechanicSystem = TargetingMechanicSystem;
	}
	if (AdvancedProtectionSystem != 0)
	if (Setup.Profile[CurrentProfile].Money >= GetSystemCost(AdvancedProtectionSystem+16))
	{
		Setup.Profile[CurrentProfile].Money -= GetSystemCost(AdvancedProtectionSystem+16);
		Setup.Profile[CurrentProfile].AdvancedProtectionSystem = AdvancedProtectionSystem;
	}



	// покупаем оружие, если можем... нет - ставим что продали его (это сделали ранее)
	// всегда покупаем новое, т.е. с полным боекомплектом!
	for (int i=0; i<WorkshopFighterGame->WeaponQuantity; i++)
	{
		if (Setup.Profile[CurrentProfile].Weapon[i] != 0)
		{
			if (Setup.Profile[CurrentProfile].Money >= GetWeaponBaseCost(Setup.Profile[CurrentProfile].Weapon[i]))
			{
				if (SetEarthSpaceFighterWeapon(WorkshopFighterGame, i+1, Setup.Profile[CurrentProfile].Weapon[i]))
				{
					if (WorkshopFighterGame->Weapon[i]->Fire != 0)
					if (WorkshopFighterGame->Weapon[i]->Fire->Light != 0){vw_ReleaseLight(WorkshopFighterGame->Weapon[i]->Fire->Light); WorkshopFighterGame->Weapon[i]->Fire->Light = 0;}

					Setup.Profile[CurrentProfile].Money -= GetWeaponBaseCost(Setup.Profile[CurrentProfile].Weapon[i]);
					Setup.Profile[CurrentProfile].WeaponAmmo[i] = WorkshopFighterGame->Weapon[i]->AmmoStart;
					Setup.Profile[CurrentProfile].WeaponSlotYAngle[i] = 0.0f;
				}
				else
				// если поставить не смогли - не такой слот
				{
					Setup.Profile[CurrentProfile].Weapon[i] = 0;
					Setup.Profile[CurrentProfile].WeaponAmmo[i] = 0;
				}
			}// если не хватило денег снова купить
			else
			{
				Setup.Profile[CurrentProfile].Weapon[i] = 0;
				Setup.Profile[CurrentProfile].WeaponAmmo[i] = 0;
			}
		}
	}
	// если было больше слотов чем есть сейчас
	if (OldWeaponQuantity > WorkshopFighterGame->WeaponQuantity)
	for (int i=0; i<OldWeaponQuantity; i++)
	if (WorkshopFighterGame->WeaponQuantity <= i) Setup.Profile[CurrentProfile].Weapon[i] = 0;




	// создаем системы (визуальные)
	SetEarthSpaceFighterEngine(WorkshopFighterGame, Setup.Profile[CurrentProfile].EngineSystem);
	if (Setup.Profile[CurrentProfile].AdvancedProtectionSystem == 2)
		SetEarthSpaceFighterArmour(WorkshopFighterGame, 7);
	else
		SetEarthSpaceFighterArmour(WorkshopFighterGame, Setup.Profile[CurrentProfile].ShipHullUpgrade-1);

	GameNPCArmorPenalty = TMPGameNPCArmorPenalty;
	WorkshopFighterGame->SetLocation(VECTOR3D(1000,-1000-(WorkshopFighterGame->Height/2.0f + WorkshopFighterGame->AABB[6].y), -(WorkshopFighterGame->Length/2.0f + WorkshopFighterGame->AABB[6].z)));
	// чтобы оружие заняло свое место...
	WorkshopFighterGame->Update(vw_GetTime());

	WorkshopFighterGame->SetRotation(VECTOR3D(0.0f,150.0f,0.0f));
}





void BuyShip()
{
	// сбрасываем особенные настройки слотов оружия
	for (int i=0; i<WorkshopFighterGame->WeaponQuantity; i++)
	{
		Setup.Profile[CurrentProfile].WeaponSlotYAngle[i] = 0;
	}

	// создаем новый корабль
	WorkshopCreateBuyShip();

	// переводим на другой корабль
	CurrentWorkshopNewFighter ++;
	if (CurrentWorkshopNewFighter == Setup.Profile[CurrentProfile].Ship) CurrentWorkshopNewFighter ++;
	if (CurrentWorkshopNewFighter > 22) CurrentWorkshopNewFighter = 1;
	// если это 1-й, переводим на 2-й
	if (CurrentWorkshopNewFighter == Setup.Profile[CurrentProfile].Ship) CurrentWorkshopNewFighter ++;
	WorkshopCreateNewShip();
}






void RepairShip()
{
	// смотрим, если денег достаточно для полного ремонта - делаем его
	if (Setup.Profile[CurrentProfile].Money >= GetWorkshopShipRepairCost(Setup.Profile[CurrentProfile].Ship, WorkshopFighterGame))
	{
		Setup.Profile[CurrentProfile].Money -= GetWorkshopShipRepairCost(Setup.Profile[CurrentProfile].Ship, WorkshopFighterGame);
		Setup.Profile[CurrentProfile].ShipHullCurrentStrength = WorkshopFighterGame->StrengthStart;
		WorkshopFighterGame->Strength = Setup.Profile[CurrentProfile].ShipHullCurrentStrength;
		return;
	}


	// вычисляем, сколько можем отремонтировать
	float StrengtRepair = WorkshopFighterGame->StrengthStart - WorkshopFighterGame->Strength;
	float MoneyRepair = 1.0f*GetWorkshopShipRepairCost(Setup.Profile[CurrentProfile].Ship, WorkshopFighterGame);
	// сколько можем отремонтировать
	float CanRepair = StrengtRepair * (Setup.Profile[CurrentProfile].Money/MoneyRepair);
	// ремонтируем сколько можем
	WorkshopFighterGame->Strength += CanRepair;
	Setup.Profile[CurrentProfile].ShipHullCurrentStrength = WorkshopFighterGame->Strength;
	// это будет стоить все имеющиеся деньги
	Setup.Profile[CurrentProfile].Money = 0;

	// выводим диалог с надписью, что не достаточно денег для полного ремонта
	SetCurrentDialogBox(3);
}





void UpgradeShip()
{
	// ув. данные о базовой прочности корабля
	float OldStr = WorkshopFighterGame->StrengthStart;
	WorkshopFighterGame->StrengthStart /= Setup.Profile[CurrentProfile].ShipHullUpgrade;

	// ув. данные о номере апгрейда
	Setup.Profile[CurrentProfile].ShipHullUpgrade ++;

	WorkshopFighterGame->StrengthStart *= Setup.Profile[CurrentProfile].ShipHullUpgrade;
	WorkshopFighterGame->Strength = WorkshopFighterGame->StrengthStart - OldStr +
		Setup.Profile[CurrentProfile].ShipHullCurrentStrength;

	Setup.Profile[CurrentProfile].ShipHullCurrentStrength = WorkshopFighterGame->Strength;


	// вычитаем деньги за апгрейд
	Setup.Profile[CurrentProfile].Money -= GetWorkshopShipCost(Setup.Profile[CurrentProfile].Ship);


	// сообщаем о новых возможностях и т.п. которые дает этот апгрейд
	SetEarthSpaceFighterArmour(WorkshopFighterGame, Setup.Profile[CurrentProfile].ShipHullUpgrade-1);

}

















//------------------------------------------------------------------------------------
// покупка-апгрейд корпуса
//------------------------------------------------------------------------------------
void Workshop_Shipyard()
{

	// затемнение при выводе
	int SizeI;
	RECT SrcRect, DstRect;


	// затемнение
	SetRect(&SrcRect,0,0,256,256 );
	SetRect(&DstRect,Setup.iAspectRatioWidth/2-480,100-32,Setup.iAspectRatioWidth/2-32,450+32);
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/back_spot2.tga"), true, 0.45f*MenuContentTransp);
	SetRect(&DstRect,Setup.iAspectRatioWidth/2+32,100-32,Setup.iAspectRatioWidth/2+480,450+32);
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/back_spot2.tga"), true, 0.45f*MenuContentTransp);



	vw_End2DMode();
	WorkshopDrawShip(WorkshopFighterGame, 3);
	WorkshopDrawShip(WorkshopNewFighter, 2);
	vw_Start2DMode(-1,1);


	// условия, при которых корабль купить можно
	bool CanBuy = false;
	if (GetWorkshopShipFullCost(Setup.Profile[CurrentProfile].Ship, WorkshopFighterGame) +
		Setup.Profile[CurrentProfile].Money >= GetWorkshopShipCost(CurrentWorkshopNewFighter))
		CanBuy = true;


	// на новом корабле
	vw_SetFontSize(24);
	vw_SetFontOffsetY(2);
	vw_DrawFont(Setup.iAspectRatioWidth/2-445, 600, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("7_Ships_Stock"));
	vw_SetFontSize(Setup.FontSize);
	vw_SetFontOffsetY(8 - ceil(Setup.FontSize/2.0f) + 2);


	// вывод названия корабля
	vw_DrawFont(Setup.iAspectRatioWidth/2-448, 50+6, 0, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("7_shipfullname"), vw_GetText(GetWorkshopShipName(CurrentWorkshopNewFighter)), 1);
	if (DrawButton128_2(Setup.iAspectRatioWidth/2-197,50, vw_GetText("1_Info"), MenuContentTransp, false))
	{
		SetCurrentDialogBox(6);
		DialogSpaceShip = WorkshopNewFighter;
	}


	// находим смещение, чтобы было красиво
	int SmSizeI = vw_FontSize(vw_GetText("4_Armor:"));
	int SmSizeI2 = vw_FontSize(vw_GetText("4_Weapons"));
	if (SmSizeI < SmSizeI2) SmSizeI = SmSizeI2;

	vw_DrawFont(Setup.iAspectRatioWidth/2-78-40-SmSizeI, 110, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*MenuContentTransp, vw_GetText("4_Armor:"));
	vw_DrawFont(Setup.iAspectRatioWidth/2-78-vw_FontSize("%i", (int)WorkshopNewFighter->StrengthStart), 110, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*MenuContentTransp, "%i", (int)WorkshopNewFighter->StrengthStart);

	vw_DrawFont(Setup.iAspectRatioWidth/2-78-40-SmSizeI, 130, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*MenuContentTransp, "%s:", vw_GetText("4_Weapons"));
	vw_DrawFont(Setup.iAspectRatioWidth/2-78-vw_FontSize("%i", WorkshopNewFighter->WeaponQuantity), 130, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*MenuContentTransp, "%i", WorkshopNewFighter->WeaponQuantity);

	// вывод стоимости корабля
	if (CanBuy)
		vw_DrawFont(Setup.iAspectRatioWidth/2-438, 420, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%s: %i", vw_GetText("7_Ship_Hull_Cost"), GetWorkshopShipCost(CurrentWorkshopNewFighter));
	else
		vw_DrawFont(Setup.iAspectRatioWidth/2-438, 420, 0, 0, 1.0f, 1.0f,0.5f,0.0f, CurrentAlert3*MenuContentTransp, "%s: %i", vw_GetText("7_Ship_Hull_Cost"), GetWorkshopShipCost(CurrentWorkshopNewFighter));



	// рамки
	SetRect(&SrcRect,0,0,400,35 );
	SetRect(&DstRect,Setup.iAspectRatioWidth/2-457,100-11,Setup.iAspectRatioWidth/2-57,100+35-11);
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/workshop_panel4.tga"), true, MenuContentTransp);

	SetRect(&SrcRect,0,0,400,173 );
	SetRect(&DstRect,Setup.iAspectRatioWidth/2-457,450-13,Setup.iAspectRatioWidth/2-57,450+173-13);
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/workshop_panel1.tga"), true, MenuContentTransp);


	if (DrawButton128_2(Setup.iAspectRatioWidth/2-395,482, vw_GetText("1_Prev"), MenuContentTransp, false))
	{
		CurrentWorkshopNewFighter --;
		if (CurrentWorkshopNewFighter < 1) CurrentWorkshopNewFighter = 22;
		WorkshopCreateNewShip();
	}
	if (DrawButton128_2(Setup.iAspectRatioWidth/2-247,482, vw_GetText("1_Next"), MenuContentTransp, false))
	{
		CurrentWorkshopNewFighter ++;
		if (CurrentWorkshopNewFighter > 22) CurrentWorkshopNewFighter = 1;
		WorkshopCreateNewShip();
	}
	if (DrawButton200_2(Setup.iAspectRatioWidth/2-357,533, vw_GetText("1_Buy_Ship"), MenuContentTransp, !CanBuy))
	{
		BuyShip();
	}








	// на корабле игрока
	vw_SetFontSize(24);
	vw_SetFontOffsetY(2);
	vw_DrawFont(Setup.iAspectRatioWidth/2+445-vw_FontSize(vw_GetText("7_Player_Ship")), 600, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("7_Player_Ship"));
	vw_SetFontSize(Setup.FontSize);
	vw_SetFontOffsetY(8 - ceil(Setup.FontSize/2.0f) + 2);

	// вывод названия корабля
	vw_DrawFont(Setup.iAspectRatioWidth/2+64, 56, 0, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("7_shipfullname"), vw_GetText(GetWorkshopShipName(Setup.Profile[CurrentProfile].Ship)), Setup.Profile[CurrentProfile].ShipHullUpgrade);

	if (DrawButton128_2(Setup.iAspectRatioWidth/2+315, 50, vw_GetText("1_Info"), MenuContentTransp, false))
	{
		SetCurrentDialogBox(6);
		DialogSpaceShip = WorkshopFighterGame;
	}





	int LinePos = 420;

	// вывод стомости модернизации
	// модернизация корпуса
	bool CanUpgrade = false;
	if (Setup.Profile[CurrentProfile].ShipHullUpgrade<4) CanUpgrade = true;

	if (CanUpgrade)
	{
		if (Setup.Profile[CurrentProfile].Money < GetWorkshopShipCost(Setup.Profile[CurrentProfile].Ship)) CanUpgrade = false;
		if (CanUpgrade)
			vw_DrawFont(Setup.iAspectRatioWidth/2+438-vw_FontSize("%s: %i", vw_GetText("7_Upgrade_Hull_Cost"), GetWorkshopShipCost(Setup.Profile[CurrentProfile].Ship)), LinePos, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%s: %i", vw_GetText("7_Upgrade_Hull_Cost"), GetWorkshopShipCost(Setup.Profile[CurrentProfile].Ship));
		else
			vw_DrawFont(Setup.iAspectRatioWidth/2+438-vw_FontSize("%s: %i", vw_GetText("7_Upgrade_Hull_Cost"), GetWorkshopShipCost(Setup.Profile[CurrentProfile].Ship)), LinePos, 0, 0, 1.0f, 1.0f,0.5f,0.0f, CurrentAlert3*MenuContentTransp, "%s: %i", vw_GetText("7_Upgrade_Hull_Cost"), GetWorkshopShipCost(Setup.Profile[CurrentProfile].Ship));

		LinePos -= 20;
	}


	// вывод стомости ремонта, если нужно
	bool NeedRepair = false;
	if (GetWorkshopShipRepairCost(Setup.Profile[CurrentProfile].Ship, WorkshopFighterGame) > 0)
	{
		SizeI = vw_FontSize("%s: %i", vw_GetText("7_Repair_Hull_Cost"), GetWorkshopShipRepairCost(Setup.Profile[CurrentProfile].Ship, WorkshopFighterGame));
		vw_DrawFont(Setup.iAspectRatioWidth/2+438-SizeI, LinePos, 0, 0, 1.0f, 1.0f,0.0f,0.0f, CurrentAlert3*MenuContentTransp, "%s: %i", vw_GetText("7_Repair_Hull_Cost"), GetWorkshopShipRepairCost(Setup.Profile[CurrentProfile].Ship, WorkshopFighterGame));
		LinePos -= 20;
		NeedRepair = true;

		// надпись Armor, красная
		vw_DrawFont(Setup.iAspectRatioWidth/2+74, 110, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*MenuContentTransp, vw_GetText("4_Armor:"));
		SizeI = vw_FontSize("%i/%i", (int)WorkshopFighterGame->Strength, (int)WorkshopFighterGame->StrengthStart);
		vw_DrawFont(Setup.iAspectRatioWidth/2+74+14+SmSizeI, 110, 0, 0, 1.0f, 1.0f,0.0f,0.0f, CurrentAlert3*MenuContentTransp, "%i/%i", (int)WorkshopFighterGame->Strength, (int)WorkshopFighterGame->StrengthStart);
	}
	else
	{	// надпись Armor, нормальная
		vw_DrawFont(Setup.iAspectRatioWidth/2+74, 110, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*MenuContentTransp, vw_GetText("4_Armor:"));
		SizeI = vw_FontSize("%i/%i", (int)WorkshopFighterGame->Strength, (int)WorkshopFighterGame->StrengthStart);
		vw_DrawFont(Setup.iAspectRatioWidth/2+74+14+SmSizeI, 110, 0, 0,1.0f, 1.0f,1.0f,1.0f, 0.5f*MenuContentTransp, "%i/%i", (int)WorkshopFighterGame->Strength, (int)WorkshopFighterGame->StrengthStart);
	}

	SizeI = SizeI - vw_FontSize("%i", WorkshopFighterGame->WeaponQuantity);
	vw_DrawFont(Setup.iAspectRatioWidth/2+74, 130, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*MenuContentTransp, "%s:", vw_GetText("4_Weapons"));
	vw_DrawFont(Setup.iAspectRatioWidth/2+74+14+SmSizeI+SizeI, 130, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*MenuContentTransp, "%i", WorkshopFighterGame->WeaponQuantity);




	// вывод стоимости корабля (со всеми системами)
	SizeI = vw_FontSize("%s: %i", vw_GetText("7_Ship_Cost"), GetWorkshopShipFullCost(Setup.Profile[CurrentProfile].Ship, WorkshopFighterGame));
	vw_DrawFont(Setup.iAspectRatioWidth/2+438-SizeI, LinePos, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%s: %i", vw_GetText("7_Ship_Cost"), GetWorkshopShipFullCost(Setup.Profile[CurrentProfile].Ship, WorkshopFighterGame));


	// рамки
	SetRect(&SrcRect,0,0,400,35 );
	SetRect(&DstRect,Setup.iAspectRatioWidth/2+57,100-11,Setup.iAspectRatioWidth/2+457,100+35-11);
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/workshop_panel4.tga"), true, MenuContentTransp);

	SetRect(&SrcRect,0,0,400,173 );
	SetRect(&DstRect,Setup.iAspectRatioWidth/2+57,450-13,Setup.iAspectRatioWidth/2+457,450+173-13);
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/workshop_panel1+.tga"), true, MenuContentTransp);



	if (DrawButton200_2(Setup.iAspectRatioWidth/2+153, 482, vw_GetText("1_Repair_Hull"), MenuContentTransp, !NeedRepair))
	{
		RepairShip();
	}


	if (DrawButton200_2(Setup.iAspectRatioWidth/2+153, 533, vw_GetText("1_Upgrade_Hull"), MenuContentTransp, !CanUpgrade))
	{
		UpgradeShip();
	}


	// вывод информации
	vw_SetFontSize(20);
	vw_SetFontOffsetY(2);
	SizeI = (Setup.iAspectRatioWidth-vw_FontSize("%s: %i", vw_GetText("3_Money"), Setup.Profile[CurrentProfile].Money))/2;
	if (CanBuy)
		vw_DrawFont(SizeI, 630, 0, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, "%s: %i", vw_GetText("3_Money"), Setup.Profile[CurrentProfile].Money);
	else
		vw_DrawFont(SizeI, 630, 0, 0, 1.0f, 1.0f,0.5f,0.0f, CurrentAlert3*MenuContentTransp, "%s: %i", vw_GetText("3_Money"), Setup.Profile[CurrentProfile].Money);
	vw_SetFontSize(Setup.FontSize);
	vw_SetFontOffsetY(8 - ceil(Setup.FontSize/2.0f) + 2);
}


