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
#include "../ui/font.h"
#include "../object3d/space_ship/space_ship.h"
#include "../object3d/space_ship/earth_space_fighter/earth_space_fighter.h"


extern cEarthSpaceFighter *WorkshopFighterGame;
extern cEarthSpaceFighter *WorkshopNewFighter;
extern int	CurrentWorkshopNewFighter;
extern int	CurrentWorkshopNewWeapon;
extern float CurrentAlert2;
extern float CurrentAlert3;

void WorkshopDrawShip(cEarthSpaceFighter *SpaceFighter, int Mode);
void WorkshopCreateNewShip();
int GetSystemCost(int Num);
int GetWeaponCost(int Num, int Ammo, int AmmoStart);
int GetWeaponBaseCost(int Num);

// что рисовать в диалоге 6,7,8
extern cSpaceShip *DialogSpaceShip;






//------------------------------------------------------------------------------------
// название корабля
//------------------------------------------------------------------------------------
const char *GetWorkshopShipName(int Num)
{
	switch (Num) {
	case 1:
		return "Cobra";
	case 2:
		return "SpaceEagle";
	case 3:
		return "FireStorm";
	case 4:
		return "Hedgehog";
	case 5:
		return "Thunder";
	case 6:
		return "Excalibur";
	case 7:
		return "DarkStar";
	case 8:
		return "Scorpion";
	case 9:
		return "Runner";
	case 10:
		return "Kronos";
	case 11:
		return "Chameleon";
	case 12:
		return "Falcon";
	case 13:
		return "DeathRipple";
	case 14:
		return "AstroSword";
	case 15:
		return "Pulser";
	case 16:
		return "ClumsyMace";
	case 17:
		return "Raptor";
	case 18:
		return "Destroyer";
	case 19:
		return "AstroSpear";
	case 20:
		return "Mite";
	case 21:
		return "Viper";
	case 22:
		return "BlackIce";

	default:
		std::cerr << __func__ << "(): " << "wrong Num.\n";
		break;
	}

	return nullptr;
}




//------------------------------------------------------------------------------------
// тип корабля
//------------------------------------------------------------------------------------
const char *GetShipGroupTitle(int Num)
{
	switch (Num) {
	case 1:
	case 4:
	case 6:
	case 10:
	case 12:
	case 14:
	case 15:
	case 17:
	case 20:
		return "Fighter";
	case 2:
	case 5:
	case 19:
		return "Interceptor";
	case 3:
	case 16:
	case 18:
		return "Bomber";
	case 7:
	case 13:
		return "Attack Ship";
	case 8:
	case 9:
	case 11:
	case 21:
	case 22:
		return "Scout";

	default:
		std::cerr << __func__ << "(): " << "wrong Num.\n";
		break;
	}

	return nullptr;
}



//------------------------------------------------------------------------------------
// стоимость корпуса
//------------------------------------------------------------------------------------
int GetWorkshopShipCost(int Num)
{
	switch (Num) {
	case 1:
		return 1100;
	case 2:
		return 900;
	case 3:
		return 1500;
	case 4:
		return 1300;
	case 5:
		return 1350;
	case 6:
		return 2400;
	case 7:
		return 3000;
	case 8:
		return 3500;
	case 9:
		return 4000;
	case 10:
		return 6000;
	case 11:
		return 4500;
	case 12:
		return 5000;
	case 13:
		return 9000;
	case 14:
		return 7500;
	case 15:
		return 7000;
	case 16:
		return 9000;
	case 17:
		return 10000;
	case 18:
		return 12000;
	case 19:
		return 5000;
	case 20:
		return 9000;
	case 21:
		return 10000;
	case 22:
		return 8000;

	default:
		std::cerr << __func__ << "(): " << "wrong Num.\n";
		break;
	}

	return 0;
}






int GetWorkshopShipRepairCost(int Num, cEarthSpaceFighter *Fighter)
{
	int ShipCost = GetWorkshopShipCost(Num) * GameConfig().Profile[CurrentProfile].ShipHullUpgrade;
	// расчет стоимости ремонта корабля
	return ShipCost - (int)(ShipCost*(Fighter->Strength/Fighter->StrengthStart));
}






int GetWorkshopShipFullCost(int Num, cEarthSpaceFighter *Fighter)
{
	// полная стоимость корпуса корабля с повреждениями корабля
	int ShipCost = GetWorkshopShipCost(Num) * GameConfig().Profile[CurrentProfile].ShipHullUpgrade -
		       GetWorkshopShipRepairCost(Num, Fighter);


	// прибавить стоимость оружия
	for (int i = 0; i < WorkshopFighterGame->WeaponQuantity; i++)
		if (WorkshopFighterGame->Weapon[i] != nullptr) {
			ShipCost += GetWeaponCost(WorkshopFighterGame->Weapon[i]->ObjectCreationType,
						  WorkshopFighterGame->Weapon[i]->Ammo,
						  WorkshopFighterGame->Weapon[i]->AmmoStart);
		}

	// прибавить стоимость систем
	if (GameConfig().Profile[CurrentProfile].EngineSystem)
		ShipCost += GetSystemCost(GameConfig().Profile[CurrentProfile].EngineSystem);
	if (GameConfig().Profile[CurrentProfile].PowerSystem)
		ShipCost += GetSystemCost(GameConfig().Profile[CurrentProfile].PowerSystem + 4);
	if (GameConfig().Profile[CurrentProfile].TargetingSystem)
		ShipCost += GetSystemCost(GameConfig().Profile[CurrentProfile].TargetingSystem + 8);
	if (GameConfig().Profile[CurrentProfile].TargetingMechanicSystem)
		ShipCost += GetSystemCost(GameConfig().Profile[CurrentProfile].TargetingMechanicSystem + 12);
	if (GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem)
		ShipCost += GetSystemCost(GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem + 16);

	return ShipCost;
}









//------------------------------------------------------------------------------------
// создание корабля игрока при покупке нового корпуса
//------------------------------------------------------------------------------------
void WorkshopCreateBuyShip()
{
	// продаем все оружие и все системы
	// 1 - Системы
	int EngineSystem = GameConfig().Profile[CurrentProfile].EngineSystem;
	if (GameConfig().Profile[CurrentProfile].EngineSystem)
		ChangeGameConfig().Profile[CurrentProfile].Money += GetSystemCost(GameConfig().Profile[CurrentProfile].EngineSystem);
	ChangeGameConfig().Profile[CurrentProfile].EngineSystem = 0;

	int PowerSystem = GameConfig().Profile[CurrentProfile].PowerSystem;
	if (GameConfig().Profile[CurrentProfile].PowerSystem)
		ChangeGameConfig().Profile[CurrentProfile].Money += GetSystemCost(GameConfig().Profile[CurrentProfile].PowerSystem+4);
	ChangeGameConfig().Profile[CurrentProfile].PowerSystem = 0;

	int TargetingSystem = GameConfig().Profile[CurrentProfile].TargetingSystem;
	if (GameConfig().Profile[CurrentProfile].TargetingSystem)
		ChangeGameConfig().Profile[CurrentProfile].Money += GetSystemCost(GameConfig().Profile[CurrentProfile].TargetingSystem+8);
	ChangeGameConfig().Profile[CurrentProfile].TargetingSystem = 0;

	int TargetingMechanicSystem = GameConfig().Profile[CurrentProfile].TargetingMechanicSystem;
	if (GameConfig().Profile[CurrentProfile].TargetingMechanicSystem)
		ChangeGameConfig().Profile[CurrentProfile].Money += GetSystemCost(GameConfig().Profile[CurrentProfile].TargetingMechanicSystem+12);
	ChangeGameConfig().Profile[CurrentProfile].TargetingMechanicSystem = 0;

	int AdvancedProtectionSystem = GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem;
	if (GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem)
		ChangeGameConfig().Profile[CurrentProfile].Money += GetSystemCost(GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem+16);
	ChangeGameConfig().Profile[CurrentProfile].AdvancedProtectionSystem = 0;

	// 2 - Оружие
	int OldWeaponQuantity = WorkshopFighterGame->WeaponQuantity;
	for (int i = 0; i < WorkshopFighterGame->WeaponQuantity; i++)
		if (WorkshopFighterGame->Weapon[i] != nullptr) {
			ChangeGameConfig().Profile[CurrentProfile].Money += GetWeaponCost(
									    WorkshopFighterGame->Weapon[i]->ObjectCreationType,
									    WorkshopFighterGame->Weapon[i]->Ammo,
									    WorkshopFighterGame->Weapon[i]->AmmoStart);
			delete WorkshopFighterGame->Weapon[i];
			WorkshopFighterGame->Weapon[i] = nullptr;
		}

	// 3 - корпус
	ChangeGameConfig().Profile[CurrentProfile].Money +=
		GetWorkshopShipCost(ChangeGameConfig().Profile[CurrentProfile].Ship) *
		GameConfig().Profile[CurrentProfile].ShipHullUpgrade -
		GetWorkshopShipRepairCost(ChangeGameConfig().Profile[CurrentProfile].Ship, WorkshopFighterGame);


	// сейчас у нас все в "деньгах", покупаем корпус
	ChangeGameConfig().Profile[CurrentProfile].Ship = CurrentWorkshopNewFighter;
	ChangeGameConfig().Profile[CurrentProfile].Money -= GetWorkshopShipCost(GameConfig().Profile[CurrentProfile].Ship);



	// создаем объект
	delete WorkshopFighterGame;

	int TMPGameNPCArmorPenalty = GameNPCArmorPenalty;
	GameNPCArmorPenalty = 1;

	WorkshopFighterGame = new cEarthSpaceFighter;
	WorkshopFighterGame->Create(GameConfig().Profile[CurrentProfile].Ship);
	WorkshopFighterGame->ID = 1000;
	WorkshopFighterGame->EngineDestroyType = true;
	WorkshopFighterGame->ShowStrength = false;
	ChangeGameConfig().Profile[CurrentProfile].ShipHullCurrentStrength = WorkshopFighterGame->Strength;
	ChangeGameConfig().Profile[CurrentProfile].ShipHullUpgrade = 1;


	// пытаемся купить

	// 1 - Системы
	if (EngineSystem != 0)
		if (GameConfig().Profile[CurrentProfile].Money >= GetSystemCost(EngineSystem)) {
			ChangeGameConfig().Profile[CurrentProfile].Money -= GetSystemCost(EngineSystem);
			ChangeGameConfig().Profile[CurrentProfile].EngineSystem = EngineSystem;
		}
	if (PowerSystem != 0)
		if (GameConfig().Profile[CurrentProfile].Money >= GetSystemCost(PowerSystem+4)) {
			ChangeGameConfig().Profile[CurrentProfile].Money -= GetSystemCost(PowerSystem+4);
			ChangeGameConfig().Profile[CurrentProfile].PowerSystem = PowerSystem;
		}
	if (TargetingSystem != 0)
		if (GameConfig().Profile[CurrentProfile].Money >= GetSystemCost(TargetingSystem+8)) {
			ChangeGameConfig().Profile[CurrentProfile].Money -= GetSystemCost(TargetingSystem+8);
			ChangeGameConfig().Profile[CurrentProfile].TargetingSystem = TargetingSystem;
		}
	if (TargetingMechanicSystem != 0)
		if (GameConfig().Profile[CurrentProfile].Money >= GetSystemCost(TargetingMechanicSystem+12)) {
			ChangeGameConfig().Profile[CurrentProfile].Money -= GetSystemCost(TargetingMechanicSystem+12);
			ChangeGameConfig().Profile[CurrentProfile].TargetingMechanicSystem = TargetingMechanicSystem;
		}
	if (AdvancedProtectionSystem != 0)
		if (GameConfig().Profile[CurrentProfile].Money >= GetSystemCost(AdvancedProtectionSystem+16)) {
			ChangeGameConfig().Profile[CurrentProfile].Money -= GetSystemCost(AdvancedProtectionSystem+16);
			ChangeGameConfig().Profile[CurrentProfile].AdvancedProtectionSystem = AdvancedProtectionSystem;
		}



	// покупаем оружие, если можем... нет - ставим что продали его (это сделали ранее)
	// всегда покупаем новое, т.е. с полным боекомплектом!
	for (int i = 0; i < WorkshopFighterGame->WeaponQuantity; i++) {
		if (GameConfig().Profile[CurrentProfile].Weapon[i] != 0) {
			if (GameConfig().Profile[CurrentProfile].Money >= GetWeaponBaseCost(GameConfig().Profile[CurrentProfile].Weapon[i])) {
				if (SetEarthSpaceFighterWeapon(WorkshopFighterGame, i + 1, GameConfig().Profile[CurrentProfile].Weapon[i])) {
					if (auto sharedFire = WorkshopFighterGame->Weapon[i]->Fire.lock())
						vw_ReleaseLight(sharedFire->Light);

					ChangeGameConfig().Profile[CurrentProfile].Money -= GetWeaponBaseCost(GameConfig().Profile[CurrentProfile].Weapon[i]);
					ChangeGameConfig().Profile[CurrentProfile].WeaponAmmo[i] = WorkshopFighterGame->Weapon[i]->AmmoStart;
					ChangeGameConfig().Profile[CurrentProfile].WeaponSlotYAngle[i] = 0.0f;
				} else { // если поставить не смогли - не такой слот
					ChangeGameConfig().Profile[CurrentProfile].Weapon[i] = 0;
					ChangeGameConfig().Profile[CurrentProfile].WeaponAmmo[i] = 0;
				}
			} else { // если не хватило денег снова купить
				ChangeGameConfig().Profile[CurrentProfile].Weapon[i] = 0;
				ChangeGameConfig().Profile[CurrentProfile].WeaponAmmo[i] = 0;
			}
		}
	}
	// если было больше слотов чем есть сейчас
	if (OldWeaponQuantity > WorkshopFighterGame->WeaponQuantity) {
		for (int j = 0; j < OldWeaponQuantity; j++) {
			if (WorkshopFighterGame->WeaponQuantity <= j)
				ChangeGameConfig().Profile[CurrentProfile].Weapon[j] = 0;
		}
	}




	// создаем системы (визуальные)
	SetEarthSpaceFighterEngine(WorkshopFighterGame, GameConfig().Profile[CurrentProfile].EngineSystem);
	if (GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem == 2)
		SetEarthSpaceFighterArmour(WorkshopFighterGame, 7);
	else
		SetEarthSpaceFighterArmour(WorkshopFighterGame, GameConfig().Profile[CurrentProfile].ShipHullUpgrade-1);

	GameNPCArmorPenalty = TMPGameNPCArmorPenalty;
	WorkshopFighterGame->SetLocation(sVECTOR3D(1000,-1000-(WorkshopFighterGame->Height/2.0f + WorkshopFighterGame->AABB[6].y), -(WorkshopFighterGame->Length/2.0f + WorkshopFighterGame->AABB[6].z)));
	// чтобы оружие заняло свое место...
	WorkshopFighterGame->Update(vw_GetTimeThread(0));

	WorkshopFighterGame->SetRotation(sVECTOR3D(0.0f,150.0f,0.0f));
}





void BuyShip()
{
	// сбрасываем особенные настройки слотов оружия
	for (int i = 0; i < WorkshopFighterGame->WeaponQuantity; i++) {
		ChangeGameConfig().Profile[CurrentProfile].WeaponSlotYAngle[i] = 0;
	}

	// создаем новый корабль
	WorkshopCreateBuyShip();

	// переводим на другой корабль
	CurrentWorkshopNewFighter++;
	if (CurrentWorkshopNewFighter == GameConfig().Profile[CurrentProfile].Ship)
		CurrentWorkshopNewFighter++;
	if (CurrentWorkshopNewFighter > 22)
		CurrentWorkshopNewFighter = 1;
	// если это 1-й, переводим на 2-й
	if (CurrentWorkshopNewFighter == GameConfig().Profile[CurrentProfile].Ship)
		CurrentWorkshopNewFighter++;
	WorkshopCreateNewShip();
}






void RepairShip()
{
	// смотрим, если денег достаточно для полного ремонта - делаем его
	if (GameConfig().Profile[CurrentProfile].Money >= GetWorkshopShipRepairCost(GameConfig().Profile[CurrentProfile].Ship, WorkshopFighterGame)) {
		ChangeGameConfig().Profile[CurrentProfile].Money -= GetWorkshopShipRepairCost(GameConfig().Profile[CurrentProfile].Ship, WorkshopFighterGame);
		ChangeGameConfig().Profile[CurrentProfile].ShipHullCurrentStrength = WorkshopFighterGame->StrengthStart;
		WorkshopFighterGame->Strength = GameConfig().Profile[CurrentProfile].ShipHullCurrentStrength;
		return;
	}


	// вычисляем, сколько можем отремонтировать
	float StrengtRepair = WorkshopFighterGame->StrengthStart - WorkshopFighterGame->Strength;
	float MoneyRepair = static_cast<float>(GetWorkshopShipRepairCost(GameConfig().Profile[CurrentProfile].Ship, WorkshopFighterGame));
	// сколько можем отремонтировать
	float CanRepair = StrengtRepair * (GameConfig().Profile[CurrentProfile].Money/MoneyRepair);
	// ремонтируем сколько можем
	WorkshopFighterGame->Strength += CanRepair;
	ChangeGameConfig().Profile[CurrentProfile].ShipHullCurrentStrength = WorkshopFighterGame->Strength;
	// это будет стоить все имеющиеся деньги
	ChangeGameConfig().Profile[CurrentProfile].Money = 0;

	// выводим диалог с надписью, что не достаточно денег для полного ремонта
	SetCurrentDialogBox(eDialogBox::RepairShip);
}





void UpgradeShip()
{
	// ув. данные о базовой прочности корабля
	float OldStr = WorkshopFighterGame->StrengthStart;
	WorkshopFighterGame->StrengthStart /= GameConfig().Profile[CurrentProfile].ShipHullUpgrade;

	// ув. данные о номере апгрейда
	ChangeGameConfig().Profile[CurrentProfile].ShipHullUpgrade++;

	WorkshopFighterGame->StrengthStart *= GameConfig().Profile[CurrentProfile].ShipHullUpgrade;
	WorkshopFighterGame->Strength = WorkshopFighterGame->StrengthStart - OldStr +
					GameConfig().Profile[CurrentProfile].ShipHullCurrentStrength;

	ChangeGameConfig().Profile[CurrentProfile].ShipHullCurrentStrength = WorkshopFighterGame->Strength;


	// вычитаем деньги за апгрейд
	ChangeGameConfig().Profile[CurrentProfile].Money -= GetWorkshopShipCost(GameConfig().Profile[CurrentProfile].Ship);


	// сообщаем о новых возможностях и т.п. которые дает этот апгрейд
	SetEarthSpaceFighterArmour(WorkshopFighterGame, GameConfig().Profile[CurrentProfile].ShipHullUpgrade - 1);

}

















//------------------------------------------------------------------------------------
// покупка-апгрейд корпуса
//------------------------------------------------------------------------------------
void Workshop_Shipyard()
{

	// затемнение при выводе
	int SizeI;
	sRECT SrcRect, DstRect;


	// затемнение
	SrcRect(0,0,256,256 );
	DstRect(GameConfig().InternalWidth/2-480, 100-32, GameConfig().InternalWidth/2-32, 450+32);
	vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/back_spot2.tga"), true, 0.45f * MenuContentTransp);
	DstRect(GameConfig().InternalWidth/2+32, 100-32, GameConfig().InternalWidth/2+480, 450+32);
	vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/back_spot2.tga"), true, 0.45f * MenuContentTransp);



	vw_End2DMode();
	WorkshopDrawShip(WorkshopFighterGame, 3);
	WorkshopDrawShip(WorkshopNewFighter, 2);
	vw_Start2DMode(-1,1);


	// условия, при которых корабль купить можно
	bool CanBuy = false;
	if (GetWorkshopShipFullCost(GameConfig().Profile[CurrentProfile].Ship, WorkshopFighterGame) +
	    GameConfig().Profile[CurrentProfile].Money >= GetWorkshopShipCost(CurrentWorkshopNewFighter))
		CanBuy = true;


	// на новом корабле
	vw_SetFontSize(24);
	vw_DrawText(GameConfig().InternalWidth/2-445, 600, 0, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Ship Stock"));
	ResetFontSize();


	// вывод названия корабля
	vw_DrawText(GameConfig().InternalWidth/2-448, 50+6, 0, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("%s Mk%i"), vw_GetText(GetWorkshopShipName(CurrentWorkshopNewFighter)), 1);
	if (DrawButton128_2(GameConfig().InternalWidth/2-197,50, vw_GetText("Info"), MenuContentTransp, false)) {
		SetCurrentDialogBox(eDialogBox::ShowShipInfo);
		DialogSpaceShip = WorkshopNewFighter;
	}


	// находим смещение, чтобы было красиво
	int SmSizeI = vw_TextWidth(vw_GetText("Armor:"));
	int SmSizeI2 = vw_TextWidth(vw_GetText("Weapons"));
	if (SmSizeI < SmSizeI2) SmSizeI = SmSizeI2;

	vw_DrawText(GameConfig().InternalWidth/2-78-40-SmSizeI, 110, 0, 0, 1.0f, eRGBCOLOR::white, 0.5f*MenuContentTransp, vw_GetText("Armor:"));
	vw_DrawText(GameConfig().InternalWidth/2-78-vw_TextWidth("%i", (int)WorkshopNewFighter->StrengthStart), 110, 0, 0, 1.0f, eRGBCOLOR::white, 0.5f*MenuContentTransp, "%i", (int)WorkshopNewFighter->StrengthStart);

	vw_DrawText(GameConfig().InternalWidth/2-78-40-SmSizeI, 130, 0, 0, 1.0f, eRGBCOLOR::white, 0.5f*MenuContentTransp, "%s:", vw_GetText("Weapons"));
	vw_DrawText(GameConfig().InternalWidth/2-78-vw_TextWidth("%i", WorkshopNewFighter->WeaponQuantity), 130, 0, 0, 1.0f, eRGBCOLOR::white, 0.5f*MenuContentTransp, "%i", WorkshopNewFighter->WeaponQuantity);

	// вывод стоимости корабля
	if (CanBuy)
		vw_DrawText(GameConfig().InternalWidth/2-438, 420, 0, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%s: %i", vw_GetText("Ship Hull Cost"), GetWorkshopShipCost(CurrentWorkshopNewFighter));
	else
		vw_DrawText(GameConfig().InternalWidth/2-438, 420, 0, 0, 1.0f, eRGBCOLOR::orange, CurrentAlert3*MenuContentTransp, "%s: %i", vw_GetText("Ship Hull Cost"), GetWorkshopShipCost(CurrentWorkshopNewFighter));



	// рамки
	SrcRect(0,0,400,35 );
	DstRect(GameConfig().InternalWidth/2-457, 100-11, GameConfig().InternalWidth/2-57, 100+35-11);
	vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/workshop_panel4.tga"), true, MenuContentTransp);

	SrcRect(0,0,400,173 );
	DstRect(GameConfig().InternalWidth/2-457, 450-13, GameConfig().InternalWidth/2-57, 450+173-13);
	vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/workshop_panel1.tga"), true, MenuContentTransp);


	// проверяем колесо мышки
	DstRect(GameConfig().InternalWidth/2-457, 100+35-11, GameConfig().InternalWidth/2-57, 450-13);
	if (vw_MouseOverRect(DstRect)) {
		if (vw_GetWheelStatus() != 0 && !isDialogBoxDrawing()) {
			CurrentWorkshopNewFighter += vw_GetWheelStatus();

			if (CurrentWorkshopNewFighter < 1)
				CurrentWorkshopNewFighter = 22;
			if (CurrentWorkshopNewFighter > 22)
				CurrentWorkshopNewFighter = 1;

			WorkshopCreateNewShip();

			vw_ResetWheelStatus();
		}
	} else if (vw_GetWheelStatus() != 0) {
		vw_ResetWheelStatus();
	}

	if (DrawButton128_2(GameConfig().InternalWidth/2-395, 482, vw_GetText("Prev"), MenuContentTransp, false)) {
		CurrentWorkshopNewFighter--;
		if (CurrentWorkshopNewFighter < 1)
			CurrentWorkshopNewFighter = 22;
		WorkshopCreateNewShip();
	}
	if (DrawButton128_2(GameConfig().InternalWidth/2-247, 482, vw_GetText("Next"), MenuContentTransp, false)) {
		CurrentWorkshopNewFighter++;
		if (CurrentWorkshopNewFighter > 22)
			CurrentWorkshopNewFighter = 1;
		WorkshopCreateNewShip();
	}
	if (DrawButton200_2(GameConfig().InternalWidth/2-357, 533, vw_GetText("Buy Ship"), MenuContentTransp, !CanBuy))
		BuyShip();








	// на корабле игрока
	vw_SetFontSize(24);
	vw_DrawText(GameConfig().InternalWidth/2+445-vw_TextWidth(vw_GetText("Player Ship")), 600, 0, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Player Ship"));
	ResetFontSize();

	// вывод названия корабля
	vw_DrawText(GameConfig().InternalWidth/2+64, 56, 0, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("%s Mk%i"), vw_GetText(GetWorkshopShipName(GameConfig().Profile[CurrentProfile].Ship)), GameConfig().Profile[CurrentProfile].ShipHullUpgrade);

	if (DrawButton128_2(GameConfig().InternalWidth/2+315, 50, vw_GetText("Info"), MenuContentTransp, false)) {
		SetCurrentDialogBox(eDialogBox::ShowShipInfo);
		DialogSpaceShip = WorkshopFighterGame;
	}





	int LinePos = 420;

	// вывод стомости модернизации
	// модернизация корпуса
	bool CanUpgrade = false;
	if (GameConfig().Profile[CurrentProfile].ShipHullUpgrade < 4)
		CanUpgrade = true;

	if (CanUpgrade) {
		if (GameConfig().Profile[CurrentProfile].Money < GetWorkshopShipCost(GameConfig().Profile[CurrentProfile].Ship))
			CanUpgrade = false;
		if (CanUpgrade)
			vw_DrawText(GameConfig().InternalWidth/2+438-vw_TextWidth("%s: %i", vw_GetText("Upgrade Hull Cost"), GetWorkshopShipCost(GameConfig().Profile[CurrentProfile].Ship)), LinePos, 0, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%s: %i", vw_GetText("Upgrade Hull Cost"), GetWorkshopShipCost(GameConfig().Profile[CurrentProfile].Ship));
		else
			vw_DrawText(GameConfig().InternalWidth/2+438-vw_TextWidth("%s: %i", vw_GetText("Upgrade Hull Cost"), GetWorkshopShipCost(GameConfig().Profile[CurrentProfile].Ship)), LinePos, 0, 0, 1.0f, eRGBCOLOR::orange, CurrentAlert3*MenuContentTransp, "%s: %i", vw_GetText("Upgrade Hull Cost"), GetWorkshopShipCost(GameConfig().Profile[CurrentProfile].Ship));

		LinePos -= 20;
	}


	// вывод стомости ремонта, если нужно
	bool NeedRepair = false;
	if (GetWorkshopShipRepairCost(GameConfig().Profile[CurrentProfile].Ship, WorkshopFighterGame) > 0) {
		SizeI = vw_TextWidth("%s: %i", vw_GetText("Repair Hull Cost"), GetWorkshopShipRepairCost(GameConfig().Profile[CurrentProfile].Ship, WorkshopFighterGame));
		vw_DrawText(GameConfig().InternalWidth/2+438-SizeI, LinePos, 0, 0, 1.0f, eRGBCOLOR::red, CurrentAlert3*MenuContentTransp, "%s: %i", vw_GetText("Repair Hull Cost"), GetWorkshopShipRepairCost(GameConfig().Profile[CurrentProfile].Ship, WorkshopFighterGame));
		LinePos -= 20;
		NeedRepair = true;

		// надпись Armor, красная
		vw_DrawText(GameConfig().InternalWidth/2+74, 110, 0, 0, 1.0f, eRGBCOLOR::white, 0.5f*MenuContentTransp, vw_GetText("Armor:"));
		SizeI = vw_TextWidth("%i/%i", static_cast<int>(WorkshopFighterGame->Strength), static_cast<int>(WorkshopFighterGame->StrengthStart));
		vw_DrawText(GameConfig().InternalWidth/2+74+14+SmSizeI, 110, 0, 0, 1.0f, eRGBCOLOR::red, CurrentAlert3*MenuContentTransp, "%i/%i", static_cast<int>(WorkshopFighterGame->Strength), static_cast<int>(WorkshopFighterGame->StrengthStart));
	} else {
		// надпись Armor, нормальная
		vw_DrawText(GameConfig().InternalWidth/2+74, 110, 0, 0, 1.0f, eRGBCOLOR::white, 0.5f*MenuContentTransp, vw_GetText("Armor:"));
		SizeI = vw_TextWidth("%i/%i", static_cast<int>(WorkshopFighterGame->Strength), static_cast<int>(WorkshopFighterGame->StrengthStart));
		vw_DrawText(GameConfig().InternalWidth/2+74+14+SmSizeI, 110, 0, 0,1.0f, eRGBCOLOR::white, 0.5f*MenuContentTransp, "%i/%i", static_cast<int>(WorkshopFighterGame->Strength), static_cast<int>(WorkshopFighterGame->StrengthStart));
	}

	SizeI = SizeI - vw_TextWidth("%i", WorkshopFighterGame->WeaponQuantity);
	vw_DrawText(GameConfig().InternalWidth/2+74, 130, 0, 0, 1.0f, eRGBCOLOR::white, 0.5f*MenuContentTransp, "%s:", vw_GetText("Weapons"));
	vw_DrawText(GameConfig().InternalWidth/2+74+14+SmSizeI+SizeI, 130, 0, 0, 1.0f, eRGBCOLOR::white, 0.5f*MenuContentTransp, "%i", WorkshopFighterGame->WeaponQuantity);




	// вывод стоимости корабля (со всеми системами)
	SizeI = vw_TextWidth("%s: %i", vw_GetText("Ship Cost"), GetWorkshopShipFullCost(GameConfig().Profile[CurrentProfile].Ship, WorkshopFighterGame));
	vw_DrawText(GameConfig().InternalWidth/2+438-SizeI, LinePos, 0, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%s: %i", vw_GetText("Ship Cost"), GetWorkshopShipFullCost(GameConfig().Profile[CurrentProfile].Ship, WorkshopFighterGame));


	// рамки
	SrcRect(0,0,400,35 );
	DstRect(GameConfig().InternalWidth/2+57, 100-11, GameConfig().InternalWidth/2+457, 100+35-11);
	vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/workshop_panel4.tga"), true, MenuContentTransp);

	SrcRect(0,0,400,173 );
	DstRect(GameConfig().InternalWidth/2+57, 450-13, GameConfig().InternalWidth/2+457, 450+173-13);
	vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/workshop_panel1+.tga"), true, MenuContentTransp);



	if (DrawButton200_2(GameConfig().InternalWidth/2+153, 482, vw_GetText("Repair Hull"), MenuContentTransp, !NeedRepair)) {
		RepairShip();
	}


	if (DrawButton200_2(GameConfig().InternalWidth/2+153, 533, vw_GetText("Upgrade Hull"), MenuContentTransp, !CanUpgrade)) {
		UpgradeShip();
	}


	// вывод информации
	vw_SetFontSize(20);
	SizeI = (GameConfig().InternalWidth-vw_TextWidth("%s: %i", vw_GetText("Money"), GameConfig().Profile[CurrentProfile].Money)) / 2;
	if (CanBuy)
		vw_DrawText(SizeI, 630, 0, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, "%s: %i", vw_GetText("Money"), GameConfig().Profile[CurrentProfile].Money);
	else
		vw_DrawText(SizeI, 630, 0, 0, 1.0f, eRGBCOLOR::orange, CurrentAlert3*MenuContentTransp, "%s: %i", vw_GetText("Money"), GameConfig().Profile[CurrentProfile].Money);
	ResetFontSize();
}


