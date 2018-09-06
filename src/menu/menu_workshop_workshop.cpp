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
	along with AstroMenace. If not, see <https://www.gnu.org/licenses/>.


	Website: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#include "../game.h"
#include "../config/config.h"
#include "../ui/font.h"
#include "../assets/audio.h"
#include "../assets/texture.h"
#include "../object3d/space_ship/space_ship.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

extern std::weak_ptr<cSpaceShip> WorkshopFighterGame;
extern float CurrentAlert2;
extern float CurrentAlert3;

void WorkshopDrawShip(std::weak_ptr<cSpaceShip> &SpaceShip, int Mode);

float GetShipEngineSystemEnergyUse(int Num);
float GetShipProtectionSystemEnergyUse(int Num);
float GetShipRechargeEnergy(int Num);


// проблемы с энергией
extern bool NeedMoreEnergyDialog;
// Номер, для проигрывания голосового сообщения об проблемы с энергией
unsigned int VoiceNeedMoreEnergy{0};


// что рисовать в диалоге 6,7,8
extern int DialogSystem;


// текущий номер системы в мастерской
int CurrentSystemStockNum = 1;



int GetCurrentSustemStockNumBase()
{
	switch (CurrentSystemStockNum) {
	case 1:
	case 2:
	case 3:
	case 4:
		return 1;

	case 5:
	case 6:
	case 7:
	case 8:
		return 5;

	case 9:
	case 10:
	case 11:
	case 12:
		return 9;

	case 13:
	case 14:
	case 15:
	case 16:
		return 13;

	case 17:
	case 18:
	case 19:
	case 20:
		return 17;

	default:
		std::cerr << __func__ << "(): " << "wrong CurrentSystemStockNum.\n";
		break;
	}

	return 0;
}



const char *GetSystemName(int Num)
{
	switch (Num) {
	case 1:
		return "Jet Engine";
	case 2:
		return "Photon Jet";
	case 3:
		return "Plasma Jet";
	case 4:
		return "Antimatter";

	case 5:
		return "Ion Battery";
	case 6:
		return "Nuclear";
	case 7:
		return "Plasma";
	case 8:
		return "Antimatter";

	case 9:
		return "Base O.T.C.";
	case 10:
		return "Hamerik O.T.C.";
	case 11:
		return "Neo M.O.T.C.";
	case 12:
		return "Supra M.O.T.C.";

	case 13:
		return "Mechanical";
	case 14:
		return "Hydraulic";
	case 15:
		return "Magnetic";
	case 16:
		return "Vacuum";

	case 17:
		return "Repair NaRs";
	case 18:
		return "Layer";
	case 19:
		return "Energy Shield";
	case 20:
		return "Deflector";

	default:
		std::cerr << __func__ << "(): " << "wrong Num.\n";
		break;
	}

	return nullptr;
}




GLtexture GetSystemIcon(int Num)
{
	switch (Num) {
	case -4:
		return GetPreloadedTextureAsset("menu/system_empty.tga");
	case -3:
		return GetPreloadedTextureAsset("menu/system_empty.tga");
	case -2:
		return GetPreloadedTextureAsset("menu/system_empty.tga");
	case -1:
		return GetPreloadedTextureAsset("menu/system_empty.tga");
	case 0:
		return GetPreloadedTextureAsset("menu/system_empty.tga");

	case 1:
		return GetPreloadedTextureAsset("menu/system_engine1.tga");
	case 2:
		return GetPreloadedTextureAsset("menu/system_engine2.tga");
	case 3:
		return GetPreloadedTextureAsset("menu/system_engine3.tga");
	case 4:
		return GetPreloadedTextureAsset("menu/system_engine4.tga");

	case 5:
		return GetPreloadedTextureAsset("menu/system_power1.tga");
	case 6:
		return GetPreloadedTextureAsset("menu/system_power2.tga");
	case 7:
		return GetPreloadedTextureAsset("menu/system_power3.tga");
	case 8:
		return GetPreloadedTextureAsset("menu/system_power4.tga");

	case 9:
		return GetPreloadedTextureAsset("menu/system_target1.tga");
	case 10:
		return GetPreloadedTextureAsset("menu/system_target2.tga");
	case 11:
		return GetPreloadedTextureAsset("menu/system_target3.tga");
	case 12:
		return GetPreloadedTextureAsset("menu/system_target4.tga");

	case 13:
		return GetPreloadedTextureAsset("menu/system_mechan1.tga");
	case 14:
		return GetPreloadedTextureAsset("menu/system_mechan2.tga");
	case 15:
		return GetPreloadedTextureAsset("menu/system_mechan3.tga");
	case 16:
		return GetPreloadedTextureAsset("menu/system_mechan4.tga");

	case 17:
		return GetPreloadedTextureAsset("menu/system_protect1.tga");
	case 18:
		return GetPreloadedTextureAsset("menu/system_protect2.tga");
	case 19:
		return GetPreloadedTextureAsset("menu/system_protect3.tga");
	case 20:
		return GetPreloadedTextureAsset("menu/system_protect4.tga");

	default:
		std::cerr << __func__ << "(): " << "wrong Num.\n";
		break;
	}

	return 0;
}



int GetSystemCost(int Num)
{
	switch (Num) {
	case 1:
		return 200;
	case 2:
		return 1500;
	case 3:
		return 3000;
	case 4:
		return 6000;

	case 5:
		return 250;
	case 6:
		return 2000;
	case 7:
		return 7000;
	case 8:
		return 15000;

	case 9:
		return 100;
	case 10:
		return 1000;
	case 11:
		return 3000;
	case 12:
		return 6000;

	case 13:
		return 150;
	case 14:
		return 1000;
	case 15:
		return 3000;
	case 16:
		return 7000;

	case 17:
		return 2500;
	case 18:
		return 6000;
	case 19:
		return 10000;
	case 20:
		return 15000;

	default:
		std::cerr << __func__ << "(): " << "wrong Num.\n";
		break;
	}

	return 0;
}





void SellCurrentSystem()
{
	if ((CurrentSystemStockNum >= 1) && (CurrentSystemStockNum <= 4)) {
		ChangeGameConfig().Profile[CurrentProfile].Money += GetSystemCost(GameConfig().Profile[CurrentProfile].EngineSystem);
		ChangeGameConfig().Profile[CurrentProfile].EngineSystem = 0;
		SetEarthSpaceFighterEngine(WorkshopFighterGame, GameConfig().Profile[CurrentProfile].EngineSystem);
	}
	if ((CurrentSystemStockNum >= 5) && (CurrentSystemStockNum <= 8)) {
		ChangeGameConfig().Profile[CurrentProfile].Money += GetSystemCost(GameConfig().Profile[CurrentProfile].PowerSystem+4);
		ChangeGameConfig().Profile[CurrentProfile].PowerSystem = 0;
	}
	if ((CurrentSystemStockNum >= 9) && (CurrentSystemStockNum <= 12)) {
		ChangeGameConfig().Profile[CurrentProfile].Money += GetSystemCost(GameConfig().Profile[CurrentProfile].TargetingSystem+8);
		ChangeGameConfig().Profile[CurrentProfile].TargetingSystem = 0;
	}
	if ((CurrentSystemStockNum >= 13) && (CurrentSystemStockNum <= 16)) {
		ChangeGameConfig().Profile[CurrentProfile].Money += GetSystemCost(GameConfig().Profile[CurrentProfile].TargetingMechanicSystem+12);
		ChangeGameConfig().Profile[CurrentProfile].TargetingMechanicSystem = 0;
	}
	if ((CurrentSystemStockNum >= 17) && (CurrentSystemStockNum <= 20)) {
		ChangeGameConfig().Profile[CurrentProfile].Money += GetSystemCost(GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem+16);
		ChangeGameConfig().Profile[CurrentProfile].AdvancedProtectionSystem = 0;

		SetEarthSpaceFighterArmor(WorkshopFighterGame, GameConfig().Profile[CurrentProfile].ShipHullUpgrade - 1);
	}
}


void BuyCurrentSystem()
{
	if ((CurrentSystemStockNum >= 1) && (CurrentSystemStockNum <= 4)) {
		if (GameConfig().Profile[CurrentProfile].EngineSystem != 0)
			ChangeGameConfig().Profile[CurrentProfile].Money += GetSystemCost(GameConfig().Profile[CurrentProfile].EngineSystem);
		ChangeGameConfig().Profile[CurrentProfile].EngineSystem = CurrentSystemStockNum;
		ChangeGameConfig().Profile[CurrentProfile].Money -= GetSystemCost(GameConfig().Profile[CurrentProfile].EngineSystem);

		SetEarthSpaceFighterEngine(WorkshopFighterGame, GameConfig().Profile[CurrentProfile].EngineSystem);
	}
	if ((CurrentSystemStockNum >= 5) && (CurrentSystemStockNum <= 8)) {
		if (GameConfig().Profile[CurrentProfile].PowerSystem != 0)
			ChangeGameConfig().Profile[CurrentProfile].Money += GetSystemCost(GameConfig().Profile[CurrentProfile].PowerSystem + 4);
		ChangeGameConfig().Profile[CurrentProfile].PowerSystem = CurrentSystemStockNum - 4;
		ChangeGameConfig().Profile[CurrentProfile].Money -= GetSystemCost(GameConfig().Profile[CurrentProfile].PowerSystem + 4);
	}
	if ((CurrentSystemStockNum >= 9) && (CurrentSystemStockNum <= 12)) {
		if (GameConfig().Profile[CurrentProfile].TargetingSystem != 0)
			ChangeGameConfig().Profile[CurrentProfile].Money += GetSystemCost(GameConfig().Profile[CurrentProfile].TargetingSystem + 8);
		ChangeGameConfig().Profile[CurrentProfile].TargetingSystem = CurrentSystemStockNum - 8;
		ChangeGameConfig().Profile[CurrentProfile].Money -= GetSystemCost(GameConfig().Profile[CurrentProfile].TargetingSystem + 8);
	}
	if ((CurrentSystemStockNum >= 13) && (CurrentSystemStockNum <= 16)) {
		if (GameConfig().Profile[CurrentProfile].TargetingMechanicSystem != 0)
			ChangeGameConfig().Profile[CurrentProfile].Money += GetSystemCost(GameConfig().Profile[CurrentProfile].TargetingMechanicSystem + 12);
		ChangeGameConfig().Profile[CurrentProfile].TargetingMechanicSystem = CurrentSystemStockNum - 12;
		ChangeGameConfig().Profile[CurrentProfile].Money -= GetSystemCost(GameConfig().Profile[CurrentProfile].TargetingMechanicSystem + 12);
	}
	if ((CurrentSystemStockNum >= 17) && (CurrentSystemStockNum <= 20)) {
		if (GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem != 0)
			ChangeGameConfig().Profile[CurrentProfile].Money += GetSystemCost(GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem + 16);
		ChangeGameConfig().Profile[CurrentProfile].AdvancedProtectionSystem = CurrentSystemStockNum - 16;
		ChangeGameConfig().Profile[CurrentProfile].Money -= GetSystemCost(GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem + 16);
		SetEarthSpaceFighterArmor(WorkshopFighterGame, GameConfig().Profile[CurrentProfile].ShipHullUpgrade - 1);
	}
}














// для кнопок, чтобы проигрывать наведение
// последние координаты кнопки на которую наводились
int NeedPlayWorkshopOnButtonSoundX = 0;
int NeedPlayWorkshopOnButtonSoundY = 0;




//------------------------------------------------------------------------------------
// покупка-установка систем корабля
//------------------------------------------------------------------------------------
void Workshop_Workshop()
{
	sRECT SrcRect(0, 0, 256, 256);
	sRECT DstRect(GameConfig().InternalWidth / 2 - 256, 0, GameConfig().InternalWidth / 2 - 256 + 512, 412);
	vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/back_spot.tga"), true, 0.35f * MenuContentTransp);



	vw_End2DMode();
	WorkshopDrawShip(WorkshopFighterGame, 1);
	vw_Start2DMode(-1,1);


	SrcRect(0,0,210,600);
	DstRect(GameConfig().InternalWidth/2-492, 50-10, GameConfig().InternalWidth/2-492+210, 50+600-10);
	vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/workshop_panel2.tga"), true, MenuContentTransp);



	// выводим иконки систем в магазине, по которым тоже можно кликать

	float Current = 0.4f;

	SrcRect(0,0,128,128);
	DstRect(GameConfig().InternalWidth/2-451, 60, GameConfig().InternalWidth/2-451+128, 60+128);
	if (vw_MouseOverRect(DstRect) && !isDialogBoxDrawing()) {
		if (NeedPlayWorkshopOnButtonSoundX != DstRect.left || NeedPlayWorkshopOnButtonSoundY != DstRect.top) {
			PlayMenuSFX(eMenuSFX::OverBigButton, 1.0f);
			NeedPlayWorkshopOnButtonSoundX = DstRect.left;
			NeedPlayWorkshopOnButtonSoundY = DstRect.top;
		}

		CurrentCursorStatus = 1;
		if (vw_GetMouseLeftClick(true)) {
			CurrentSystemStockNum = GetCurrentSustemStockNumBase();
			PlayMenuSFX(eMenuSFX::Click, 1.0f);
		}

		Current = 0.7f;
	} else {
		if (NeedPlayWorkshopOnButtonSoundX == DstRect.left && NeedPlayWorkshopOnButtonSoundY == DstRect.top) {
			NeedPlayWorkshopOnButtonSoundX = 0;
			NeedPlayWorkshopOnButtonSoundY = 0;
		}
	}
	if (CurrentSystemStockNum == GetCurrentSustemStockNumBase()) {
		Current = 1.0f;
	}
	vw_Draw2D(DstRect, SrcRect, GetSystemIcon(GetCurrentSustemStockNumBase()), true, MenuContentTransp*Current);
	int Size = vw_TextWidth(vw_GetText(GetSystemName(GetCurrentSustemStockNumBase())));
	float WScale = 0;
	if (Size > 128) {
		Size = 128;
		WScale = -128;
	}
	vw_DrawText(DstRect.left+(DstRect.right-DstRect.left-Size)/2, DstRect.bottom-20, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp*Current, vw_GetText(GetSystemName(GetCurrentSustemStockNumBase())));




	Current = 0.4f;

	SrcRect(0,0,128,128);
	DstRect(GameConfig().InternalWidth/2-451, 200, GameConfig().InternalWidth/2-451+128, 200+128);
	if (vw_MouseOverRect(DstRect) && !isDialogBoxDrawing()) {
		if (NeedPlayWorkshopOnButtonSoundX != DstRect.left || NeedPlayWorkshopOnButtonSoundY != DstRect.top) {
			PlayMenuSFX(eMenuSFX::OverBigButton, 1.0f);
			NeedPlayWorkshopOnButtonSoundX = DstRect.left;
			NeedPlayWorkshopOnButtonSoundY = DstRect.top;
		}

		CurrentCursorStatus = 1;
		if (vw_GetMouseLeftClick(true)) {
			CurrentSystemStockNum = GetCurrentSustemStockNumBase()+1;
			PlayMenuSFX(eMenuSFX::Click, 1.0f);
		}

		Current = 0.7f;
	} else {
		if (NeedPlayWorkshopOnButtonSoundX == DstRect.left && NeedPlayWorkshopOnButtonSoundY == DstRect.top) {
			NeedPlayWorkshopOnButtonSoundX = 0;
			NeedPlayWorkshopOnButtonSoundY = 0;
		}
	}
	if (CurrentSystemStockNum == GetCurrentSustemStockNumBase()+1) {
		Current = 1.0f;
	}
	vw_Draw2D(DstRect, SrcRect, GetSystemIcon(GetCurrentSustemStockNumBase()+1), true, MenuContentTransp*Current);
	Size = vw_TextWidth(vw_GetText(GetSystemName(GetCurrentSustemStockNumBase()+1)));
	WScale = 0;
	if (Size > 128) {
		Size = 128;
		WScale = -128;
	}
	vw_DrawText(DstRect.left+(DstRect.right-DstRect.left-Size)/2, DstRect.bottom-20, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp*Current, vw_GetText(GetSystemName(GetCurrentSustemStockNumBase()+1)));




	Current = 0.4f;

	SrcRect(0,0,128,128);
	DstRect(GameConfig().InternalWidth/2-451, 340, GameConfig().InternalWidth/2-451+128, 340+128);
	if (vw_MouseOverRect(DstRect) && !isDialogBoxDrawing()) {
		if (NeedPlayWorkshopOnButtonSoundX != DstRect.left || NeedPlayWorkshopOnButtonSoundY != DstRect.top) {
			PlayMenuSFX(eMenuSFX::OverBigButton, 1.0f);
			NeedPlayWorkshopOnButtonSoundX = DstRect.left;
			NeedPlayWorkshopOnButtonSoundY = DstRect.top;
		}

		CurrentCursorStatus = 1;
		if (vw_GetMouseLeftClick(true)) {
			CurrentSystemStockNum = GetCurrentSustemStockNumBase()+2;
			PlayMenuSFX(eMenuSFX::Click, 1.0f);
		}

		Current = 0.7f;
	} else {
		if (NeedPlayWorkshopOnButtonSoundX == DstRect.left && NeedPlayWorkshopOnButtonSoundY == DstRect.top) {
			NeedPlayWorkshopOnButtonSoundX = 0;
			NeedPlayWorkshopOnButtonSoundY = 0;
		}
	}
	if (CurrentSystemStockNum == GetCurrentSustemStockNumBase()+2) {
		Current = 1.0f;
	}
	vw_Draw2D(DstRect, SrcRect, GetSystemIcon(GetCurrentSustemStockNumBase()+2), true, MenuContentTransp*Current);
	Size = vw_TextWidth(vw_GetText(GetSystemName(GetCurrentSustemStockNumBase()+2)));
	WScale = 0;
	if (Size > 128) {
		Size = 128;
		WScale = -128;
	}
	vw_DrawText(DstRect.left+(DstRect.right-DstRect.left-Size)/2, DstRect.bottom-20, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp*Current, vw_GetText(GetSystemName(GetCurrentSustemStockNumBase()+2)));




	Current = 0.4f;

	SrcRect(0,0,128,128);
	DstRect(GameConfig().InternalWidth/2-451, 610-128, GameConfig().InternalWidth/2-451+128, 610);
	if (vw_MouseOverRect(DstRect) && !isDialogBoxDrawing()) {
		if (NeedPlayWorkshopOnButtonSoundX != DstRect.left || NeedPlayWorkshopOnButtonSoundY != DstRect.top) {
			PlayMenuSFX(eMenuSFX::OverBigButton, 1.0f);
			NeedPlayWorkshopOnButtonSoundX = DstRect.left;
			NeedPlayWorkshopOnButtonSoundY = DstRect.top;
		}

		CurrentCursorStatus = 1;
		if (vw_GetMouseLeftClick(true)) {
			CurrentSystemStockNum = GetCurrentSustemStockNumBase()+3;
			PlayMenuSFX(eMenuSFX::Click, 1.0f);
		}

		Current = 0.7f;
	} else {
		if (NeedPlayWorkshopOnButtonSoundX == DstRect.left && NeedPlayWorkshopOnButtonSoundY == DstRect.top) {
			NeedPlayWorkshopOnButtonSoundX = 0;
			NeedPlayWorkshopOnButtonSoundY = 0;
		}
	}
	if (CurrentSystemStockNum == GetCurrentSustemStockNumBase()+3) {
		Current = 1.0f;
	}
	vw_Draw2D(DstRect, SrcRect, GetSystemIcon(GetCurrentSustemStockNumBase()+3), true, MenuContentTransp*Current);
	Size = vw_TextWidth(vw_GetText(GetSystemName(GetCurrentSustemStockNumBase()+3)));
	WScale = 0;
	if (Size > 128) {
		Size = 128;
		WScale = -128;
	}
	vw_DrawText(DstRect.left+(DstRect.right-DstRect.left-Size)/2, DstRect.bottom-20, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp*Current, vw_GetText(GetSystemName(GetCurrentSustemStockNumBase()+3)));



	vw_SetFontSize(24);
	vw_DrawText(GameConfig().InternalWidth/2-475, 630, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetText("System Stock"));
	ResetFontSize();












	SrcRect(0,0,210,600);
	DstRect(GameConfig().InternalWidth/2+282, 50-10, GameConfig().InternalWidth/2+492, 50+600-10);
	vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/workshop_panel2+.tga"), true, MenuContentTransp);



	// проверяем, а все ли нормально с энергией... если не нормально, будем моргать и выведем соотв. надпись
	bool NeedMoreEnergy = false;

	if (GameConfig().Profile[CurrentProfile].PowerSystem != 0) {

		float Have = GetShipRechargeEnergy(GameConfig().Profile[CurrentProfile].PowerSystem);

		float Need = 0.0f;

		// если это не аркадный режим, нужно учитывать двигатель
		if ((GameConfig().Profile[CurrentProfile].SpaceShipControlMode != 1) &&
		    (GameConfig().Profile[CurrentProfile].EngineSystem != 0))
			Need += GetShipEngineSystemEnergyUse(GameConfig().Profile[CurrentProfile].EngineSystem);
		// если есть спец система, ее нужно тоже учитывать
		if (GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem != 0)
			Need += GetShipProtectionSystemEnergyUse(GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem);

		// слабый реактор
		if (Need >= Have) NeedMoreEnergy = true;

	} else {
		// у нас вообще нет реактора... караул!!!
		NeedMoreEnergy = true;
	}

	// выводим информационную надпись + голосовое сообщение
	if (NeedMoreEnergy) {
		// запускаем голос...
		if (!vw_IsSoundAvailable(VoiceNeedMoreEnergy))
			VoiceNeedMoreEnergy = PlayVoicePhrase(eVoicePhrase::ReactorMalfunction, 1.0f);

		// вывод текста
		int SizeI = (GameConfig().InternalWidth - vw_TextWidth(vw_GetText("Warning! Low energy recharge rate!"))) / 2;
		vw_DrawText(SizeI, 60, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::red}, CurrentAlert3 * MenuContentTransp, vw_GetText("Warning! Low energy recharge rate!"));

		SizeI = (GameConfig().InternalWidth - vw_TextWidth(vw_GetText("Could be problems with"))) / 2;
		vw_DrawText(SizeI, 80, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::red}, CurrentAlert3 * MenuContentTransp, vw_GetText("Could be problems with"));

		SizeI = (GameConfig().InternalWidth - vw_TextWidth(vw_GetText("systems function and/or weapons recharge!"))) / 2;
		vw_DrawText(SizeI, 100, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::red}, CurrentAlert3 * MenuContentTransp, vw_GetText("systems function and/or weapons recharge!"));
	}




	// сбор данных
	int Cost = 0;
	bool CanSell = false;
	bool CanBuy = true;

	// Engine
	DstRect(GameConfig().InternalWidth/2+337, 70, GameConfig().InternalWidth/2+437, 70+100);

	if ((CurrentSystemStockNum >= 1) && (CurrentSystemStockNum <= 4)) {
		if (GameConfig().Profile[CurrentProfile].EngineSystem != 0) {
			CanSell = true;
			Cost = GetSystemCost(GameConfig().Profile[CurrentProfile].EngineSystem);
			if (Cost + GameConfig().Profile[CurrentProfile].Money < GetSystemCost(CurrentSystemStockNum))
				CanBuy = false;
		} else {
			if (GameConfig().Profile[CurrentProfile].Money < GetSystemCost(CurrentSystemStockNum))
				CanBuy = false;
		}
	}


	// Power
	DstRect(GameConfig().InternalWidth/2+337, 180, GameConfig().InternalWidth/2+437, 180+100);

	if ((CurrentSystemStockNum >= 5) && (CurrentSystemStockNum <= 8)) {
		if (GameConfig().Profile[CurrentProfile].PowerSystem != 0) {
			CanSell = true;
			Cost = GetSystemCost(GameConfig().Profile[CurrentProfile].PowerSystem + 4);
			if (Cost + GameConfig().Profile[CurrentProfile].Money < GetSystemCost(CurrentSystemStockNum))
				CanBuy = false;
		} else {
			if (GameConfig().Profile[CurrentProfile].Money < GetSystemCost(CurrentSystemStockNum))
				CanBuy = false;
		}
	}

	// Target
	DstRect(GameConfig().InternalWidth/2+337, 290, GameConfig().InternalWidth/2+437, 290+100);

	if ((CurrentSystemStockNum >= 9) && (CurrentSystemStockNum <= 12)) {
		if (GameConfig().Profile[CurrentProfile].TargetingSystem != 0) {
			CanSell = true;
			Cost = GetSystemCost(GameConfig().Profile[CurrentProfile].TargetingSystem+8);
			if (Cost + GameConfig().Profile[CurrentProfile].Money < GetSystemCost(CurrentSystemStockNum))
				CanBuy = false;
		} else {
			if (GameConfig().Profile[CurrentProfile].Money < GetSystemCost(CurrentSystemStockNum))
				CanBuy = false;
		}
	}


	// Mech
	DstRect(GameConfig().InternalWidth/2+337, 400, GameConfig().InternalWidth/2+437, 400+100);
	if ((CurrentSystemStockNum >= 13) && (CurrentSystemStockNum <= 16)) {
		if (GameConfig().Profile[CurrentProfile].TargetingMechanicSystem != 0) {
			CanSell = true;
			Cost = GetSystemCost(GameConfig().Profile[CurrentProfile].TargetingMechanicSystem + 12);
			if (Cost + GameConfig().Profile[CurrentProfile].Money < GetSystemCost(CurrentSystemStockNum))
				CanBuy = false;
		} else {
			if (GameConfig().Profile[CurrentProfile].Money < GetSystemCost(CurrentSystemStockNum))
				CanBuy = false;
		}
	}

	// Protect
	DstRect(GameConfig().InternalWidth/2+337, 510, GameConfig().InternalWidth/2+437, 510+100);
	if ((CurrentSystemStockNum >= 17) && (CurrentSystemStockNum <= 20)) {
		if (GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem != 0) {
			CanSell = true;
			Cost = GetSystemCost(GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem+16);
			if (Cost + GameConfig().Profile[CurrentProfile].Money < GetSystemCost(CurrentSystemStockNum))
				CanBuy = false;
		} else {
			if (GameConfig().Profile[CurrentProfile].Money < GetSystemCost(CurrentSystemStockNum))
				CanBuy = false;
		}
	}



	// прорисовка


	// Engine
	SrcRect(0,0,128,128);
	DstRect(GameConfig().InternalWidth/2+337, 70, GameConfig().InternalWidth/2+437, 70+100);
	Current = 0.5f;
	sRGBCOLOR tmpColor{sRGBCOLOR{eRGBCOLOR::white}};

	if (NeedMoreEnergy &&
	    (GameConfig().Profile[CurrentProfile].EngineSystem != 0) &&
	    (GameConfig().Profile[CurrentProfile].SpaceShipControlMode != 1))
		tmpColor = sRGBCOLOR{eRGBCOLOR::red};


	if ((CurrentSystemStockNum >= 1) && (CurrentSystemStockNum <= 4)) {
		if (!NeedMoreEnergy ||
		    (GameConfig().Profile[CurrentProfile].EngineSystem == 0) ||
		    (GameConfig().Profile[CurrentProfile].SpaceShipControlMode == 1)) {
			if (CanBuy)
				tmpColor = sRGBCOLOR{0.0f, 0.8f, 0.0f};
			else
				tmpColor = sRGBCOLOR{eRGBCOLOR::orange};
		}

		Current = 1.0f;
	}

	if (vw_MouseOverRect(DstRect) && !isDialogBoxDrawing()) {
		if ((NeedPlayWorkshopOnButtonSoundX != DstRect.left) ||
		    (NeedPlayWorkshopOnButtonSoundY != DstRect.top)) {
			PlayMenuSFX(eMenuSFX::OverBigButton, 1.0f);
			NeedPlayWorkshopOnButtonSoundX = DstRect.left;
			NeedPlayWorkshopOnButtonSoundY = DstRect.top;
		}

		Current = 1.0f;
		CurrentCursorStatus = 1;
		if (vw_GetMouseLeftClick(true)) {
			CurrentSystemStockNum = 1;
			PlayMenuSFX(eMenuSFX::Click, 1.0f);
		}
	} else {
		if (NeedPlayWorkshopOnButtonSoundX == DstRect.left && NeedPlayWorkshopOnButtonSoundY == DstRect.top) {
			NeedPlayWorkshopOnButtonSoundX = 0;
			NeedPlayWorkshopOnButtonSoundY = 0;
		}
	}
	if (GameConfig().Profile[CurrentProfile].EngineSystem <= 0) {
		vw_Draw2D(DstRect, SrcRect, GetSystemIcon(-4), true, Current*MenuContentTransp);

		Size = vw_TextWidth(vw_GetText("empty"));
		WScale = 0;
		if (Size > 128) {
			Size = 128;
			WScale = -128;
		}
		vw_DrawText(DstRect.left+(DstRect.right-DstRect.left-Size)/2, DstRect.top-4, WScale, 0, 1.0f, tmpColor, Current*MenuContentTransp, vw_GetText("empty"));

		Size = vw_TextWidth(vw_GetText("Spaceship Engine"));
		WScale = 0;
		if (Size > 128) {
			Size = 128;
			WScale = -128;
		}
		vw_DrawText(DstRect.left+(DstRect.right-DstRect.left-Size)/2, DstRect.bottom-20, WScale, 0, 1.0f, tmpColor, Current*MenuContentTransp, vw_GetText("Spaceship Engine"));
	} else {
		vw_Draw2D(DstRect, SrcRect, GetSystemIcon(GameConfig().Profile[CurrentProfile].EngineSystem), true, Current*MenuContentTransp);

		Size = vw_TextWidth(vw_GetText(GetSystemName(GameConfig().Profile[CurrentProfile].EngineSystem)));
		WScale = 0;
		if (Size > 128) {
			Size = 128;
			WScale = -128;
		}
		vw_DrawText(DstRect.left+(DstRect.right-DstRect.left-Size)/2, DstRect.bottom-70, WScale, 0, 1.0f, tmpColor, Current*MenuContentTransp, vw_GetText(GetSystemName(GameConfig().Profile[CurrentProfile].EngineSystem)));
	}


	// Power
	DstRect(GameConfig().InternalWidth/2+337, 180, GameConfig().InternalWidth/2+437, 180+100);
	tmpColor = sRGBCOLOR{eRGBCOLOR::white};

	if (NeedMoreEnergy)
		tmpColor = sRGBCOLOR{eRGBCOLOR::red};

	Current = 0.5f;
	if ((CurrentSystemStockNum >= 5) && (CurrentSystemStockNum <= 8)) {
		if (!NeedMoreEnergy) {
			if (CanBuy)
				tmpColor = sRGBCOLOR{0.0f, 0.8f, 0.0f};
			else
				tmpColor = sRGBCOLOR{eRGBCOLOR::orange};
		}
		Current = 1.0f;
	}

	if (vw_MouseOverRect(DstRect) && !isDialogBoxDrawing()) {
		if (NeedPlayWorkshopOnButtonSoundX != DstRect.left || NeedPlayWorkshopOnButtonSoundY != DstRect.top) {
			PlayMenuSFX(eMenuSFX::OverBigButton, 1.0f);
			NeedPlayWorkshopOnButtonSoundX = DstRect.left;
			NeedPlayWorkshopOnButtonSoundY = DstRect.top;
		}

		Current = 1.0f;
		CurrentCursorStatus = 1;
		if (vw_GetMouseLeftClick(true)) {
			CurrentSystemStockNum = 5;
			PlayMenuSFX(eMenuSFX::Click, 1.0f);
		}
	} else {
		if (NeedPlayWorkshopOnButtonSoundX == DstRect.left && NeedPlayWorkshopOnButtonSoundY == DstRect.top) {
			NeedPlayWorkshopOnButtonSoundX = 0;
			NeedPlayWorkshopOnButtonSoundY = 0;
		}
	}

	if (GameConfig().Profile[CurrentProfile].PowerSystem <= 0) {
		vw_Draw2D(DstRect, SrcRect, GetSystemIcon(-2), true, Current*MenuContentTransp);

		Size = vw_TextWidth(vw_GetText("empty"));
		WScale = 0;
		if (Size > 128) {
			Size = 128;
			WScale = -128;
		}
		vw_DrawText(DstRect.left+(DstRect.right-DstRect.left-Size)/2, DstRect.top-4, WScale, 0, 1.0f, tmpColor, Current*MenuContentTransp, vw_GetText("empty"));

		Size = vw_TextWidth(vw_GetText("Power Source"));
		WScale = 0;
		if (Size > 128) {
			Size = 128;
			WScale = -128;
		}
		vw_DrawText(DstRect.left+(DstRect.right-DstRect.left-Size)/2, DstRect.bottom-20, WScale, 0, 1.0f, tmpColor, Current*MenuContentTransp, vw_GetText("Power Source"));
	} else {
		vw_Draw2D(DstRect, SrcRect, GetSystemIcon(GameConfig().Profile[CurrentProfile].PowerSystem + 4), true, Current * MenuContentTransp);

		Size = vw_TextWidth(vw_GetText(GetSystemName(GameConfig().Profile[CurrentProfile].PowerSystem + 4)));
		WScale = 0;
		if (Size > 128) {
			Size = 128;
			WScale = -128;
		}
		vw_DrawText(DstRect.left+(DstRect.right-DstRect.left-Size)/2, DstRect.bottom-70, WScale, 0, 1.0f, tmpColor, Current*MenuContentTransp, vw_GetText(GetSystemName(GameConfig().Profile[CurrentProfile].PowerSystem + 4)));
	}



	// Target
	DstRect(GameConfig().InternalWidth/2+337, 290, GameConfig().InternalWidth/2+437, 290+100);
	tmpColor = sRGBCOLOR{eRGBCOLOR::white};

	Current = 0.5f;
	if ((CurrentSystemStockNum >= 9) && (CurrentSystemStockNum <= 12)) {
		if (CanBuy)
			tmpColor = sRGBCOLOR{0.0f, 0.8f, 0.0f};
		else
			tmpColor = sRGBCOLOR{eRGBCOLOR::orange};

		Current = 1.0f;
	}

	if (vw_MouseOverRect(DstRect) && !isDialogBoxDrawing()) {
		if (NeedPlayWorkshopOnButtonSoundX != DstRect.left || NeedPlayWorkshopOnButtonSoundY != DstRect.top) {
			PlayMenuSFX(eMenuSFX::OverBigButton, 1.0f);
			NeedPlayWorkshopOnButtonSoundX = DstRect.left;
			NeedPlayWorkshopOnButtonSoundY = DstRect.top;
		}

		Current = 1.0f;
		CurrentCursorStatus = 1;
		if (vw_GetMouseLeftClick(true)) {
			CurrentSystemStockNum = 9;
			PlayMenuSFX(eMenuSFX::Click, 1.0f);
		}
	} else {
		if (NeedPlayWorkshopOnButtonSoundX == DstRect.left && NeedPlayWorkshopOnButtonSoundY == DstRect.top) {
			NeedPlayWorkshopOnButtonSoundX = 0;
			NeedPlayWorkshopOnButtonSoundY = 0;
		}
	}

	if (GameConfig().Profile[CurrentProfile].TargetingSystem <= 0) {
		vw_Draw2D(DstRect, SrcRect, GetSystemIcon(-1), true, Current*MenuContentTransp);

		Size = vw_TextWidth(vw_GetText("empty"));
		WScale = 0;
		if (Size > 128) {
			Size = 128;
			WScale = -128;
		}
		vw_DrawText(DstRect.left+(DstRect.right-DstRect.left-Size)/2, DstRect.top-4, WScale, 0, 1.0f, tmpColor, Current*MenuContentTransp, vw_GetText("empty"));

		Size = vw_TextWidth(vw_GetText("Optical Computer"));
		WScale = 0;
		if (Size > 128) {
			Size = 128;
			WScale = -128;
		}
		vw_DrawText(DstRect.left+(DstRect.right-DstRect.left-Size)/2, DstRect.bottom-20, WScale, 0, 1.0f, tmpColor, Current*MenuContentTransp, vw_GetText("Optical Computer"));
	} else {
		vw_Draw2D(DstRect, SrcRect, GetSystemIcon(GameConfig().Profile[CurrentProfile].TargetingSystem + 8), true, Current * MenuContentTransp);

		Size = vw_TextWidth(vw_GetText(GetSystemName(GameConfig().Profile[CurrentProfile].TargetingSystem + 8)));
		WScale = 0;
		if (Size > 128) {
			Size = 128;
			WScale = -128;
		}
		vw_DrawText(DstRect.left+(DstRect.right - DstRect.left-Size) / 2, DstRect.bottom-70, WScale, 0, 1.0f, tmpColor, Current * MenuContentTransp, vw_GetText(GetSystemName(GameConfig().Profile[CurrentProfile].TargetingSystem + 8)));
	}



	// Mech
	DstRect(GameConfig().InternalWidth/2+337, 400, GameConfig().InternalWidth/2+437, 400+100);
	tmpColor = sRGBCOLOR{eRGBCOLOR::white};

	Current = 0.5f;
	if ((CurrentSystemStockNum >= 13) && (CurrentSystemStockNum <= 16)) {
		if (CanBuy)
			tmpColor = sRGBCOLOR{0.0f, 0.8f, 0.0f};
		else
			tmpColor = sRGBCOLOR{eRGBCOLOR::orange};

		Current = 1.0f;
	}

	if (vw_MouseOverRect(DstRect) && !isDialogBoxDrawing()) {
		if ((NeedPlayWorkshopOnButtonSoundX != DstRect.left) ||
		    (NeedPlayWorkshopOnButtonSoundY != DstRect.top)) {
			PlayMenuSFX(eMenuSFX::OverBigButton, 1.0f);
			NeedPlayWorkshopOnButtonSoundX = DstRect.left;
			NeedPlayWorkshopOnButtonSoundY = DstRect.top;
		}

		Current = 1.0f;
		CurrentCursorStatus = 1;
		if (vw_GetMouseLeftClick(true)) {
			PlayMenuSFX(eMenuSFX::Click, 1.0f);
			CurrentSystemStockNum = 13;
		}
	} else {
		if ((NeedPlayWorkshopOnButtonSoundX == DstRect.left) &&
		    (NeedPlayWorkshopOnButtonSoundY == DstRect.top)) {
			NeedPlayWorkshopOnButtonSoundX = 0;
			NeedPlayWorkshopOnButtonSoundY = 0;
		}
	}

	if (GameConfig().Profile[CurrentProfile].TargetingMechanicSystem <= 0) {
		vw_Draw2D(DstRect, SrcRect, GetSystemIcon(-3), true, Current*MenuContentTransp);

		Size = vw_TextWidth(vw_GetText("empty"));
		WScale = 0;
		if (Size > 128) {
			Size = 128;
			WScale = -128;
		}
		vw_DrawText(DstRect.left+(DstRect.right-DstRect.left-Size)/2, DstRect.top-4, WScale, 0, 1.0f, tmpColor, Current*MenuContentTransp, vw_GetText("empty"));

		Size = vw_TextWidth(vw_GetText("Targeting System"));
		WScale = 0;
		if (Size > 128) {
			Size = 128;
			WScale = -128;
		}
		vw_DrawText(DstRect.left+(DstRect.right-DstRect.left-Size)/2, DstRect.bottom-20, WScale, 0, 1.0f, tmpColor, Current*MenuContentTransp, vw_GetText("Targeting System"));
	} else {
		vw_Draw2D(DstRect, SrcRect, GetSystemIcon(GameConfig().Profile[CurrentProfile].TargetingMechanicSystem + 12), true, Current * MenuContentTransp);

		Size = vw_TextWidth(vw_GetText(GetSystemName(GameConfig().Profile[CurrentProfile].TargetingMechanicSystem + 12)));
		WScale = 0;
		if (Size > 128) {
			Size = 128;
			WScale = -128;
		}
		vw_DrawText(DstRect.left+(DstRect.right-DstRect.left-Size)/2, DstRect.bottom-70, WScale, 0, 1.0f, tmpColor, Current*MenuContentTransp, vw_GetText(GetSystemName(GameConfig().Profile[CurrentProfile].TargetingMechanicSystem + 12)));
	}



	// Protect
	DstRect(GameConfig().InternalWidth/2+337, 510, GameConfig().InternalWidth/2+437, 510+100);
	tmpColor = sRGBCOLOR{eRGBCOLOR::white};

	if (NeedMoreEnergy && (GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem != 0))
		tmpColor = sRGBCOLOR{eRGBCOLOR::red};

	Current = 0.5f;
	if ((CurrentSystemStockNum >= 17) && (CurrentSystemStockNum <= 20)) {
		if (!NeedMoreEnergy || (GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem == 0)) {
			if (CanBuy)
				tmpColor = sRGBCOLOR{0.0f, 0.8f, 0.0f};
			else
				tmpColor = sRGBCOLOR{eRGBCOLOR::orange};
		}
		Current = 1.0f;
	}

	if (vw_MouseOverRect(DstRect) && !isDialogBoxDrawing()) {
		if (NeedPlayWorkshopOnButtonSoundX != DstRect.left || NeedPlayWorkshopOnButtonSoundY != DstRect.top) {
			PlayMenuSFX(eMenuSFX::OverBigButton, 1.0f);
			NeedPlayWorkshopOnButtonSoundX = DstRect.left;
			NeedPlayWorkshopOnButtonSoundY = DstRect.top;
		}

		Current = 1.0f;
		CurrentCursorStatus = 1;
		if (vw_GetMouseLeftClick(true)) {
			PlayMenuSFX(eMenuSFX::Click, 1.0f);
			CurrentSystemStockNum = 17;
		}
	} else {
		if (NeedPlayWorkshopOnButtonSoundX == DstRect.left && NeedPlayWorkshopOnButtonSoundY == DstRect.top) {
			NeedPlayWorkshopOnButtonSoundX = 0;
			NeedPlayWorkshopOnButtonSoundY = 0;
		}
	}

	if (GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem <= 0) {
		vw_Draw2D(DstRect, SrcRect, GetSystemIcon(0), true, Current*MenuContentTransp);

		Size = vw_TextWidth(vw_GetText("empty"));
		WScale = 0;
		if (Size > 128) {
			Size = 128;
			WScale = -128;
		}
		vw_DrawText(DstRect.left+(DstRect.right-DstRect.left-Size)/2, DstRect.top-4, WScale, 0, 1.0f, tmpColor, Current*MenuContentTransp, vw_GetText("empty"));

		Size = vw_TextWidth(vw_GetText("Advanced System"));
		WScale = 0;
		if (Size > 128) {
			Size = 128;
			WScale = -128;
		}
		vw_DrawText(DstRect.left+(DstRect.right-DstRect.left-Size)/2, DstRect.bottom-20, WScale, 0, 1.0f, tmpColor, Current*MenuContentTransp, vw_GetText("Advanced System"));

	} else {
		vw_Draw2D(DstRect, SrcRect, GetSystemIcon(GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem + 16), true, Current*MenuContentTransp);

		Size = vw_TextWidth(vw_GetText(GetSystemName(GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem + 16)));
		WScale = 0;
		if (Size > 128) {
			Size = 128;
			WScale = -128;
		}
		vw_DrawText(DstRect.left+(DstRect.right-DstRect.left-Size)/2, DstRect.bottom-70, WScale, 0, 1.0f, tmpColor, Current*MenuContentTransp, vw_GetText(GetSystemName(GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem + 16)));
	}



	vw_SetFontSize(24);
	vw_DrawText(GameConfig().InternalWidth/2+475-vw_TextWidth(vw_GetText("Installed Systems")), 630, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetText("Installed Systems"));
	ResetFontSize();

	// текущая система
	vw_DrawText(GameConfig().InternalWidth/2-250, 430, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetText("Selected System"));
	vw_DrawText(GameConfig().InternalWidth/2+250-vw_TextWidth(vw_GetText("Installed System")), 430, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetText("Installed System"));

	if (CanBuy)
		vw_DrawText(GameConfig().InternalWidth/2-250, 485, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, "%s: %i", vw_GetText("Cost"), GetSystemCost(CurrentSystemStockNum));
	else
		vw_DrawText(GameConfig().InternalWidth/2-250, 485, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, CurrentAlert3*MenuContentTransp, "%s: %i", vw_GetText("Cost"), GetSystemCost(CurrentSystemStockNum));

	vw_DrawText(GameConfig().InternalWidth/2+250-vw_TextWidth("%s: %i", vw_GetText("Cost"), Cost), 485, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, "%s: %i", vw_GetText("Cost"), Cost);


	if (DrawButton128_2(GameConfig().InternalWidth/2-250,580-55, vw_GetText("Info"), MenuContentTransp, false)) {
		SetCurrentDialogBox(eDialogBox::ShowSystemsInfo);
		DialogSystem = CurrentSystemStockNum;
	}

	if (DrawButton128_2(GameConfig().InternalWidth/2+250-128, 580-55, vw_GetText("Info"), MenuContentTransp, !CanSell)) {
		SetCurrentDialogBox(eDialogBox::ShowSystemsInfo);

		NeedMoreEnergyDialog = NeedMoreEnergy;

		if ((CurrentSystemStockNum >= 1) && (CurrentSystemStockNum <= 4))
			DialogSystem = GameConfig().Profile[CurrentProfile].EngineSystem;
		if ((CurrentSystemStockNum >= 5) && (CurrentSystemStockNum <= 8))
			DialogSystem = GameConfig().Profile[CurrentProfile].PowerSystem + 4;
		if ((CurrentSystemStockNum >= 9) && (CurrentSystemStockNum <= 12))
			DialogSystem = GameConfig().Profile[CurrentProfile].TargetingSystem + 8;
		if ((CurrentSystemStockNum >= 13) && (CurrentSystemStockNum <= 16))
			DialogSystem = GameConfig().Profile[CurrentProfile].TargetingMechanicSystem + 12;
		if ((CurrentSystemStockNum >= 17) && (CurrentSystemStockNum <= 20))
			DialogSystem = GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem + 16;
	}


	if (DrawButton128_2(GameConfig().InternalWidth/2-250, 50+580-55, vw_GetText("Buy"), MenuContentTransp, !CanBuy)) {
		BuyCurrentSystem();
	}
	// покупка, если 2 раза кликнули на иконку текущей системы
	DstRect(GameConfig().InternalWidth/2-451, 55+128+20, GameConfig().InternalWidth/2-451+128, 55+128+128+20);
	if (vw_MouseOverRect(DstRect) && !isDialogBoxDrawing() && CanBuy && vw_GetMouseLeftDoubleClick(true))
		BuyCurrentSystem();



	if (DrawButton128_2(GameConfig().InternalWidth/2+250-128, 50+580-55, vw_GetText("Sell"), MenuContentTransp, !CanSell))
		SellCurrentSystem();


	// вывод информации
	vw_SetFontSize(20);
	int SizeI = (GameConfig().InternalWidth-vw_TextWidth("%s: %i", vw_GetText("Money"), GameConfig().Profile[CurrentProfile].Money)) / 2;
	if (CanBuy)
		vw_DrawText(SizeI, 630, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, "%s: %i", vw_GetText("Money"), GameConfig().Profile[CurrentProfile].Money);
	else
		vw_DrawText(SizeI, 630, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, CurrentAlert3*MenuContentTransp, "%s: %i", vw_GetText("Money"), GameConfig().Profile[CurrentProfile].Money);
	ResetFontSize();
}

} // astromenace namespace
} // viewizard namespace
