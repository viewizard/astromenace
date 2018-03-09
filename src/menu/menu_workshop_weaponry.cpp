/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
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
#include "../object3d/weapon/weapon.h"
#include "../object3d/space_ship/earth_space_fighter/earth_space_fighter.h"


extern cEarthSpaceFighter *WorkshopFighterGame;
extern cWeapon *WorkshopNewWeapon;
extern int	CurrentWorkshopNewWeapon;
extern float CurentDeviationSum;
extern sVECTOR3D WorkShopPointCamera;
extern float CurrentAlert2;
extern float CurrentAlert3;

void WorkshopCreateNewWeapon();
void WorkshopDrawShip(cEarthSpaceFighter *SpaceFighter, int Mode);
void WorkshopDrawWeapon(cWeapon *Weapon);

char *GetWeaponGroupTitle(int Num);
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
int VoiceAmmoOut = 0;







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



const char *GetWeaponName(int Num)
{
	switch (Num) {
	case 1:
		return "7_weapon1";
	case 2:
		return "7_weapon2";
	case 3:
		return "7_weapon3";
	case 4:
		return "7_weapon4";

	case 5:
		return "7_weapon5";
	case 6:
		return "7_weapon6";
	case 7:
		return "7_weapon7";

	case 8:
		return "7_weapon8";
	case 9:
		return "7_weapon9";
	case 10:
		return "7_weapon10";

	case 11:
		return "7_weapon11";
	case 12:
		return "7_weapon12";

	case 13:
		return "7_weapon13";

	case 14:
		return "7_weapon14";

	case 15:
		return "7_weapon15";

	case 16:
		return "7_weapon16";
	case 17:
		return "7_weapon17";
	case 18:
		return "7_weapon18";
	case 19:
		return "7_weapon19";

	default:
		std::cerr << "Error in GetWeaponName function call, wrong Num.\n";
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
		std::cerr << "Error in GetWeaponIconName function call, wrong Num.\n";
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
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("menu/workshop_panel3.tga"), true, MenuContentTransp);


	// нужны предупреждения...
	bool WeaponAmmoOut = false;

	// отображаем боекомплект
	if (WorkshopFighterGame->Weapon[SlotNum] != nullptr) {
		int AmmoShow = (int)((56.0f*(WorkshopFighterGame->Weapon[SlotNum]->AmmoStart-WorkshopFighterGame->Weapon[SlotNum]->Ammo))/WorkshopFighterGame->Weapon[SlotNum]->AmmoStart);
		// если меняли боекомплект и сделали его меньше, чтобы не вылазила линия боекомплекта...
		if (AmmoShow < 0)
			AmmoShow = 0;

		SrcRect(0,AmmoShow,18,56);
		DstRect(Xpos+23,Ypos+40+AmmoShow,Xpos+18+23,Ypos+56+40);
		if (AmmoShow > 0)
			vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("menu/ammo.tga"), true, CurrentAlert3*MenuContentTransp);
		else
			vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("menu/ammo.tga"), true, MenuContentTransp);

		if (WorkshopFighterGame->Weapon[SlotNum]->Ammo == 0)
			WeaponAmmoOut = true;
	}

	// запускаем голос...
	if ((WeaponAmmoOut) &&
	    (vw_FindSoundByNum(VoiceAmmoOut) == nullptr)) // уже не играем, нужно запустить опять
		VoiceAmmoOut = Audio_PlayVoice(10, 1.0f);


	// кнопка Setup
	int MouseX, MouseY;
	vw_GetMousePos(&MouseX, &MouseY);

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
		vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName(vw_GetText("12_button_weaponry_in.tga")), true, MenuContentTransp);
		CurrentCursorStatus = 1;
		if (vw_GetWindowLBMouse(true) || (InFocusByKeyboard && (vw_GetKeyStatus(SDLK_KP_ENTER) || vw_GetKeyStatus(SDLK_RETURN)))) {
			Audio_PlaySound2D(2,1.0f);
			WeaponSetupSlot = SlotNum;
			if (InFocusByKeyboard) {
				vw_SetKeyStatus(SDLK_KP_ENTER, false);
				vw_SetKeyStatus(SDLK_RETURN, false);
			}
		}
	} else
		vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName(vw_GetText("12_button_weaponry_out.tga")), true, MenuContentTransp);



	// обработка перетягивания
	DstRect(X,Y,X+128,Y+64);
	if (vw_OnRect(DstRect) && !isDialogBoxDrawing()) {

		int Money = Setup.Profile[CurrentProfile].Money;
		if (WorkshopFighterGame->Weapon[SlotNum] != nullptr)
			Money += GetWeaponCost(WorkshopFighterGame->Weapon[SlotNum]->ObjectCreationType,
					       WorkshopFighterGame->Weapon[SlotNum]->Ammo,
					       WorkshopFighterGame->Weapon[SlotNum]->AmmoStart);

		// если отпустили тут
		if (!vw_GetWindowLBMouse(false) && DragWeapon) {
			// есть уровень слота соотв. уровню оружия
			if ((WorkshopFighterGame->WeaponType[SlotNum] >= DragWeaponLevel) &&
			    // если стоимость меньше чем есть денег + стоимость оружия
			    (Money >= GetWeaponCost(DragWeaponNum, DragWeaponAmmo, DragWeaponAmmoStart))) {
				// звук устанавливаемого оружия
				Audio_PlaySound2D(10,1.0f);

				// если тут было оружие - сначало продаем его
				if (WorkshopFighterGame->Weapon[SlotNum] != nullptr) {
					Setup.Profile[CurrentProfile].Money += GetWeaponCost(WorkshopFighterGame->Weapon[SlotNum]->ObjectCreationType,
									       WorkshopFighterGame->Weapon[SlotNum]->Ammo,
									       WorkshopFighterGame->Weapon[SlotNum]->AmmoStart);
					delete WorkshopFighterGame->Weapon[SlotNum];
					WorkshopFighterGame->Weapon[SlotNum] = nullptr;
					Setup.Profile[CurrentProfile].Weapon[SlotNum] = 0;
					Setup.Profile[CurrentProfile].WeaponAmmo[SlotNum] = 0;
				}

				// покупаем оружие
				Setup.Profile[CurrentProfile].Money -= GetWeaponCost(DragWeaponNum, DragWeaponAmmo, DragWeaponAmmoStart);

				// все проверки сделали до этого, можем просто вызвать функцию, там 100% будет оружие
				SetEarthSpaceFighterWeapon(WorkshopFighterGame, SlotNum+1, DragWeaponNum);
				// убираем источник света
				if ((WorkshopFighterGame->Weapon[SlotNum]->Fire != nullptr) &&
				    (WorkshopFighterGame->Weapon[SlotNum]->Fire->Light != nullptr)) {
					vw_ReleaseLight(WorkshopFighterGame->Weapon[SlotNum]->Fire->Light);
					WorkshopFighterGame->Weapon[SlotNum]->Fire->Light = nullptr;
				}

				Setup.Profile[CurrentProfile].Weapon[SlotNum] = DragWeaponNum;
				Setup.Profile[CurrentProfile].WeaponAmmo[SlotNum] = DragWeaponAmmo;
				Setup.Profile[CurrentProfile].WeaponControl[SlotNum] = DragWeaponControl;
				Setup.Profile[CurrentProfile].WeaponAltControl[SlotNum] = DragWeaponAltControl;
				Setup.Profile[CurrentProfile].WeaponAltControlData[SlotNum] = DragWeaponAltControlData;
				WorkshopFighterGame->Weapon[SlotNum]->Ammo = DragWeaponAmmo;
				WorkshopFighterGame->Weapon[SlotNum]->AmmoStart = DragWeaponAmmoStart;

				// если не ракетная установка
				if (DragWeaponNum < 16) {
					WorkshopFighterGame->WeaponYAngle[SlotNum] = -Setup.Profile[CurrentProfile].WeaponSlotYAngle[SlotNum];
					sVECTOR3D NeedAngle = WorkshopFighterGame->Rotation;
					NeedAngle.y += WorkshopFighterGame->WeaponYAngle[SlotNum];
					WorkshopFighterGame->Weapon[SlotNum]->SetRotation(NeedAngle);
				} else
					Setup.Profile[CurrentProfile].WeaponSlotYAngle[SlotNum] = 0.0f;

				// чтобы оружие заняло свое место...
				WorkshopFighterGame->Update(vw_GetTime());

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
				Audio_PlaySound2D(8, 1.0f);

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
		if ((WorkshopFighterGame->Weapon[SlotNum] != nullptr) &&
		    vw_GetWindowLBMouse(false) &&
		    !DragWeapon) {
			// звук снятия оружия из слота
			Audio_PlaySound2D(9, 1.0f);

			DragWeapon = true;
			DragWeaponNum = WorkshopFighterGame->Weapon[SlotNum]->ObjectCreationType;
			DragWeaponLevel = WorkshopFighterGame->Weapon[SlotNum]->WeaponLevel;
			DragWeaponAmmo = WorkshopFighterGame->Weapon[SlotNum]->Ammo;
			DragWeaponAmmoStart = WorkshopFighterGame->Weapon[SlotNum]->AmmoStart;
			DragWeaponControl = Setup.Profile[CurrentProfile].WeaponControl[SlotNum];
			DragWeaponAltControl = Setup.Profile[CurrentProfile].WeaponAltControl[SlotNum];
			DragWeaponAltControlData = Setup.Profile[CurrentProfile].WeaponAltControlData[SlotNum];

			Setup.Profile[CurrentProfile].Money += GetWeaponCost(WorkshopFighterGame->Weapon[SlotNum]->ObjectCreationType,
							       WorkshopFighterGame->Weapon[SlotNum]->Ammo,
							       WorkshopFighterGame->Weapon[SlotNum]->AmmoStart);
			delete WorkshopFighterGame->Weapon[SlotNum];
			WorkshopFighterGame->Weapon[SlotNum] = nullptr;
			Setup.Profile[CurrentProfile].Weapon[SlotNum] = 0;
			Setup.Profile[CurrentProfile].WeaponAmmo[SlotNum] = 0;
		}



		// если не тянем и в слоте что-то есть, показываем, что можем тянуть
		if (!DragWeapon && (WorkshopFighterGame->Weapon[SlotNum] != nullptr))
			CurrentCursorStatus = 1;

	}




	// рисуем можем ставить в слот или нет
	bool CanOn = false;
	// проверка по уровню слота
	if (DragWeapon && (WorkshopFighterGame->WeaponType[SlotNum] >= DragWeaponLevel)) CanOn = true;
	if (!DragWeapon && (WorkshopFighterGame->WeaponType[SlotNum] >= WorkshopNewWeapon->WeaponLevel)) CanOn = true;

	SrcRect(0,0,128,64);
	DstRect(X,Y,X+128,Y+64);

	if (CanOn) {
		if (!WeaponAmmoOut) {
			bool NeedAlert = false;
			// если есть хоть 1 слот меньшего уровня
			for (int i=0; i<WorkshopFighterGame->WeaponQuantity; i++) {
				// если что-то тянем, смотрим по нему...
				if (DragWeapon) {
					if (WorkshopFighterGame->WeaponType[i] < DragWeaponLevel) NeedAlert = true;
				} else {
					if (WorkshopFighterGame->WeaponType[i] < WorkshopNewWeapon->WeaponLevel) NeedAlert = true;
				}
			}

			if (NeedAlert)
				vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("menu/weapon_on_icon.tga"), true, CurrentAlert3*MenuContentTransp, 0, RI_UL_CORNER, 0.0f, 1.0f, 0.0f);
			else
				vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("menu/weapon_on_icon.tga"), true, MenuContentTransp, 0, RI_UL_CORNER, 0.0f, 1.0f, 0.0f);
		} else
			vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("menu/weapon_on_icon.tga"), true, CurrentAlert3*MenuContentTransp, 0, RI_UL_CORNER, 1.0f, 0.0f, 0.0f);
	} else
		vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("menu/weapon_on_icon.tga"), true, MenuContentTransp, 0, RI_UL_CORNER, 1.0f, 0.5f, 0.0f);





	// прорисовка

	if (WorkshopFighterGame->Weapon[SlotNum] == nullptr) {
		// пустой слот, рисуем его

		DstRect(X,Y,X+128,Y+64);

		int Size = vw_FontSize(vw_GetText("3_EMPTY"));
		float WScale = 0;
		if (Size > 88) {
			Size = 88;
			WScale = -88;
		}
		vw_DrawFont(DstRect.left+(DstRect.right-DstRect.left-Size)/2, DstRect.bottom-53, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, 0.7*MenuContentTransp, vw_GetText("3_EMPTY"));

		Size = vw_FontSize("%s %i",vw_GetText("3_Level"),WorkshopFighterGame->WeaponType[SlotNum]);
		WScale = 0;
		if (Size > 88) {
			Size = 88;
			WScale = -88;
		}
		vw_DrawFont(DstRect.left+(DstRect.right-DstRect.left-Size)/2, DstRect.bottom-32, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, 0.7*MenuContentTransp, "%s %i",vw_GetText("3_Level"),WorkshopFighterGame->WeaponType[SlotNum]);


		return;
	}

	SrcRect(0,0,128,64);
	DstRect(X,Y,X+128,Y+64);
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName(GetWeaponIconName(WorkshopFighterGame->Weapon[SlotNum]->ObjectCreationType)), true, MenuContentTransp);




}






void ShipSlotSetupWeapon(int Slot)
{
	sRECT SrcRect, DstRect;

	// проверяем, нужно ли вытягивать что-то или нет...
	CheckMouseKeybJState();

	SrcRect(0,0,404,570);
	int Xpos = Setup.iAspectRatioWidth/2+55;
	int Ypos = 50-10;
	DstRect(Xpos,Ypos,Xpos+404,Ypos+570);
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("menu/workshop_panel5.tga"), true, MenuContentTransp);



	if (WorkshopFighterGame->Weapon[Slot] == nullptr) {
		Xpos += 74;
		Ypos += 128;
		// пустой слот, рисуем его
		SrcRect(0,0,256,256);
		DstRect(Xpos,Ypos,Xpos+256,Ypos+256);
		vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("menu/weapon_empty_icon.tga"), true, MenuContentTransp);

		int Size = vw_FontSize(vw_GetText("3_Empty_Weapon_Slot"));
		float WScale = 0;
		if (Size > 228) {
			Size = 228;
			WScale = -228;
		}
		vw_DrawFont(DstRect.left+(DstRect.right-DstRect.left-Size)/2, DstRect.bottom-40, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("3_Empty_Weapon_Slot"));
	} else {
		Xpos += 74+64;
		Ypos += 33;
		// выводим боекомплект   текущий/полный
		Xpos = Setup.iAspectRatioWidth/2+55 + 50;
		vw_DrawFont(Xpos, Ypos, -170, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("7_Weapon_Ammo:"));
		Xpos = (Setup.iAspectRatioWidth/2+512)-55 - 50 - vw_FontSize("%i/%i", WorkshopFighterGame->Weapon[Slot]->Ammo, WorkshopFighterGame->Weapon[Slot]->AmmoStart);


		// если все нормально - белым... иначе подмаргиваем
		if (WorkshopFighterGame->Weapon[Slot]->Ammo == WorkshopFighterGame->Weapon[Slot]->AmmoStart)
			vw_DrawFont(Xpos, Ypos, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i/%i", WorkshopFighterGame->Weapon[Slot]->Ammo, WorkshopFighterGame->Weapon[Slot]->AmmoStart);
		else
			vw_DrawFont(Xpos, Ypos, 0, 0, 1.0f, 1.0f,0.5f,0.0f, CurrentAlert3*MenuContentTransp, "%i/%i", WorkshopFighterGame->Weapon[Slot]->Ammo, WorkshopFighterGame->Weapon[Slot]->AmmoStart);


		// стоимость перезарядки
		Xpos = Setup.iAspectRatioWidth/2+55 + 50;
		Ypos += 30;
		vw_DrawFont(Xpos, Ypos, -230, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("7_Weapon_Reload_Cost:"));
		// находим стоимость перезарядки
		int ReloadCost = GetWeaponReloadCost(WorkshopFighterGame->Weapon[Slot]->ObjectCreationType,
						     WorkshopFighterGame->Weapon[Slot]->Ammo,
						     WorkshopFighterGame->Weapon[Slot]->AmmoStart);
		Xpos = (Setup.iAspectRatioWidth/2+512)-55 - 50 - vw_FontSize("%i", ReloadCost);
		if (ReloadCost == 0)
			vw_DrawFont(Xpos, Ypos, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i", ReloadCost);
		else
			vw_DrawFont(Xpos, Ypos, 0, 0, 1.0f, 1.0f,0.5f,0.0f, CurrentAlert3*MenuContentTransp, "%i", ReloadCost);


		// кнопка перезарядить оружие
		Ypos += 40;
		if (DrawButton200_2(Setup.iAspectRatioWidth/2+155, Ypos, vw_GetText("1_Reload"), MenuContentTransp, ReloadCost==0 || Setup.Profile[CurrentProfile].Money<ReloadCost)) {
			WorkshopFighterGame->Weapon[Slot]->Ammo = WorkshopFighterGame->Weapon[Slot]->AmmoStart;
			Setup.Profile[CurrentProfile].WeaponAmmo[Slot] = WorkshopFighterGame->Weapon[Slot]->Ammo;
			Setup.Profile[CurrentProfile].Money -= ReloadCost;
		}



		// настройка управления для этого оружия

		bool Status1 = false;
		bool Status2 = false;

		Xpos = Setup.iAspectRatioWidth/2+55+34 + 16;
		Ypos += 60;
		vw_DrawFont(Xpos, Ypos, -300, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("3_Weapon_Fire_Control:"));
		// вкл-выкл первичного управления
		if (Setup.Profile[CurrentProfile].WeaponControl[Slot] == 1 ||
		    Setup.Profile[CurrentProfile].WeaponControl[Slot] ==3) Status1 = true;
		Xpos = Setup.iAspectRatioWidth/2+55+54 + 16;
		Ypos += 30;
		DrawCheckBox(Xpos,Ypos, &Status1, vw_GetText("3_Primary_Attack"), MenuContentTransp);
		// вкл-выкл вторичного управления
		if (Setup.Profile[CurrentProfile].WeaponControl[Slot] == 2 ||
		    Setup.Profile[CurrentProfile].WeaponControl[Slot] ==3) Status2 = true;
		Xpos = Setup.iAspectRatioWidth/2+55+54 + 16;
		Ypos += 40;
		DrawCheckBox(Xpos,Ypos, &Status2, vw_GetText("3_Secondary_Attack"), MenuContentTransp);
		// получаем данны обратно
		Setup.Profile[CurrentProfile].WeaponControl[Slot] = 0;
		if (Status1) Setup.Profile[CurrentProfile].WeaponControl[Slot] += 1;
		if (Status2) Setup.Profile[CurrentProfile].WeaponControl[Slot] += 2;

		// получение альтернативного управления
		Xpos = Setup.iAspectRatioWidth/2+55+34 + 16;
		Ypos += 40;
		vw_DrawFont(Xpos, Ypos, -300, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("3_Alternative_Fire_Control:"));

		Ypos += 40;

		const char *TextTmp = "?";
		// установка надписи на кнопке
		if (NeedCheck != 100) {
			if (NewWeaponControlType != 0) {
				Setup.Profile[CurrentProfile].WeaponAltControl[Slot] = NewWeaponControlType;
				Setup.Profile[CurrentProfile].WeaponAltControlData[Slot] = NewWeaponControlTypeData;
				NewWeaponControlType = 0;
				NewWeaponControlTypeData = 0;
			}
			if (Setup.Profile[CurrentProfile].WeaponAltControl[Slot] != 0) {
				if (Setup.Profile[CurrentProfile].WeaponAltControl[Slot] == 1)
					TextTmp = SDL_GetKeyName(Setup.Profile[CurrentProfile].WeaponAltControlData[Slot]);
				if (Setup.Profile[CurrentProfile].WeaponAltControl[Slot] == 2)
					TextTmp = MouseCodeName(Setup.Profile[CurrentProfile].WeaponAltControlData[Slot]);
				if (Setup.Profile[CurrentProfile].WeaponAltControl[Slot] == 3)
					TextTmp = JoystickCodeName(Setup.Profile[CurrentProfile].WeaponAltControlData[Slot]);

			} else TextTmp = (char*)vw_GetText("1_Unimplemented");
		}

		// собственно сама кнопка
		float Transp = 1.0f;
		bool Off = false;
		if (NeedCheck == 100) {
			Transp = But[1];
			Off = true;
		}
		if (DrawButton200_2(Setup.iAspectRatioWidth/2+155, Ypos, TextTmp, Transp*MenuContentTransp, Off)) {
			NeedCheck = 100;
			for (int i=0; i<8; i++) SDL_MouseCurrentStatus[i] = false;
			NewWeaponControlType = 0;
			NewWeaponControlTypeData = 0;
		}



		if (Setup.Profile[CurrentProfile].Weapon[Slot] < 16) {

			// выводим угол поворота ствола
			Xpos = Setup.iAspectRatioWidth/2+55+34 + 16;
			Ypos += 60;
			vw_DrawFont(Xpos, Ypos, -300, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("3_Weapon_Angle:_%2.1f"), Setup.Profile[CurrentProfile].WeaponSlotYAngle[Slot]);
			Ypos += 40;

			float Min = 0.0f;
			float Max = 0.0f;
			GetShipWeaponSlotAngle(Setup.Profile[CurrentProfile].Ship, Slot, &Min, &Max);


			if (Setup.Profile[CurrentProfile].TargetingSystem <= 2) {
				vw_DrawFont(Xpos, Ypos-15, 300, 0, 1.0f, 1.0f,0.5f,0.0f, 1.0f, vw_GetText("9_17Line1"));
				vw_DrawFont(Xpos, Ypos+5, 300, 0, 1.0f, 1.0f,0.5f,0.0f, 1.0f, vw_GetText("9_17Line2"));
				vw_DrawFont(Xpos, Ypos+25, 300, 0, 1.0f, 1.0f,0.5f,0.0f, 1.0f, vw_GetText("9_17Line3"));
			} else {
				if (DrawButton128_2(Setup.iAspectRatioWidth/2+118, Ypos, vw_GetText("1_Left"), MenuContentTransp, Setup.Profile[CurrentProfile].WeaponSlotYAngle[Slot] <= Min)) {
					Setup.Profile[CurrentProfile].WeaponSlotYAngle[Slot] -= 5.0f;
					WorkshopFighterGame->WeaponYAngle[Slot] = -Setup.Profile[CurrentProfile].WeaponSlotYAngle[Slot];

					WorkshopFighterGame->Weapon[Slot]->SetRotation(WorkshopFighterGame->Weapon[Slot]->Rotation^(-1));
					sVECTOR3D NeedAngle = WorkshopFighterGame->Rotation;
					NeedAngle.y += WorkshopFighterGame->WeaponYAngle[Slot];
					WorkshopFighterGame->Weapon[Slot]->SetRotation(NeedAngle);
				}
				if (DrawButton128_2(Setup.iAspectRatioWidth/2+266, Ypos, vw_GetText("1_Right"), MenuContentTransp, Setup.Profile[CurrentProfile].WeaponSlotYAngle[Slot] >= Max)) {
					Setup.Profile[CurrentProfile].WeaponSlotYAngle[Slot] += 5.0f;
					WorkshopFighterGame->WeaponYAngle[Slot] = -Setup.Profile[CurrentProfile].WeaponSlotYAngle[Slot];

					WorkshopFighterGame->Weapon[Slot]->SetRotation(WorkshopFighterGame->Weapon[Slot]->Rotation^(-1));
					sVECTOR3D NeedAngle = WorkshopFighterGame->Rotation;
					NeedAngle.y += WorkshopFighterGame->WeaponYAngle[Slot];
					WorkshopFighterGame->Weapon[Slot]->SetRotation(NeedAngle);
				}
			}
		}
	}





	// обработка перетягивания
	int SlotNum = Slot;
	Xpos = Setup.iAspectRatioWidth/2+55;
	Ypos = 50-10;
	DstRect(Xpos+10,Ypos+10,Xpos+404-10,Ypos+570-10);
	if (vw_OnRect(DstRect) && !isDialogBoxDrawing()) {

		int Money = Setup.Profile[CurrentProfile].Money;
		if (WorkshopFighterGame->Weapon[SlotNum] != nullptr)
			Money += GetWeaponCost(WorkshopFighterGame->Weapon[SlotNum]->ObjectCreationType,
					       WorkshopFighterGame->Weapon[SlotNum]->Ammo,
					       WorkshopFighterGame->Weapon[SlotNum]->AmmoStart);

		// если отпустили тут
		if (!vw_GetWindowLBMouse(false) && DragWeapon) {
			// есть уровень слота соотв. уровню оружия
			if ((WorkshopFighterGame->WeaponType[SlotNum] >= DragWeaponLevel) &&
			    // если стоимость меньше чем есть денег + стоимость оружия
			    (Money >= GetWeaponCost(DragWeaponNum, DragWeaponAmmo, DragWeaponAmmoStart))) {
				// звук устанавливаемого оружия
				Audio_PlaySound2D(10,1.0f);

				// если тут было оружие - сначало продаем его
				if (WorkshopFighterGame->Weapon[SlotNum] != nullptr) {
					Setup.Profile[CurrentProfile].Money += GetWeaponCost(WorkshopFighterGame->Weapon[SlotNum]->ObjectCreationType,
									       WorkshopFighterGame->Weapon[SlotNum]->Ammo,
									       WorkshopFighterGame->Weapon[SlotNum]->AmmoStart);
					delete WorkshopFighterGame->Weapon[SlotNum];
					WorkshopFighterGame->Weapon[SlotNum] = nullptr;
					Setup.Profile[CurrentProfile].Weapon[SlotNum] = 0;
					Setup.Profile[CurrentProfile].WeaponAmmo[SlotNum] = 0;
				}

				// покупаем оружие
				Setup.Profile[CurrentProfile].Money -= GetWeaponCost(DragWeaponNum, DragWeaponAmmo, DragWeaponAmmoStart);

				// все проверки сделали до этого, можем просто вызвать функцию, там 100% будет оружие
				SetEarthSpaceFighterWeapon(WorkshopFighterGame, SlotNum+1, DragWeaponNum);
				// убираем источник света
				if ((WorkshopFighterGame->Weapon[SlotNum]->Fire != nullptr) &&
				    (WorkshopFighterGame->Weapon[SlotNum]->Fire->Light != nullptr)) {
					vw_ReleaseLight(WorkshopFighterGame->Weapon[SlotNum]->Fire->Light);
					WorkshopFighterGame->Weapon[SlotNum]->Fire->Light = nullptr;
				}

				Setup.Profile[CurrentProfile].Weapon[SlotNum] = DragWeaponNum;
				Setup.Profile[CurrentProfile].WeaponAmmo[SlotNum] = DragWeaponAmmo;
				Setup.Profile[CurrentProfile].WeaponControl[SlotNum] = DragWeaponControl;
				Setup.Profile[CurrentProfile].WeaponAltControl[SlotNum] = DragWeaponAltControl;
				Setup.Profile[CurrentProfile].WeaponAltControlData[SlotNum] = DragWeaponAltControlData;
				WorkshopFighterGame->Weapon[SlotNum]->Ammo = DragWeaponAmmo;
				WorkshopFighterGame->Weapon[SlotNum]->AmmoStart = DragWeaponAmmoStart;

				// если не ракетная установка
				if (DragWeaponNum < 16) {
					WorkshopFighterGame->WeaponYAngle[SlotNum] = -Setup.Profile[CurrentProfile].WeaponSlotYAngle[SlotNum];
					sVECTOR3D NeedAngle = WorkshopFighterGame->Rotation;
					NeedAngle.y += WorkshopFighterGame->WeaponYAngle[SlotNum];
					WorkshopFighterGame->Weapon[SlotNum]->SetRotation(NeedAngle);
				} else
					Setup.Profile[CurrentProfile].WeaponSlotYAngle[SlotNum] = 0.0f;

				// чтобы оружие заняло свое место...
				WorkshopFighterGame->Update(vw_GetTime());

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
				Audio_PlaySound2D(8, 1.0f);

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
	if (DrawButton200_2(Setup.iAspectRatioWidth/2+155, 533, vw_GetText("1_Close"), MenuContentTransp, false)) {
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

	switch (Setup.Profile[CurrentProfile].Ship) {
	case 1:
		ShipSlotWeapon(0, Setup.iAspectRatioWidth/2+50, 530);
		ShipSlotWeapon(1, (Setup.iAspectRatioWidth/2+512)-128-50, 530);
		ShipSlotWeapon(2, Setup.iAspectRatioWidth/2+50, 130);
		ShipSlotWeapon(3, (Setup.iAspectRatioWidth/2+512)-128-50, 130);
		ShipSlotWeapon(4, (Setup.iAspectRatioWidth/2+256)-64, 330);
		break;
	case 2:
		ShipSlotWeapon(0, Setup.iAspectRatioWidth/2+50, 530);
		ShipSlotWeapon(1, (Setup.iAspectRatioWidth/2+512)-128-50, 530);
		ShipSlotWeapon(2, Setup.iAspectRatioWidth/2+50, 130);
		ShipSlotWeapon(3, (Setup.iAspectRatioWidth/2+512)-128-50, 130);
		break;
	case 3:
		ShipSlotWeapon(0, (Setup.iAspectRatioWidth/2+256)-64, 130);
		ShipSlotWeapon(1, Setup.iAspectRatioWidth/2+50, 530);
		ShipSlotWeapon(2, (Setup.iAspectRatioWidth/2+512)-128-50, 530);
		break;
	case 4:
		ShipSlotWeapon(0, Setup.iAspectRatioWidth/2+50, 530);
		ShipSlotWeapon(1, (Setup.iAspectRatioWidth/2+512)-128-50, 530);
		ShipSlotWeapon(2, Setup.iAspectRatioWidth/2+50, 330);
		ShipSlotWeapon(3, (Setup.iAspectRatioWidth/2+512)-128-50, 330);
		ShipSlotWeapon(4, (Setup.iAspectRatioWidth/2+256)-64, 130);
		break;
	case 5:
		ShipSlotWeapon(0, Setup.iAspectRatioWidth/2+50, 330);
		ShipSlotWeapon(1, (Setup.iAspectRatioWidth/2+512)-128-50, 330);
		break;
	case 6:
		ShipSlotWeapon(0, Setup.iAspectRatioWidth/2+50, 530);
		ShipSlotWeapon(1, (Setup.iAspectRatioWidth/2+512)-128-50, 530);
		ShipSlotWeapon(2, Setup.iAspectRatioWidth/2+50, 330);
		ShipSlotWeapon(3, (Setup.iAspectRatioWidth/2+512)-128-50, 330);
		ShipSlotWeapon(4, (Setup.iAspectRatioWidth/2+256)-64, 130);
		break;
	case 7:
		ShipSlotWeapon(0, (Setup.iAspectRatioWidth/2+256)-64, 130);
		ShipSlotWeapon(1, Setup.iAspectRatioWidth/2+50, 330);
		ShipSlotWeapon(2, (Setup.iAspectRatioWidth/2+512)-128-50, 330);
		ShipSlotWeapon(3, Setup.iAspectRatioWidth/2+50, 530);
		ShipSlotWeapon(4, (Setup.iAspectRatioWidth/2+512)-128-50, 530);
		break;
	case 8:
		ShipSlotWeapon(0, Setup.iAspectRatioWidth/2+50, 330);
		ShipSlotWeapon(1, (Setup.iAspectRatioWidth/2+512)-128-50, 330);
		break;
	case 9:
		ShipSlotWeapon(0, Setup.iAspectRatioWidth/2+50, 330);
		ShipSlotWeapon(1, (Setup.iAspectRatioWidth/2+512)-128-50, 330);
		break;
	case 10:
		ShipSlotWeapon(0, Setup.iAspectRatioWidth/2+50, 530);
		ShipSlotWeapon(1, (Setup.iAspectRatioWidth/2+512)-128-50, 530);
		ShipSlotWeapon(2, Setup.iAspectRatioWidth/2+50, 330);
		ShipSlotWeapon(3, (Setup.iAspectRatioWidth/2+512)-128-50, 330);
		ShipSlotWeapon(4, (Setup.iAspectRatioWidth/2+256)-64, 130);
		break;
	case 11:
		ShipSlotWeapon(0, Setup.iAspectRatioWidth/2+50, 530);
		ShipSlotWeapon(1, (Setup.iAspectRatioWidth/2+512)-128-50, 530);
		ShipSlotWeapon(2, (Setup.iAspectRatioWidth/2+256)-64, 130);
		break;
	case 12:
		ShipSlotWeapon(0, Setup.iAspectRatioWidth/2+50, 530);
		ShipSlotWeapon(1, (Setup.iAspectRatioWidth/2+512)-128-50, 530);
		ShipSlotWeapon(2, (Setup.iAspectRatioWidth/2+256)-64, 130);
		break;
	case 13:
		ShipSlotWeapon(0, (Setup.iAspectRatioWidth/2+256)-64, 530);
		ShipSlotWeapon(1, Setup.iAspectRatioWidth/2+50, 330);
		ShipSlotWeapon(2, (Setup.iAspectRatioWidth/2+512)-128-50, 330);
		ShipSlotWeapon(3, Setup.iAspectRatioWidth/2+50, 130);
		ShipSlotWeapon(4, (Setup.iAspectRatioWidth/2+512)-128-50, 130);
		break;
	case 14:
		ShipSlotWeapon(0, Setup.iAspectRatioWidth/2+50, 130);
		ShipSlotWeapon(1, (Setup.iAspectRatioWidth/2+512)-128-50, 130);
		ShipSlotWeapon(2, Setup.iAspectRatioWidth/2+50, 530);
		ShipSlotWeapon(3, (Setup.iAspectRatioWidth/2+512)-128-50, 530);
		ShipSlotWeapon(4, (Setup.iAspectRatioWidth/2+256)-64, 330);
		break;
	case 15:
		ShipSlotWeapon(0, (Setup.iAspectRatioWidth/2+256)-64, 130);
		ShipSlotWeapon(1, Setup.iAspectRatioWidth/2+50, 530);
		ShipSlotWeapon(2, (Setup.iAspectRatioWidth/2+512)-128-50, 530);
		break;
	case 16:
		ShipSlotWeapon(0, (Setup.iAspectRatioWidth/2+256)-64, 130);
		ShipSlotWeapon(1, Setup.iAspectRatioWidth/2+50, 530);
		ShipSlotWeapon(2, (Setup.iAspectRatioWidth/2+512)-128-50, 530);
		break;
	case 17:
		ShipSlotWeapon(0, Setup.iAspectRatioWidth/2+50, 330);
		ShipSlotWeapon(1, (Setup.iAspectRatioWidth/2+512)-128-50, 330);
		ShipSlotWeapon(2, Setup.iAspectRatioWidth/2+50, 530);
		ShipSlotWeapon(3, (Setup.iAspectRatioWidth/2+512)-128-50, 530);
		ShipSlotWeapon(4, (Setup.iAspectRatioWidth/2+256)-64, 130);
		break;
	case 18:
		ShipSlotWeapon(0, Setup.iAspectRatioWidth/2+50, 130);
		ShipSlotWeapon(1, (Setup.iAspectRatioWidth/2+512)-128-50, 130);
		ShipSlotWeapon(2, Setup.iAspectRatioWidth/2+50, 530);
		ShipSlotWeapon(3, (Setup.iAspectRatioWidth/2+512)-128-50, 530);
		break;
	case 19:
		ShipSlotWeapon(0, (Setup.iAspectRatioWidth/2+256)-64, 130);
		ShipSlotWeapon(1, Setup.iAspectRatioWidth/2+50, 530);
		ShipSlotWeapon(2, (Setup.iAspectRatioWidth/2+512)-128-50, 530);
		break;
	case 20:
		ShipSlotWeapon(0, Setup.iAspectRatioWidth/2+50, 130);
		ShipSlotWeapon(1, (Setup.iAspectRatioWidth/2+512)-128-50, 130);
		ShipSlotWeapon(2, Setup.iAspectRatioWidth/2+50, 530);
		ShipSlotWeapon(3, (Setup.iAspectRatioWidth/2+512)-128-50, 530);
		break;
	case 21:
		ShipSlotWeapon(0, Setup.iAspectRatioWidth/2+50, 530);
		ShipSlotWeapon(1, (Setup.iAspectRatioWidth/2+512)-128-50, 530);
		ShipSlotWeapon(2, (Setup.iAspectRatioWidth/2+256)-64, 130);
		break;
	case 22:
		ShipSlotWeapon(0, (Setup.iAspectRatioWidth/2+256)-64, 330);
		break;
	}

}



int GetShipWeaponsMaxSlotLevel()
{
	int max = 1;

	for (int i=0; i<WorkshopFighterGame->WeaponQuantity; i++)
		if (WorkshopFighterGame->WeaponType[i] > max)
			max = WorkshopFighterGame->WeaponType[i];

	return max;
}




// вызываем прямо перед прорисовкой курсора
void DrawDragingWeaponIcon(int X, int Y)
{
	// если не тянем - соотв. и не рисуем
	if (!DragWeapon) return;

	// в х и у - положение точки курсора
	sRECT SrcRect(0, 0, 128, 64);
	sRECT DstRect(X - 64, Y - 32, X + 64, Y + 32);
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName(GetWeaponIconName(DragWeaponNum)), true, MenuContentTransp);
}




















//------------------------------------------------------------------------------------
// покупка-установка и ремонт вооружения корабля
//------------------------------------------------------------------------------------
void Workshop_Weaponry()
{
	sRECT SrcRect, DstRect;



	// если нажали - установка и тянем
	DstRect(Setup.iAspectRatioWidth/2-416,100+32,Setup.iAspectRatioWidth/2-96,450-32);
	if (vw_OnRect(DstRect) && !isDialogBoxDrawing())
		if (!DragWeapon) {
			CurrentCursorStatus = 1;

			if (vw_GetWindowLBMouse(false)) {
				// звук взяли оружие
				Audio_PlaySound2D(9,1.0f);

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
	DstRect(Setup.iAspectRatioWidth/2-480,100-32,Setup.iAspectRatioWidth/2-32,450+32);
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("menu/back_spot2.tga"), true, 0.45f*MenuContentTransp);
	DstRect(Setup.iAspectRatioWidth/2,0,Setup.iAspectRatioWidth/2+512,622);
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("menu/back_spot.tga"), true, 0.35f*MenuContentTransp);


	vw_End2DMode();
	WorkshopDrawShip(WorkshopFighterGame, 4);
	WorkshopDrawWeapon(WorkshopNewWeapon);
	vw_Start2DMode(-1,1);


	// вывод названия
	vw_DrawFont(Setup.iAspectRatioWidth/2-438, 50+6, 0, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText(GetWeaponName(CurrentWorkshopNewWeapon)));
	if (DrawButton128_2(Setup.iAspectRatioWidth/2-197,50, vw_GetText("1_Info"), MenuContentTransp, false)) {
		SetCurrentDialogBox(eDialogBox::ShowWeaponsInfo);
		DialogWeapon = WorkshopNewWeapon;
	}

	// поставить GameNPCWeaponPenalty=1!!!
	vw_DrawFont(Setup.iAspectRatioWidth/2-438, 110, -170, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%s:", vw_GetText("7_Weapon_Type"));
	vw_DrawFont(Setup.iAspectRatioWidth/2-438+175, 110, -184, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText(GetWeaponGroupTitle(CurrentWorkshopNewWeapon)));


	int k2 = 0;
	if (GetWeaponHullDamage(WorkshopNewWeapon->ObjectCreationType) > 0.0f) {
		vw_DrawFont(Setup.iAspectRatioWidth/2-438, 130, -170, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Damage,_Hull:"));
		if ((WorkshopNewWeapon->ObjectCreationType == 11) |
		    (WorkshopNewWeapon->ObjectCreationType == 12) |
		    (WorkshopNewWeapon->ObjectCreationType == 14))
			vw_DrawFont(Setup.iAspectRatioWidth/2-438+175, 130, -184, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i %s", GetWeaponHullDamage(WorkshopNewWeapon->ObjectCreationType), vw_GetText("4_units/sec"));
		else
			vw_DrawFont(Setup.iAspectRatioWidth/2-438+175, 130, -184, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i %s", GetWeaponHullDamage(WorkshopNewWeapon->ObjectCreationType), vw_GetText("4_units/shot"));

		k2=20;
	}
	if (GetWeaponSystemsDamage(WorkshopNewWeapon->ObjectCreationType) > 0.0f) {
		vw_DrawFont(Setup.iAspectRatioWidth/2-438, 130+k2, -170, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("4_Damage,_Systems:"));
		if ((WorkshopNewWeapon->ObjectCreationType == 11) |
		    (WorkshopNewWeapon->ObjectCreationType == 12) |
		    (WorkshopNewWeapon->ObjectCreationType == 14))
			vw_DrawFont(Setup.iAspectRatioWidth/2-438+175, 130+k2, -184, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i %s", GetWeaponSystemsDamage(WorkshopNewWeapon->ObjectCreationType), vw_GetText("4_units/sec"));
		else
			vw_DrawFont(Setup.iAspectRatioWidth/2-438+175, 130+k2, -184, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i %s", GetWeaponSystemsDamage(WorkshopNewWeapon->ObjectCreationType), vw_GetText("4_units/shot"));

	}



	// вывод уровня оружия
	if (WorkshopNewWeapon->WeaponLevel <= GetShipWeaponsMaxSlotLevel())
		vw_DrawFont(Setup.iAspectRatioWidth/2-438, 400, 0, 0, 1.0f, 0.0f,1.0f,0.0f, MenuContentTransp, "%s: %i", vw_GetText("7_Weapon_Level"), WorkshopNewWeapon->WeaponLevel);
	else
		vw_DrawFont(Setup.iAspectRatioWidth/2-438, 400, 0, 0, 1.0f, 1.0f,0.5f,0.0f, CurrentAlert3*MenuContentTransp, "%s: %i", vw_GetText("7_Weapon_Level"), WorkshopNewWeapon->WeaponLevel);

	// вывод стоимости
	if (Setup.Profile[CurrentProfile].Money >= GetWeaponBaseCost(CurrentWorkshopNewWeapon))// всегда ведь новое, считать боекомплект не нужно
		vw_DrawFont(Setup.iAspectRatioWidth/2-438, 420, 0, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, "%s: %i", vw_GetText("7_Weapon_Cost"), GetWeaponBaseCost(CurrentWorkshopNewWeapon));
	else
		vw_DrawFont(Setup.iAspectRatioWidth/2-438, 420, 0, 0, 1.0f, 1.0f,0.0f,0.0f, CurrentAlert3*MenuContentTransp, "%s: %i", vw_GetText("7_Weapon_Cost"), GetWeaponBaseCost(CurrentWorkshopNewWeapon));



	// рамки
	SrcRect(0,0,400,35 );
	DstRect(Setup.iAspectRatioWidth/2-457,100-11,Setup.iAspectRatioWidth/2-57,100+35-11);
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("menu/workshop_panel4.tga"), true, MenuContentTransp);

	SrcRect(0,0,400,173 );
	DstRect(Setup.iAspectRatioWidth/2-457,450-13,Setup.iAspectRatioWidth/2-57,450+173-13);
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("menu/workshop_panel1.tga"), true, MenuContentTransp);





	// проверяем колесо мышки
	DstRect(Setup.iAspectRatioWidth/2-457,100+35-11,Setup.iAspectRatioWidth/2-57,450-13);
	if (vw_OnRect(DstRect)) {
		if (vw_GetWheelStatus() != 0 && !isDialogBoxDrawing()) {
			CurrentWorkshopNewWeapon += vw_GetWheelStatus();

			if (CurrentWorkshopNewWeapon < 1) CurrentWorkshopNewWeapon = 19;
			if (CurrentWorkshopNewWeapon > 19) CurrentWorkshopNewWeapon = 1;
			WorkshopCreateNewWeapon();

			vw_ResetWheelStatus();
		}
	} else if (vw_GetWheelStatus() != 0) {
		vw_ResetWheelStatus();
	}


	if (DrawButton128_2(Setup.iAspectRatioWidth/2-395,482, vw_GetText("1_Prev"), MenuContentTransp, false)) {
		CurrentWorkshopNewWeapon --;
		if (CurrentWorkshopNewWeapon < 1) CurrentWorkshopNewWeapon = 19;
		WorkshopCreateNewWeapon();
	}
	if (DrawButton128_2(Setup.iAspectRatioWidth/2-247,482, vw_GetText("1_Next"), MenuContentTransp, false)) {
		CurrentWorkshopNewWeapon ++;
		if (CurrentWorkshopNewWeapon > 19) CurrentWorkshopNewWeapon = 1;
		WorkshopCreateNewWeapon();
	}


	if (DrawButton128_2(Setup.iAspectRatioWidth/2-395,533, vw_GetText(GetWeaponGroupTitle(PrevWeaponGroup())), MenuContentTransp, false)) {
		CurrentWorkshopNewWeapon = PrevWeaponGroup();
		WorkshopCreateNewWeapon();
	}
	if (DrawButton128_2(Setup.iAspectRatioWidth/2-247,533, vw_GetText(GetWeaponGroupTitle(NextWeaponGroup())), MenuContentTransp, false)) {
		CurrentWorkshopNewWeapon = NextWeaponGroup();
		WorkshopCreateNewWeapon();
	}

	vw_SetFontSize(24);
	vw_SetFontOffsetY(2);
	vw_DrawFont(Setup.iAspectRatioWidth/2-445, 600, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("7_Weapon_Stock"));
	vw_SetFontSize(Setup.FontSize);
	vw_SetFontOffsetY(8 - ceil(Setup.FontSize/2.0f) + 2);

	DrawShipWeaponsInSlots();


	// кнопка "перезарядить все" оружие
	if (WeaponSetupSlot == -1) {
		int ReloadCost = 0;
		// находим стоимость перезарядки
		for (int i=0; i<WorkshopFighterGame->WeaponQuantity; i++) {
			if (WorkshopFighterGame->Weapon[i] != nullptr)
				ReloadCost += GetWeaponReloadCost(WorkshopFighterGame->Weapon[i]->ObjectCreationType,
								  WorkshopFighterGame->Weapon[i]->Ammo,
								  WorkshopFighterGame->Weapon[i]->AmmoStart);
		}
		std::string ButtonName{std::string(vw_GetText("1_Reload_All")) + ": " + std::to_string(ReloadCost)};

		if (DrawButton200_2(Setup.iAspectRatioWidth/2+153, 50, ButtonName.c_str(), MenuContentTransp, ReloadCost==0 || Setup.Profile[CurrentProfile].Money<ReloadCost)) {
			for (int i = 0; i < WorkshopFighterGame->WeaponQuantity; i++) {
				if (WorkshopFighterGame->Weapon[i] != nullptr) {
					WorkshopFighterGame->Weapon[i]->Ammo = WorkshopFighterGame->Weapon[i]->AmmoStart;
					Setup.Profile[CurrentProfile].WeaponAmmo[i] = WorkshopFighterGame->Weapon[i]->Ammo;
				}
			}
			Setup.Profile[CurrentProfile].Money -= ReloadCost;
		}
	}

	vw_SetFontSize(24);
	vw_SetFontOffsetY(2);
	vw_DrawFont(Setup.iAspectRatioWidth/2+445-vw_FontSize(vw_GetText("7_Installed_Weapons")), 600, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("7_Installed_Weapons"));
	vw_SetFontSize(Setup.FontSize);
	vw_SetFontOffsetY(8 - ceil(Setup.FontSize/2.0f) + 2);

	// вывод информации
	vw_SetFontSize(20);
	vw_SetFontOffsetY(2);
	int SizeI = (Setup.iAspectRatioWidth-vw_FontSize("%s: %i", vw_GetText("3_Money"), Setup.Profile[CurrentProfile].Money))/2;
	if (Setup.Profile[CurrentProfile].Money >= GetWeaponBaseCost(CurrentWorkshopNewWeapon))
		vw_DrawFont(SizeI, 630, 0, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, "%s: %i", vw_GetText("3_Money"), Setup.Profile[CurrentProfile].Money);
	else
		vw_DrawFont(SizeI, 630, 0, 0, 1.0f, 1.0f,0.0f,0.0f, CurrentAlert3*MenuContentTransp, "%s: %i", vw_GetText("3_Money"), Setup.Profile[CurrentProfile].Money);
	vw_SetFontSize(Setup.FontSize);
	vw_SetFontOffsetY(8 - ceil(Setup.FontSize/2.0f) + 2);


	// проверяем состояние, если тянули и отжали, и сюда пришли - значит никто не перехватил, нужно сделать сброс
	if (!vw_GetWindowLBMouse(false) && DragWeapon) {
		// звук пропадающего оружия
		Audio_PlaySound2D(11,1.0f);

		// сброс
		DragWeapon = false;
		DragWeaponNum = 0;
		DragWeaponLevel = 0;
		DragWeaponAmmo = 0;
		DragWeaponAmmoStart = 0;
	}



	if (DragWeapon) CurrentCursorStatus = 3;


}



