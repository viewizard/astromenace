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
#include "../platform/platform.h"
#include "../ui/font.h"
#include "../assets/audio.h"
#include "../assets/texture.h"
#include "../object3d/weapon/weapon.h"
#include "../object3d/space_ship/earth_space_fighter/earth_space_fighter.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

extern cEarthSpaceFighter *WorkshopFighterGame;
extern cWeapon *WorkshopNewWeapon;
extern int	CurrentWorkshopNewWeapon;
extern float CurrentDeviationSum;
extern sVECTOR3D WorkShopPointCamera;
extern float CurrentAlert2;
extern float CurrentAlert3;

void WorkshopCreateNewWeapon();
void WorkshopDrawShip(cEarthSpaceFighter *SpaceFighter, int Mode);
void WorkshopDrawWeapon(cWeapon *Weapon);

int GetWeaponHullDamage(int Num);
int GetWeaponSystemsDamage(int Num);

// что рисовать в диалоге 6,7,8
extern cWeapon *DialogWeapon;


// флаг-тянем
bool DragWeapon = false;
// номер оружия, которое тянем
int DragWeaponNum = 0;
// уровень оружия, которое тянем
int DragWeaponLevel = 0;
// кол-во боекомплекта в оружии
int DragWeaponAmmo = 0;
// общее кол-во боекомплекта в заполненном боекомплекте
int DragWeaponAmmoStart = 0;
// управление
int DragWeaponControl = 0;
int DragWeaponAltControl = 0;
int DragWeaponAltControlData = 0;


// сюда вытягиваем данные
int	NewWeaponControlType = 0;
int	NewWeaponControlTypeData = 0;



// вывод большого меню с настройкой оружия
// передаем только номер слота оружия
int WeaponSetupSlot = -1;

// Номер, для проигрывания голосового сообщения об отсутствии снарядов в боекомплекте
unsigned int VoiceAmmoOut{0};







int NextWeaponGroup()
{
	if (CurrentWorkshopNewWeapon>=1 && CurrentWorkshopNewWeapon<=4) return 5;
	if (CurrentWorkshopNewWeapon>=5 && CurrentWorkshopNewWeapon<=7) return 8;
	if (CurrentWorkshopNewWeapon>=8 && CurrentWorkshopNewWeapon<=10) return 11;
	if (CurrentWorkshopNewWeapon>=11 && CurrentWorkshopNewWeapon<=12) return 13;
	if (CurrentWorkshopNewWeapon>=13 && CurrentWorkshopNewWeapon<=13) return 14;
	if (CurrentWorkshopNewWeapon>=14 && CurrentWorkshopNewWeapon<=14) return 15;
	if (CurrentWorkshopNewWeapon>=15 && CurrentWorkshopNewWeapon<=15) return 16;
	if (CurrentWorkshopNewWeapon>=16 && CurrentWorkshopNewWeapon<=19) return 1;

	return 0;
}
int PrevWeaponGroup()
{
	if (CurrentWorkshopNewWeapon>=1 && CurrentWorkshopNewWeapon<=4) return 16;
	if (CurrentWorkshopNewWeapon>=5 && CurrentWorkshopNewWeapon<=7) return 1;
	if (CurrentWorkshopNewWeapon>=8 && CurrentWorkshopNewWeapon<=10) return 5;
	if (CurrentWorkshopNewWeapon>=11 && CurrentWorkshopNewWeapon<=12) return 8;
	if (CurrentWorkshopNewWeapon>=13 && CurrentWorkshopNewWeapon<=13) return 11;
	if (CurrentWorkshopNewWeapon>=14 && CurrentWorkshopNewWeapon<=14) return 13;
	if (CurrentWorkshopNewWeapon>=15 && CurrentWorkshopNewWeapon<=15) return 14;
	if (CurrentWorkshopNewWeapon>=16 && CurrentWorkshopNewWeapon<=19) return 15;

	return 0;
}






int GetWeaponBaseCost(int Num)
{
	switch (Num) {
	case 1:
		return 150;
	case 2:
		return 300;
	case 3:
		return 1000;
	case 4:
		return 1500;

	case 5:
		return 250;
	case 6:
		return 1000;
	case 7:
		return 1900;

	case 8:
		return 1700;
	case 9:
		return 2500;
	case 10:
		return 4500;

	case 11:
		return 2500;
	case 12:
		return 5000;

	case 13:
		return 6000;

	case 14:
		return 8000;

	case 15:
		return 10000;

	case 16:
		return 1500;
	case 17:
		return 4000;
	case 18:
		return 5000;
	case 19:
		return 8000;

	default:
		return 0;
	}
}
int GetWeaponCost(int Num, int Ammo, int AmmoStart)
{
	// получаем базовую стоимость оружия
	int Cost = GetWeaponBaseCost(Num);
	// находим базовую стоимость оружия
	int BaseCost = Cost/2;
	// берем половину стоимости, все четное, но на всякий случай так
	Cost -= BaseCost;
	// находим стоимость боекомплекта
	BaseCost += (int)(Cost*((Ammo*1.0f)/(AmmoStart*1.0f)));

	return BaseCost;
}
int GetWeaponReloadCost(int Num, int Ammo, int AmmoStart)
{
	// получаем базовую стоимость оружия
	int Cost = GetWeaponBaseCost(Num);
	// находим базовую стоимость оружия
	int BaseCost = Cost/2;
	// берем половину стоимости, все четное, но на всякий случай так
	Cost -= BaseCost;
	// находим стоимость боекомплекта
	return Cost - (int)(Cost*((Ammo*1.0f)/(AmmoStart*1.0f)));
}


// название типа оружия землян
const char *GetWeaponGroupTitle(int Num)
{
	switch (Num) {
	case 1:
	case 2:
	case 3:
	case 4:
		return "Kinetic";
	case 5:
	case 6:
	case 7:
		return "Ion";
	case 8:
	case 9:
	case 10:
		return "Plasma";
	case 11:
	case 12:
		return "Maser";
	case 13:
		return "Antimatter";
	case 14:
		return "Laser";
	case 15:
		return "Gauss";

	case 16:
	case 17:
	case 18:
	case 19:
		return "Propelled";

	default:
		std::cerr << __func__ << "(): " << "wrong Num.\n";
		break;
	}
	return nullptr;
}

const char *GetWeaponName(int Num)
{
	switch (Num) {
	case 1:
		return "FireFox";
	case 2:
		return "FireBird";
	case 3:
		return "Meteor";
	case 4:
		return "Hasher";

	case 5:
		return "Trammel";
	case 6:
		return "IonStorm";
	case 7:
		return "IonRipple";

	case 8:
		return "Quasher";
	case 9:
		return "Destabilizer";
	case 10:
		return "Annihilator";

	case 11:
		return "Devastator";
	case 12:
		return "DeathFlow";

	case 13:
		return "Vaporizer";

	case 14:
		return "Rapier";

	case 15:
		return "Pandora";

	case 16:
		return "Stinger";
	case 17:
		return "Swarm";
	case 18:
		return "Vengeance";
	case 19:
		return "BabyDoll";

	default:
		std::cerr << __func__ << "(): " << "wrong Num.\n";
		break;
	}

	return nullptr;
}





const char *GetWeaponIconName(int Num)
{
	switch (Num) {
	case 1:
		return "menu/weapon1_icon.tga";
	case 2:
		return "menu/weapon2_icon.tga";
	case 3:
		return "menu/weapon3_icon.tga";
	case 4:
		return "menu/weapon4_icon.tga";
	case 5:
		return "menu/weapon5_icon.tga";
	case 6:
		return "menu/weapon6_icon.tga";
	case 7:
		return "menu/weapon7_icon.tga";
	case 8:
		return "menu/weapon8_icon.tga";
	case 9:
		return "menu/weapon9_icon.tga";
	case 10:
		return "menu/weapon10_icon.tga";
	case 11:
		return "menu/weapon11_icon.tga";
	case 12:
		return "menu/weapon12_icon.tga";
	case 13:
		return "menu/weapon13_icon.tga";
	case 14:
		return "menu/weapon14_icon.tga";
	case 15:
		return "menu/weapon15_icon.tga";
	case 16:
		return "menu/weapon16_icon.tga";
	case 17:
		return "menu/weapon17_icon.tga";
	case 18:
		return "menu/weapon18_icon.tga";
	case 19:
		return "menu/weapon19_icon.tga";

	default:
		std::cerr << __func__ << "(): " << "wrong Num.\n";
		break;
	}
	return nullptr;
}




// вся работа с еденичным слотом
void ShipSlotWeapon(int SlotNum, int X, int Y)
{
	sRECT SrcRect(0, 0, 220, 128);
	int Xpos = X-45;
	int Ypos = Y-36;
	sRECT DstRect(Xpos, Ypos, Xpos + 220, Ypos + 128);
	vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/workshop_panel3.tga"), true, MenuContentTransp);


	// нужны предупреждения...
	bool WeaponAmmoOut = false;

	// отображаем боекомплект
	if (auto sharedWeapon = WorkshopFighterGame->WeaponSlots[SlotNum].Weapon.lock()) {
		int AmmoShow = (int)((56.0f*(sharedWeapon->AmmoStart-sharedWeapon->Ammo))/sharedWeapon->AmmoStart);
		// если меняли боекомплект и сделали его меньше, чтобы не вылазила линия боекомплекта...
		if (AmmoShow < 0)
			AmmoShow = 0;

		SrcRect(0,AmmoShow,18,56);
		DstRect(Xpos+23,Ypos+40+AmmoShow,Xpos+18+23,Ypos+56+40);
		if (AmmoShow > 0)
			vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/ammo.tga"), true, CurrentAlert3*MenuContentTransp);
		else
			vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/ammo.tga"), true, MenuContentTransp);

		if (sharedWeapon->Ammo == 0)
			WeaponAmmoOut = true;
	}

	// запускаем голос...
	if (WeaponAmmoOut &&
	    !vw_IsSoundAvailable(VoiceAmmoOut)) // уже не играем, нужно запустить опять
		VoiceAmmoOut = PlayVoicePhrase(eVoicePhrase::WeaponMalfunction, 1.0f);


	// кнопка Setup
	int MouseX, MouseY;
	vw_GetMousePos(MouseX, MouseY);

	// работаем с клавиатурой
	if ((MenuContentTransp >= 0.99f) && !isDialogBoxDrawing() && !DragWeapon) CurrentActiveMenuElement++;
	bool InFocusByKeyboard = false;
	if (CurrentKeyboardSelectMenuElement > 0) {
		if (CurrentKeyboardSelectMenuElement == CurrentActiveMenuElement) {
			InFocusByKeyboard = true;
		}
	}

	SrcRect(0,0,18,56);
	DstRect(Xpos+23+154,Ypos+40,Xpos+18+23+154,Ypos+56+40);
	if  ((((DstRect.right  >= MouseX) &&
	       (DstRect.left<= MouseX) &&
	       (DstRect.bottom >= MouseY) &&
	       (DstRect.top<= MouseY)) || InFocusByKeyboard) && !isDialogBoxDrawing() && !DragWeapon) {
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(vw_GetText("lang/en/menu/button_weaponry_in.tga")), true, MenuContentTransp);
		CurrentCursorStatus = 1;
		if (vw_GetMouseLeftClick(true) || (InFocusByKeyboard && (vw_GetKeyStatus(SDLK_KP_ENTER) || vw_GetKeyStatus(SDLK_RETURN)))) {
			PlayMenuSFX(eMenuSFX::Click, 1.0f);
			WeaponSetupSlot = SlotNum;
			if (InFocusByKeyboard) {
				vw_SetKeyStatus(SDLK_KP_ENTER, false);
				vw_SetKeyStatus(SDLK_RETURN, false);
			}
		}
	} else
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(vw_GetText("lang/en/menu/button_weaponry_out.tga")), true, MenuContentTransp);



	// обработка перетягивания
	DstRect(X,Y,X+128,Y+64);
	if (vw_MouseOverRect(DstRect) && !isDialogBoxDrawing()) {

		int Money = GameConfig().Profile[CurrentProfile].Money;
		if (auto sharedWeapon = WorkshopFighterGame->WeaponSlots[SlotNum].Weapon.lock())
			Money += GetWeaponCost(sharedWeapon->InternalType,
					       sharedWeapon->Ammo,
					       sharedWeapon->AmmoStart);

		// если отпустили тут
		if (!vw_GetMouseLeftClick(false) && DragWeapon) {
			// есть уровень слота соотв. уровню оружия
			if ((WorkshopFighterGame->WeaponSlots[SlotNum].Type >= DragWeaponLevel) &&
			    // если стоимость меньше чем есть денег + стоимость оружия
			    (Money >= GetWeaponCost(DragWeaponNum, DragWeaponAmmo, DragWeaponAmmoStart))) {
				// звук устанавливаемого оружия
				PlayMenuSFX(eMenuSFX::DragInstallToSlot, 1.0f);

				// если тут было оружие - сначало продаем его
				if (auto sharedWeapon = WorkshopFighterGame->WeaponSlots[SlotNum].Weapon.lock()) {
					ChangeGameConfig().Profile[CurrentProfile].Money +=
						GetWeaponCost(sharedWeapon->InternalType,
							      sharedWeapon->Ammo,
							      sharedWeapon->AmmoStart);
					ReleaseWeapon(WorkshopFighterGame->WeaponSlots[SlotNum].Weapon);
					ChangeGameConfig().Profile[CurrentProfile].Weapon[SlotNum] = 0;
					ChangeGameConfig().Profile[CurrentProfile].WeaponAmmo[SlotNum] = 0;
				}

				// покупаем оружие
				ChangeGameConfig().Profile[CurrentProfile].Money -= GetWeaponCost(DragWeaponNum, DragWeaponAmmo, DragWeaponAmmoStart);

				// все проверки сделали до этого, можем просто вызвать функцию, там 100% будет оружие
				SetEarthSpaceFighterWeapon(WorkshopFighterGame, SlotNum+1, DragWeaponNum);
				// убираем источник света
				if (auto sharedWeapon = WorkshopFighterGame->WeaponSlots[SlotNum].Weapon.lock()) {
					if (auto sharedFire = sharedWeapon->Fire.lock())
						vw_ReleaseLight(sharedFire->Light);

					ChangeGameConfig().Profile[CurrentProfile].Weapon[SlotNum] = DragWeaponNum;
					ChangeGameConfig().Profile[CurrentProfile].WeaponAmmo[SlotNum] = DragWeaponAmmo;
					ChangeGameConfig().Profile[CurrentProfile].WeaponControl[SlotNum] = DragWeaponControl;
					ChangeGameConfig().Profile[CurrentProfile].WeaponAltControl[SlotNum] = DragWeaponAltControl;
					ChangeGameConfig().Profile[CurrentProfile].WeaponAltControlData[SlotNum] = DragWeaponAltControlData;
					sharedWeapon->Ammo = DragWeaponAmmo;
					sharedWeapon->AmmoStart = DragWeaponAmmoStart;

					// если не ракетная установка
					if (DragWeaponNum < 16) {
						WorkshopFighterGame->WeaponSlots[SlotNum].YAngle = -GameConfig().Profile[CurrentProfile].WeaponSlotYAngle[SlotNum];
						sVECTOR3D NeedAngle = WorkshopFighterGame->Rotation;
						NeedAngle.y += WorkshopFighterGame->WeaponSlots[SlotNum].YAngle;
						sharedWeapon->SetRotation(NeedAngle);
					} else
						ChangeGameConfig().Profile[CurrentProfile].WeaponSlotYAngle[SlotNum] = 0.0f;
				}

				// чтобы оружие заняло свое место...
				WorkshopFighterGame->Update(vw_GetTimeThread(0));

				// сброс
				DragWeapon = false;
				DragWeaponNum = 0;
				DragWeaponLevel = 0;
				DragWeaponAmmo = 0;
				DragWeaponAmmoStart = 0;
				DragWeaponControl = 0;
				DragWeaponAltControl = 0;
				DragWeaponAltControlData = 0;
			} else {
				// особый случай - есть не соответствие, нужно проиграть звук неудачной установки
				PlayMenuSFX(eMenuSFX::DragError, 1.0f);

				// сброс
				DragWeapon = false;
				DragWeaponNum = 0;
				DragWeaponLevel = 0;
				DragWeaponAmmo = 0;
				DragWeaponAmmoStart = 0;
				DragWeaponControl = 0;
				DragWeaponAltControl = 0;
				DragWeaponAltControlData = 0;
			}
		}


		// взяли оружие из слота, чтобы тащить - фактически продали его
		if (vw_GetMouseLeftClick(false) &&
		    !DragWeapon) {
			if (auto sharedWeapon = WorkshopFighterGame->WeaponSlots[SlotNum].Weapon.lock()) {
				// звук снятия оружия из слота
				PlayMenuSFX(eMenuSFX::DragUninstallFromSlot, 1.0f);

				DragWeapon = true;
				DragWeaponNum = sharedWeapon->InternalType;
				DragWeaponLevel = sharedWeapon->WeaponLevel;
				DragWeaponAmmo = sharedWeapon->Ammo;
				DragWeaponAmmoStart = sharedWeapon->AmmoStart;
				DragWeaponControl = GameConfig().Profile[CurrentProfile].WeaponControl[SlotNum];
				DragWeaponAltControl = GameConfig().Profile[CurrentProfile].WeaponAltControl[SlotNum];
				DragWeaponAltControlData = GameConfig().Profile[CurrentProfile].WeaponAltControlData[SlotNum];

				ChangeGameConfig().Profile[CurrentProfile].Money +=
					GetWeaponCost(sharedWeapon->InternalType,
						      sharedWeapon->Ammo,
						      sharedWeapon->AmmoStart);
				ReleaseWeapon(WorkshopFighterGame->WeaponSlots[SlotNum].Weapon);
				ChangeGameConfig().Profile[CurrentProfile].Weapon[SlotNum] = 0;
				ChangeGameConfig().Profile[CurrentProfile].WeaponAmmo[SlotNum] = 0;
			}
		}



		// если не тянем и в слоте что-то есть, показываем, что можем тянуть
		if (!DragWeapon &&
		    !WorkshopFighterGame->WeaponSlots[SlotNum].Weapon.expired())
			CurrentCursorStatus = 1;

	}




	// рисуем можем ставить в слот или нет
	bool CanOn = false;
	// проверка по уровню слота
	if (DragWeapon && (WorkshopFighterGame->WeaponSlots[SlotNum].Type >= DragWeaponLevel))
		CanOn = true;
	if (!DragWeapon && (WorkshopFighterGame->WeaponSlots[SlotNum].Type >= WorkshopNewWeapon->WeaponLevel))
		CanOn = true;

	SrcRect(0,0,128,64);
	DstRect(X,Y,X+128,Y+64);

	if (CanOn) {
		if (!WeaponAmmoOut) {
			bool NeedAlert = false;
			// если есть хоть 1 слот меньшего уровня
			for (unsigned i=0; i<WorkshopFighterGame->WeaponSlots.size(); i++) {
				// если что-то тянем, смотрим по нему...
				if (DragWeapon) {
					if (WorkshopFighterGame->WeaponSlots[i].Type < DragWeaponLevel) NeedAlert = true;
				} else {
					if (WorkshopFighterGame->WeaponSlots[i].Type < WorkshopNewWeapon->WeaponLevel) NeedAlert = true;
				}
			}

			if (NeedAlert)
				vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/weapon_on_icon.tga"), true, CurrentAlert3*MenuContentTransp, 0.0f, sRGBCOLOR{0.0f, 1.0f, 0.0f});
			else
				vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/weapon_on_icon.tga"), true, MenuContentTransp, 0.0f, sRGBCOLOR{0.0f, 1.0f, 0.0f});
		} else
			vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/weapon_on_icon.tga"), true, CurrentAlert3*MenuContentTransp, 0.0f, sRGBCOLOR{1.0f, 0.0f, 0.0f});
	} else
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/weapon_on_icon.tga"), true, MenuContentTransp, 0.0f, sRGBCOLOR{1.0f, 0.5f, 0.0f});





	// прорисовка
	if (auto sharedWeapon = WorkshopFighterGame->WeaponSlots[SlotNum].Weapon.lock()) {
		SrcRect(0,0,128,64);
		DstRect(X,Y,X+128,Y+64);
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(GetWeaponIconName(sharedWeapon->InternalType)), true, MenuContentTransp);
	} else {
		// пустой слот, рисуем его

		DstRect(X,Y,X+128,Y+64);

		int Size = vw_TextWidth(vw_GetText("EMPTY"));
		float WScale = 0;
		if (Size > 88) {
			Size = 88;
			WScale = -88;
		}
		vw_DrawText(DstRect.left+(DstRect.right-DstRect.left-Size)/2, DstRect.bottom-53, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7*MenuContentTransp, vw_GetText("EMPTY"));

		Size = vw_TextWidth("%s %i",vw_GetText("level"),WorkshopFighterGame->WeaponSlots[SlotNum].Type);
		WScale = 0;
		if (Size > 88) {
			Size = 88;
			WScale = -88;
		}
		vw_DrawText(DstRect.left+(DstRect.right-DstRect.left-Size)/2, DstRect.bottom-32, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7*MenuContentTransp, "%s %i",vw_GetText("level"),WorkshopFighterGame->WeaponSlots[SlotNum].Type);
	}

}






void ShipSlotSetupWeapon(int SlotNum)
{
	sRECT SrcRect, DstRect;

	// проверяем, нужно ли вытягивать что-то или нет...
	CheckMouseKeybJState();

	SrcRect(0,0,404,570);
	int Xpos = GameConfig().InternalWidth / 2 + 55;
	int Ypos = 50-10;
	DstRect(Xpos,Ypos,Xpos+404,Ypos+570);
	vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/workshop_panel5.tga"), true, MenuContentTransp);



	if (auto sharedWeapon = WorkshopFighterGame->WeaponSlots[SlotNum].Weapon.lock()) {
		Ypos += 33;
		// выводим боекомплект   текущий/полный
		Xpos = GameConfig().InternalWidth / 2 + 55 + 50;
		vw_DrawText(Xpos, Ypos, -170, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetText("Weapon Ammo:"));
		Xpos = (GameConfig().InternalWidth/2+512)-55 - 50 - vw_TextWidth("%i/%i", sharedWeapon->Ammo, sharedWeapon->AmmoStart);


		// если все нормально - белым... иначе подмаргиваем
		if (sharedWeapon->Ammo == sharedWeapon->AmmoStart)
			vw_DrawText(Xpos, Ypos, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, "%i/%i", sharedWeapon->Ammo, sharedWeapon->AmmoStart);
		else
			vw_DrawText(Xpos, Ypos, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, CurrentAlert3*MenuContentTransp, "%i/%i", sharedWeapon->Ammo, sharedWeapon->AmmoStart);


		// стоимость перезарядки
		Xpos = GameConfig().InternalWidth / 2 + 55 + 50;
		Ypos += 30;
		vw_DrawText(Xpos, Ypos, -230, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetText("Weapon Reload Cost:"));
		// находим стоимость перезарядки
		int ReloadCost = GetWeaponReloadCost(sharedWeapon->InternalType,
						     sharedWeapon->Ammo,
						     sharedWeapon->AmmoStart);
		Xpos = (GameConfig().InternalWidth/2+512)-55 - 50 - vw_TextWidth("%i", ReloadCost);
		if (ReloadCost == 0)
			vw_DrawText(Xpos, Ypos, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, "%i", ReloadCost);
		else
			vw_DrawText(Xpos, Ypos, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, CurrentAlert3*MenuContentTransp, "%i", ReloadCost);


		// кнопка перезарядить оружие
		Ypos += 40;
		if (DrawButton200_2(GameConfig().InternalWidth / 2 + 155, Ypos, vw_GetText("Reload"), MenuContentTransp, (ReloadCost == 0) || GameConfig().Profile[CurrentProfile].Money<ReloadCost)) {
			sharedWeapon->Ammo = sharedWeapon->AmmoStart;
			ChangeGameConfig().Profile[CurrentProfile].WeaponAmmo[SlotNum] = sharedWeapon->Ammo;
			ChangeGameConfig().Profile[CurrentProfile].Money -= ReloadCost;
		}



		// настройка управления для этого оружия

		bool Status1 = false;
		bool Status2 = false;

		Xpos = GameConfig().InternalWidth/2+55+34 + 16;
		Ypos += 60;
		vw_DrawText(Xpos, Ypos, -300, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetText("Weapon Fire Control:"));
		// вкл-выкл первичного управления
		if ((GameConfig().Profile[CurrentProfile].WeaponControl[SlotNum] == 1) ||
		    (GameConfig().Profile[CurrentProfile].WeaponControl[SlotNum] == 3))
			Status1 = true;
		Xpos = GameConfig().InternalWidth/2+55+54 + 16;
		Ypos += 30;
		DrawCheckBox(Xpos,Ypos, &Status1, vw_GetText("Primary Attack"), MenuContentTransp);
		// вкл-выкл вторичного управления
		if ((GameConfig().Profile[CurrentProfile].WeaponControl[SlotNum] == 2) ||
		    (GameConfig().Profile[CurrentProfile].WeaponControl[SlotNum] == 3))
			Status2 = true;
		Xpos = GameConfig().InternalWidth/2+55+54 + 16;
		Ypos += 40;
		DrawCheckBox(Xpos,Ypos, &Status2, vw_GetText("Secondary Attack"), MenuContentTransp);
		// получаем данны обратно
		ChangeGameConfig().Profile[CurrentProfile].WeaponControl[SlotNum] = 0;
		if (Status1)
			ChangeGameConfig().Profile[CurrentProfile].WeaponControl[SlotNum] += 1;
		if (Status2)
			ChangeGameConfig().Profile[CurrentProfile].WeaponControl[SlotNum] += 2;

		// получение альтернативного управления
		Xpos = GameConfig().InternalWidth/2+55+34 + 16;
		Ypos += 40;
		vw_DrawText(Xpos, Ypos, -300, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetText("Alternative Fire Control:"));

		Ypos += 40;

		std::string TextTmp{"?"};
		// установка надписи на кнопке
		if (NeedCheck != 100) {
			if (NewWeaponControlType != 0) {
				ChangeGameConfig().Profile[CurrentProfile].WeaponAltControl[SlotNum] = NewWeaponControlType;
				ChangeGameConfig().Profile[CurrentProfile].WeaponAltControlData[SlotNum] = NewWeaponControlTypeData;
				NewWeaponControlType = 0;
				NewWeaponControlTypeData = 0;
			}
			if (GameConfig().Profile[CurrentProfile].WeaponAltControl[SlotNum] != 0) {
				if (GameConfig().Profile[CurrentProfile].WeaponAltControl[SlotNum] == 1)
					TextTmp = SDL_GetKeyName(GameConfig().Profile[CurrentProfile].WeaponAltControlData[SlotNum]);
				if (GameConfig().Profile[CurrentProfile].WeaponAltControl[SlotNum] == 2)
					TextTmp = MouseButtonName(GameConfig().Profile[CurrentProfile].WeaponAltControlData[SlotNum]);
				if (GameConfig().Profile[CurrentProfile].WeaponAltControl[SlotNum] == 3)
					TextTmp = JoystickButtonName(GameConfig().Profile[CurrentProfile].WeaponAltControlData[SlotNum]);

			} else TextTmp = (char*)vw_GetText("Click to setup");
		}

		// собственно сама кнопка
		float Transp = 1.0f;
		bool Off = false;
		if (NeedCheck == 100) {
			Transp = But[1];
			Off = true;
		}
		if (DrawButton200_2(GameConfig().InternalWidth / 2+155, Ypos, TextTmp.c_str(), Transp * MenuContentTransp, Off)) {
			NeedCheck = 100;
			vw_ResetMouseButtons();
			NewWeaponControlType = 0;
			NewWeaponControlTypeData = 0;
		}



		if (GameConfig().Profile[CurrentProfile].Weapon[SlotNum] < 16) {

			// выводим угол поворота ствола
			Xpos = GameConfig().InternalWidth/2+55+34 + 16;
			Ypos += 60;
			vw_DrawText(Xpos, Ypos, -300, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetText("Weapon Angle: %2.1f"), GameConfig().Profile[CurrentProfile].WeaponSlotYAngle[SlotNum]);
			Ypos += 40;

			float Min = 0.0f;
			float Max = 0.0f;
			GetShipWeaponSlotAngle(GameConfig().Profile[CurrentProfile].Ship, SlotNum, &Min, &Max);


			if (GameConfig().Profile[CurrentProfile].TargetingSystem <= 2) {
				vw_DrawText(Xpos, Ypos-15, 300, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, 1.0f, vw_GetText("Custom Weapon Angle is use-"));
				vw_DrawText(Xpos, Ypos+5, 300, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, 1.0f, vw_GetText("ful with optical computer"));
				vw_DrawText(Xpos, Ypos+25, 300, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, 1.0f, vw_GetText("system Neo or Supra only."));
			} else {
				if (DrawButton128_2(GameConfig().InternalWidth/2+118, Ypos, vw_GetText("Left"), MenuContentTransp, GameConfig().Profile[CurrentProfile].WeaponSlotYAngle[SlotNum] <= Min)) {
					ChangeGameConfig().Profile[CurrentProfile].WeaponSlotYAngle[SlotNum] -= 5.0f;
					WorkshopFighterGame->WeaponSlots[SlotNum].YAngle = -GameConfig().Profile[CurrentProfile].WeaponSlotYAngle[SlotNum];

					sharedWeapon->SetRotation(sharedWeapon->Rotation^(-1));
					sVECTOR3D NeedAngle = WorkshopFighterGame->Rotation;
					NeedAngle.y += WorkshopFighterGame->WeaponSlots[SlotNum].YAngle;
					sharedWeapon->SetRotation(NeedAngle);
				}
				if (DrawButton128_2(GameConfig().InternalWidth/2+266, Ypos, vw_GetText("Right"), MenuContentTransp, GameConfig().Profile[CurrentProfile].WeaponSlotYAngle[SlotNum] >= Max)) {
					ChangeGameConfig().Profile[CurrentProfile].WeaponSlotYAngle[SlotNum] += 5.0f;
					WorkshopFighterGame->WeaponSlots[SlotNum].YAngle = -GameConfig().Profile[CurrentProfile].WeaponSlotYAngle[SlotNum];

					sharedWeapon->SetRotation(sharedWeapon->Rotation^(-1));
					sVECTOR3D NeedAngle = WorkshopFighterGame->Rotation;
					NeedAngle.y += WorkshopFighterGame->WeaponSlots[SlotNum].YAngle;
					sharedWeapon->SetRotation(NeedAngle);
				}
			}
		}
	} else {
		Xpos += 74;
		Ypos += 128;
		// пустой слот, рисуем его
		SrcRect(0,0,256,256);
		DstRect(Xpos,Ypos,Xpos+256,Ypos+256);
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/weapon_empty_icon.tga"), true, MenuContentTransp);

		int Size = vw_TextWidth(vw_GetText("Empty Weapon Slot"));
		float WScale = 0;
		if (Size > 228) {
			Size = 228;
			WScale = -228;
		}
		vw_DrawText(DstRect.left+(DstRect.right-DstRect.left-Size)/2, DstRect.bottom-40, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetText("Empty Weapon Slot"));
	}





	// обработка перетягивания
	Xpos = GameConfig().InternalWidth / 2 + 55;
	Ypos = 50-10;
	DstRect(Xpos+10,Ypos+10,Xpos+404-10,Ypos+570-10);
	if (vw_MouseOverRect(DstRect) && !isDialogBoxDrawing()) {

		int Money = GameConfig().Profile[CurrentProfile].Money;
		if (auto sharedWeapon = WorkshopFighterGame->WeaponSlots[SlotNum].Weapon.lock())
			Money += GetWeaponCost(sharedWeapon->InternalType,
					       sharedWeapon->Ammo,
					       sharedWeapon->AmmoStart);

		// если отпустили тут
		if (!vw_GetMouseLeftClick(false) && DragWeapon) {
			// есть уровень слота соотв. уровню оружия
			if ((WorkshopFighterGame->WeaponSlots[SlotNum].Type >= DragWeaponLevel) &&
			    // если стоимость меньше чем есть денег + стоимость оружия
			    (Money >= GetWeaponCost(DragWeaponNum, DragWeaponAmmo, DragWeaponAmmoStart))) {
				// звук устанавливаемого оружия
				PlayMenuSFX(eMenuSFX::DragInstallToSlot, 1.0f);

				// если тут было оружие - сначало продаем его
				if (auto sharedWeapon = WorkshopFighterGame->WeaponSlots[SlotNum].Weapon.lock()) {
					ChangeGameConfig().Profile[CurrentProfile].Money +=
							GetWeaponCost(sharedWeapon->InternalType,
								      sharedWeapon->Ammo,
								      sharedWeapon->AmmoStart);
					ReleaseWeapon(WorkshopFighterGame->WeaponSlots[SlotNum].Weapon);
					ChangeGameConfig().Profile[CurrentProfile].Weapon[SlotNum] = 0;
					ChangeGameConfig().Profile[CurrentProfile].WeaponAmmo[SlotNum] = 0;
				}

				// покупаем оружие
				ChangeGameConfig().Profile[CurrentProfile].Money -= GetWeaponCost(DragWeaponNum, DragWeaponAmmo, DragWeaponAmmoStart);

				// все проверки сделали до этого, можем просто вызвать функцию, там 100% будет оружие
				SetEarthSpaceFighterWeapon(WorkshopFighterGame, SlotNum+1, DragWeaponNum);
				// убираем источник света
				if (auto sharedWeapon = WorkshopFighterGame->WeaponSlots[SlotNum].Weapon.lock()) {
					if (auto sharedFire = sharedWeapon->Fire.lock())
						vw_ReleaseLight(sharedFire->Light);

					ChangeGameConfig().Profile[CurrentProfile].Weapon[SlotNum] = DragWeaponNum;
					ChangeGameConfig().Profile[CurrentProfile].WeaponAmmo[SlotNum] = DragWeaponAmmo;
					ChangeGameConfig().Profile[CurrentProfile].WeaponControl[SlotNum] = DragWeaponControl;
					ChangeGameConfig().Profile[CurrentProfile].WeaponAltControl[SlotNum] = DragWeaponAltControl;
					ChangeGameConfig().Profile[CurrentProfile].WeaponAltControlData[SlotNum] = DragWeaponAltControlData;
					sharedWeapon->Ammo = DragWeaponAmmo;
					sharedWeapon->AmmoStart = DragWeaponAmmoStart;

					// если не ракетная установка
					if (DragWeaponNum < 16) {
						WorkshopFighterGame->WeaponSlots[SlotNum].YAngle = -GameConfig().Profile[CurrentProfile].WeaponSlotYAngle[SlotNum];
						sVECTOR3D NeedAngle = WorkshopFighterGame->Rotation;
						NeedAngle.y += WorkshopFighterGame->WeaponSlots[SlotNum].YAngle;
						sharedWeapon->SetRotation(NeedAngle);
					} else
						ChangeGameConfig().Profile[CurrentProfile].WeaponSlotYAngle[SlotNum] = 0.0f;
				}

				// чтобы оружие заняло свое место...
				WorkshopFighterGame->Update(vw_GetTimeThread(0));

				// сброс
				DragWeapon = false;
				DragWeaponNum = 0;
				DragWeaponLevel = 0;
				DragWeaponAmmo = 0;
				DragWeaponAmmoStart = 0;
				DragWeaponControl = 0;
				DragWeaponAltControl = 0;
				DragWeaponAltControlData = 0;
			} else {
				// особый случай - есть не соответствие, нужно проиграть звук неудачной установки
				PlayMenuSFX(eMenuSFX::DragError, 1.0f);

				// сброс
				DragWeapon = false;
				DragWeaponNum = 0;
				DragWeaponLevel = 0;
				DragWeaponAmmo = 0;
				DragWeaponAmmoStart = 0;
				DragWeaponControl = 0;
				DragWeaponAltControl = 0;
				DragWeaponAltControlData = 0;
			}
		}

	}





	//кнопка закрыть
	if (DrawButton200_2(GameConfig().InternalWidth / 2 + 155, 533, vw_GetText("Close"), MenuContentTransp, false)) {
		WeaponSetupSlot = -1;
		NeedCheck = 0;
	}


}













void DrawShipWeaponsInSlots()
{
	// если нужно - выводим настройку нужного оружия
	if (WeaponSetupSlot != -1) {
		ShipSlotSetupWeapon(WeaponSetupSlot);
		return;
	}

	switch (GameConfig().Profile[CurrentProfile].Ship) {
	case 1:
		ShipSlotWeapon(0, GameConfig().InternalWidth/2+50, 530);
		ShipSlotWeapon(1, GameConfig().InternalWidth/2+512-128-50, 530);
		ShipSlotWeapon(2, GameConfig().InternalWidth/2+50, 130);
		ShipSlotWeapon(3, GameConfig().InternalWidth/2+512-128-50, 130);
		ShipSlotWeapon(4, GameConfig().InternalWidth/2+256-64, 330);
		break;
	case 2:
		ShipSlotWeapon(0, GameConfig().InternalWidth/2+50, 530);
		ShipSlotWeapon(1, GameConfig().InternalWidth/2+512-128-50, 530);
		ShipSlotWeapon(2, GameConfig().InternalWidth/2+50, 130);
		ShipSlotWeapon(3, GameConfig().InternalWidth/2+512-128-50, 130);
		break;
	case 3:
		ShipSlotWeapon(0, GameConfig().InternalWidth/2+256-64, 130);
		ShipSlotWeapon(1, GameConfig().InternalWidth/2+50, 530);
		ShipSlotWeapon(2, GameConfig().InternalWidth/2+512-128-50, 530);
		break;
	case 4:
		ShipSlotWeapon(0, GameConfig().InternalWidth/2+50, 530);
		ShipSlotWeapon(1, GameConfig().InternalWidth/2+512-128-50, 530);
		ShipSlotWeapon(2, GameConfig().InternalWidth/2+50, 330);
		ShipSlotWeapon(3, GameConfig().InternalWidth/2+512-128-50, 330);
		ShipSlotWeapon(4, GameConfig().InternalWidth/2+256-64, 130);
		break;
	case 5:
		ShipSlotWeapon(0, GameConfig().InternalWidth/2+50, 330);
		ShipSlotWeapon(1, GameConfig().InternalWidth/2+512-128-50, 330);
		break;
	case 6:
		ShipSlotWeapon(0, GameConfig().InternalWidth/2+50, 530);
		ShipSlotWeapon(1, GameConfig().InternalWidth/2+512-128-50, 530);
		ShipSlotWeapon(2, GameConfig().InternalWidth/2+50, 330);
		ShipSlotWeapon(3, GameConfig().InternalWidth/2+512-128-50, 330);
		ShipSlotWeapon(4, GameConfig().InternalWidth/2+256-64, 130);
		break;
	case 7:
		ShipSlotWeapon(0, GameConfig().InternalWidth/2+256-64, 130);
		ShipSlotWeapon(1, GameConfig().InternalWidth/2+50, 330);
		ShipSlotWeapon(2, GameConfig().InternalWidth/2+512-128-50, 330);
		ShipSlotWeapon(3, GameConfig().InternalWidth/2+50, 530);
		ShipSlotWeapon(4, GameConfig().InternalWidth/2+512-128-50, 530);
		break;
	case 8:
		ShipSlotWeapon(0, GameConfig().InternalWidth/2+50, 330);
		ShipSlotWeapon(1, GameConfig().InternalWidth/2+512-128-50, 330);
		break;
	case 9:
		ShipSlotWeapon(0, GameConfig().InternalWidth/2+50, 330);
		ShipSlotWeapon(1, GameConfig().InternalWidth/2+512-128-50, 330);
		break;
	case 10:
		ShipSlotWeapon(0, GameConfig().InternalWidth/2+50, 530);
		ShipSlotWeapon(1, GameConfig().InternalWidth/2+512-128-50, 530);
		ShipSlotWeapon(2, GameConfig().InternalWidth/2+50, 330);
		ShipSlotWeapon(3, GameConfig().InternalWidth/2+512-128-50, 330);
		ShipSlotWeapon(4, GameConfig().InternalWidth/2+256-64, 130);
		break;
	case 11:
		ShipSlotWeapon(0, GameConfig().InternalWidth/2+50, 530);
		ShipSlotWeapon(1, GameConfig().InternalWidth/2+512-128-50, 530);
		ShipSlotWeapon(2, GameConfig().InternalWidth/2+256-64, 130);
		break;
	case 12:
		ShipSlotWeapon(0, GameConfig().InternalWidth/2+50, 530);
		ShipSlotWeapon(1, GameConfig().InternalWidth/2+512-128-50, 530);
		ShipSlotWeapon(2, GameConfig().InternalWidth/2+256-64, 130);
		break;
	case 13:
		ShipSlotWeapon(0, GameConfig().InternalWidth/2+256-64, 530);
		ShipSlotWeapon(1, GameConfig().InternalWidth/2+50, 330);
		ShipSlotWeapon(2, GameConfig().InternalWidth/2+512-128-50, 330);
		ShipSlotWeapon(3, GameConfig().InternalWidth/2+50, 130);
		ShipSlotWeapon(4, GameConfig().InternalWidth/2+512-128-50, 130);
		break;
	case 14:
		ShipSlotWeapon(0, GameConfig().InternalWidth/2+50, 130);
		ShipSlotWeapon(1, GameConfig().InternalWidth/2+512-128-50, 130);
		ShipSlotWeapon(2, GameConfig().InternalWidth/2+50, 530);
		ShipSlotWeapon(3, GameConfig().InternalWidth/2+512-128-50, 530);
		ShipSlotWeapon(4, GameConfig().InternalWidth/2+256-64, 330);
		break;
	case 15:
		ShipSlotWeapon(0, GameConfig().InternalWidth/2+256-64, 130);
		ShipSlotWeapon(1, GameConfig().InternalWidth/2+50, 530);
		ShipSlotWeapon(2, GameConfig().InternalWidth/2+512-128-50, 530);
		break;
	case 16:
		ShipSlotWeapon(0, GameConfig().InternalWidth/2+256-64, 130);
		ShipSlotWeapon(1, GameConfig().InternalWidth/2+50, 530);
		ShipSlotWeapon(2, GameConfig().InternalWidth/2+512-128-50, 530);
		break;
	case 17:
		ShipSlotWeapon(0, GameConfig().InternalWidth/2+50, 330);
		ShipSlotWeapon(1, GameConfig().InternalWidth/2+512-128-50, 330);
		ShipSlotWeapon(2, GameConfig().InternalWidth/2+50, 530);
		ShipSlotWeapon(3, GameConfig().InternalWidth/2+512-128-50, 530);
		ShipSlotWeapon(4, GameConfig().InternalWidth/2+256-64, 130);
		break;
	case 18:
		ShipSlotWeapon(0, GameConfig().InternalWidth/2+50, 130);
		ShipSlotWeapon(1, GameConfig().InternalWidth/2+512-128-50, 130);
		ShipSlotWeapon(2, GameConfig().InternalWidth/2+50, 530);
		ShipSlotWeapon(3, GameConfig().InternalWidth/2+512-128-50, 530);
		break;
	case 19:
		ShipSlotWeapon(0, GameConfig().InternalWidth/2+256-64, 130);
		ShipSlotWeapon(1, GameConfig().InternalWidth/2+50, 530);
		ShipSlotWeapon(2, GameConfig().InternalWidth/2+512-128-50, 530);
		break;
	case 20:
		ShipSlotWeapon(0, GameConfig().InternalWidth/2+50, 130);
		ShipSlotWeapon(1, GameConfig().InternalWidth/2+512-128-50, 130);
		ShipSlotWeapon(2, GameConfig().InternalWidth/2+50, 530);
		ShipSlotWeapon(3, GameConfig().InternalWidth/2+512-128-50, 530);
		break;
	case 21:
		ShipSlotWeapon(0, GameConfig().InternalWidth/2+50, 530);
		ShipSlotWeapon(1, GameConfig().InternalWidth/2+512-128-50, 530);
		ShipSlotWeapon(2, GameConfig().InternalWidth/2+256-64, 130);
		break;
	case 22:
		ShipSlotWeapon(0, GameConfig().InternalWidth/2+256-64, 330);
		break;
	}

}



int GetShipWeaponsMaxSlotLevel()
{
	int max = 1;

	for (const auto &tmpWeaponSlot : WorkshopFighterGame->WeaponSlots) {
		if (tmpWeaponSlot.Type > max)
			max = tmpWeaponSlot.Type;
	}

	return max;
}

















//------------------------------------------------------------------------------------
// покупка-установка и ремонт вооружения корабля
//------------------------------------------------------------------------------------
void Workshop_Weaponry()
{
	sRECT SrcRect, DstRect;



	// если нажали - установка и тянем
	DstRect(GameConfig().InternalWidth/2-416, 100+32, GameConfig().InternalWidth/2-96, 450-32);
	if (vw_MouseOverRect(DstRect) && !isDialogBoxDrawing())
		if (!DragWeapon) {
			CurrentCursorStatus = 1;

			if (vw_GetMouseLeftClick(false)) {
				// звук взяли оружие
				PlayMenuSFX(eMenuSFX::DragUninstallFromSlot, 1.0f);

				// установка
				DragWeapon = true;
				DragWeaponNum = CurrentWorkshopNewWeapon;
				DragWeaponLevel = WorkshopNewWeapon->WeaponLevel;
				DragWeaponAmmo = WorkshopNewWeapon->Ammo;
				DragWeaponAmmoStart = WorkshopNewWeapon->AmmoStart;
				// второй по умолчанию только для рокет
				if (CurrentWorkshopNewWeapon<=15) DragWeaponControl = 1;
				else DragWeaponControl = 2;
				DragWeaponAltControl = 0;
				DragWeaponAltControlData = 0;

			}
		}


	// затемнение
	SrcRect(0,0,256,256);
	DstRect(GameConfig().InternalWidth/2-480, 100-32, GameConfig().InternalWidth/2-32, 450+32);
	vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/back_spot2.tga"), true, 0.45f * MenuContentTransp);
	DstRect(GameConfig().InternalWidth / 2, 0, GameConfig().InternalWidth/2+512, 622);
	vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/back_spot.tga"), true, 0.35f * MenuContentTransp);


	vw_End2DMode();
	WorkshopDrawShip(WorkshopFighterGame, 4);
	WorkshopDrawWeapon(WorkshopNewWeapon);
	vw_Start2DMode(-1,1);


	// вывод названия
	vw_DrawText(GameConfig().InternalWidth/2-438, 50+6, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetText(GetWeaponName(CurrentWorkshopNewWeapon)));
	if (DrawButton128_2(GameConfig().InternalWidth/2-197, 50, vw_GetText("Info"), MenuContentTransp, false)) {
		SetCurrentDialogBox(eDialogBox::ShowWeaponsInfo);
		DialogWeapon = WorkshopNewWeapon;
	}

	vw_DrawText(GameConfig().InternalWidth/2-438, 110, -170, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, "%s:", vw_GetText("Weapon Type"));
	vw_DrawText(GameConfig().InternalWidth/2-438+175, 110, -184, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetText(GetWeaponGroupTitle(CurrentWorkshopNewWeapon)));


	int k2 = 0;
	if (GetWeaponHullDamage(WorkshopNewWeapon->InternalType) > 0.0f) {
		vw_DrawText(GameConfig().InternalWidth/2-438, 130, -170, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetText("Damage, Hull:"));
		if ((WorkshopNewWeapon->InternalType == 11) ||
		    (WorkshopNewWeapon->InternalType == 12) ||
		    (WorkshopNewWeapon->InternalType == 14))
			vw_DrawText(GameConfig().InternalWidth/2-438+175, 130, -184, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, "%i %s", GetWeaponHullDamage(WorkshopNewWeapon->InternalType), vw_GetText("units/sec"));
		else
			vw_DrawText(GameConfig().InternalWidth/2-438+175, 130, -184, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, "%i %s", GetWeaponHullDamage(WorkshopNewWeapon->InternalType), vw_GetText("units/shot"));

		k2=20;
	}
	if (GetWeaponSystemsDamage(WorkshopNewWeapon->InternalType) > 0.0f) {
		vw_DrawText(GameConfig().InternalWidth/2-438, 130+k2, -170, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetText("Damage, Systems:"));
		if ((WorkshopNewWeapon->InternalType == 11) ||
		    (WorkshopNewWeapon->InternalType == 12) ||
		    (WorkshopNewWeapon->InternalType == 14))
			vw_DrawText(GameConfig().InternalWidth/2-438+175, 130+k2, -184, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, "%i %s", GetWeaponSystemsDamage(WorkshopNewWeapon->InternalType), vw_GetText("units/sec"));
		else
			vw_DrawText(GameConfig().InternalWidth/2-438+175, 130+k2, -184, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, "%i %s", GetWeaponSystemsDamage(WorkshopNewWeapon->InternalType), vw_GetText("units/shot"));

	}



	// вывод уровня оружия
	if (WorkshopNewWeapon->WeaponLevel <= GetShipWeaponsMaxSlotLevel())
		vw_DrawText(GameConfig().InternalWidth/2-438, 400, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, MenuContentTransp, "%s: %i", vw_GetText("Weapon Level"), WorkshopNewWeapon->WeaponLevel);
	else
		vw_DrawText(GameConfig().InternalWidth/2-438, 400, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, CurrentAlert3*MenuContentTransp, "%s: %i", vw_GetText("Weapon Level"), WorkshopNewWeapon->WeaponLevel);

	// вывод стоимости
	if (GameConfig().Profile[CurrentProfile].Money >= GetWeaponBaseCost(CurrentWorkshopNewWeapon)) // всегда ведь новое, считать боекомплект не нужно
		vw_DrawText(GameConfig().InternalWidth/2-438, 420, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, "%s: %i", vw_GetText("Weapon Cost"), GetWeaponBaseCost(CurrentWorkshopNewWeapon));
	else
		vw_DrawText(GameConfig().InternalWidth/2-438, 420, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::red}, CurrentAlert3*MenuContentTransp, "%s: %i", vw_GetText("Weapon Cost"), GetWeaponBaseCost(CurrentWorkshopNewWeapon));



	// рамки
	SrcRect(0,0,400,35 );
	DstRect(GameConfig().InternalWidth/2-457, 100-11, GameConfig().InternalWidth/2-57, 100+35-11);
	vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/workshop_panel4.tga"), true, MenuContentTransp);

	SrcRect(0,0,400,173 );
	DstRect(GameConfig().InternalWidth/2-457, 450-13, GameConfig().InternalWidth/2-57, 450+173-13);
	vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/workshop_panel1.tga"), true, MenuContentTransp);





	// проверяем колесо мышки
	DstRect(GameConfig().InternalWidth/2-457, 100+35-11, GameConfig().InternalWidth/2-57, 450-13);
	if (vw_MouseOverRect(DstRect)) {
		if ((vw_GetWheelStatus() != 0) && !isDialogBoxDrawing()) {
			CurrentWorkshopNewWeapon += vw_GetWheelStatus();

			if (CurrentWorkshopNewWeapon < 1)
				CurrentWorkshopNewWeapon = 19;
			if (CurrentWorkshopNewWeapon > 19)
				CurrentWorkshopNewWeapon = 1;
			WorkshopCreateNewWeapon();

			vw_ResetWheelStatus();
		}
	} else if (vw_GetWheelStatus() != 0) {
		vw_ResetWheelStatus();
	}


	if (DrawButton128_2(GameConfig().InternalWidth/2-395,482, vw_GetText("Prev"), MenuContentTransp, false)) {
		CurrentWorkshopNewWeapon--;
		if (CurrentWorkshopNewWeapon < 1)
			CurrentWorkshopNewWeapon = 19;
		WorkshopCreateNewWeapon();
	}
	if (DrawButton128_2(GameConfig().InternalWidth/2-247,482, vw_GetText("Next"), MenuContentTransp, false)) {
		CurrentWorkshopNewWeapon++;
		if (CurrentWorkshopNewWeapon > 19)
			CurrentWorkshopNewWeapon = 1;
		WorkshopCreateNewWeapon();
	}


	if (DrawButton128_2(GameConfig().InternalWidth/2-395,533, vw_GetText(GetWeaponGroupTitle(PrevWeaponGroup())), MenuContentTransp, false)) {
		CurrentWorkshopNewWeapon = PrevWeaponGroup();
		WorkshopCreateNewWeapon();
	}
	if (DrawButton128_2(GameConfig().InternalWidth/2-247,533, vw_GetText(GetWeaponGroupTitle(NextWeaponGroup())), MenuContentTransp, false)) {
		CurrentWorkshopNewWeapon = NextWeaponGroup();
		WorkshopCreateNewWeapon();
	}

	vw_SetFontSize(24);
	vw_DrawText(GameConfig().InternalWidth/2-445, 600, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetText("Weapon Stock"));
	ResetFontSize();

	DrawShipWeaponsInSlots();


	// кнопка "перезарядить все" оружие
	if (WeaponSetupSlot == -1) {
		int ReloadCost = 0;
		// находим стоимость перезарядки
		for (const auto &tmpWeaponSlot : WorkshopFighterGame->WeaponSlots) {
			if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock())
				ReloadCost += GetWeaponReloadCost(sharedWeapon->InternalType,
								  sharedWeapon->Ammo,
								  sharedWeapon->AmmoStart);
		}
		std::string ButtonName{std::string(vw_GetText("Reload All")) + ": " + std::to_string(ReloadCost)};

		if (DrawButton200_2(GameConfig().InternalWidth/2+153, 50, ButtonName.c_str(), MenuContentTransp, (ReloadCost == 0) || (GameConfig().Profile[CurrentProfile].Money < ReloadCost))) {
			for (unsigned i = 0; i < WorkshopFighterGame->WeaponSlots.size(); i++) {
				if (auto sharedWeapon = WorkshopFighterGame->WeaponSlots[i].Weapon.lock()) {
					sharedWeapon->Ammo = sharedWeapon->AmmoStart;
					ChangeGameConfig().Profile[CurrentProfile].WeaponAmmo[i] = sharedWeapon->Ammo;
				}
			}
			ChangeGameConfig().Profile[CurrentProfile].Money -= ReloadCost;
		}
	}

	vw_SetFontSize(24);
	vw_DrawText(GameConfig().InternalWidth/2+445-vw_TextWidth(vw_GetText("Installed Weapons")), 600, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetText("Installed Weapons"));
	ResetFontSize();

	// вывод информации
	vw_SetFontSize(20);
	int SizeI = (GameConfig().InternalWidth - vw_TextWidth("%s: %i", vw_GetText("Money"), GameConfig().Profile[CurrentProfile].Money)) / 2;
	if (GameConfig().Profile[CurrentProfile].Money >= GetWeaponBaseCost(CurrentWorkshopNewWeapon))
		vw_DrawText(SizeI, 630, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, "%s: %i", vw_GetText("Money"), GameConfig().Profile[CurrentProfile].Money);
	else
		vw_DrawText(SizeI, 630, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::red}, CurrentAlert3*MenuContentTransp, "%s: %i", vw_GetText("Money"), GameConfig().Profile[CurrentProfile].Money);
	ResetFontSize();



	// проверяем состояние, если тянули и отжали, и сюда пришли - значит никто не перехватил, нужно сделать сброс
	if (!vw_GetMouseLeftClick(false) && DragWeapon) {
		// звук пропадающего оружия
		PlayMenuSFX(eMenuSFX::DragRelease, 1.0f);

		// сброс
		DragWeapon = false;
		DragWeaponNum = 0;
		DragWeaponLevel = 0;
		DragWeaponAmmo = 0;
		DragWeaponAmmoStart = 0;
	}



	if (DragWeapon) CurrentCursorStatus = 3;


}

} // astromenace namespace
} // viewizard namespace
