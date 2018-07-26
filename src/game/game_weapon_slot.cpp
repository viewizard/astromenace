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
#include "../assets/texture.h"
#include "../object3d/space_ship/space_ship.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

// работа с морганием вывода
extern float CurrentAlert2;
extern float CurrentAlert3;

extern std::weak_ptr<cSpaceShip> PlayerFighter;
int RightDrawLevelPos = 1;
int LeftDrawLevelPos = 1;


//------------------------------------------------------------------------------------
// Прорисовка левого слота
//------------------------------------------------------------------------------------
void DrawGameWeaponLeftSlot(int WeaponNum, int DrawLevelPos)
{
	auto sharedPlayerFighter = PlayerFighter.lock();
	if (!sharedPlayerFighter)
		return;

	auto sharedWeapon = sharedPlayerFighter->WeaponSlots[WeaponNum].Weapon.lock();
	if (!sharedWeapon)
		return;

	sRECT SrcRect, DstRect;

	int Xpos = 0;
	int Ypos = 150;
	switch (DrawLevelPos) {
	case 1:
		Ypos = 80;
		break;
	case 2:
		Ypos = 150;
		break;
	case 3:
		Ypos = 220;
		break;
	}

	if (GameConfig().GameWeaponInfoType == 1) {
		Ypos += DrawLevelPos * 15 - 25;

		// выводим подложку меню - общую
		SrcRect(0, 0, 164, 88);
		DstRect(Xpos, Ypos, Xpos+164, Ypos+88);
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("game/weapon_panel_left.tga"), true, 1.0f);


		// подложка-состояния
		SrcRect(0, 0, 128, 64);
		DstRect(Xpos+24, Ypos+12, Xpos+24+128, Ypos+64+12);
		// пушка работает или нет?
		if (sharedWeapon->Strength <= 0.0f) {
			vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/weapon_on_icon.tga"), true, CurrentAlert3, 0.0f, sRGBCOLOR{1.0f, 0.0f, 0.0f});

			// иконка оружия
			SrcRect(0, 0, 128, 64);
			DstRect(Xpos + 24, Ypos + 12, Xpos + 24 + 128, Ypos + 64 + 12);
			vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(GetWeaponIconName(sharedWeapon->InternalType)), true, 1.0f);
		} else {
			if (sharedWeapon->CurrentEnergyAccumulated < sharedWeapon->EnergyUse)
				vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/weapon_on_icon.tga"), true, CurrentAlert3, 0.0f, sRGBCOLOR{0.0f, 1.0f, 1.0f});
			else {
				if (sharedWeapon->Ammo == 0)
					vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/weapon_on_icon.tga"), true, CurrentAlert3, 0.0f, sRGBCOLOR{1.0f, 0.5f, 0.2f});
				else
					vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/weapon_on_icon.tga"), true, 1.0f, 0.0f, sRGBCOLOR{0.0f, 1.0f, 0.0f});
			}

			// иконка оружия
			SrcRect(0, 0, 128, 64);
			DstRect(Xpos+24, Ypos+12, Xpos+24+128, Ypos+64+12);
			vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(GetWeaponIconName(sharedWeapon->InternalType)), true, 1.0f);

			// боекомплект
			int AmmoShow = (int)((56.0f*(sharedWeapon->AmmoStart-sharedWeapon->Ammo))/sharedWeapon->AmmoStart);
			// если меняли боекомплект и сделали его меньше, чтобы не вылазила линия боекомплекта...
			if (AmmoShow < 0) AmmoShow = 0;

			SrcRect(0, AmmoShow, 8, 56);
			DstRect(Xpos+2, Ypos+16+AmmoShow, Xpos+8+2, Ypos+56+16);
			vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("game/weapon_ammo.tga"), true, 1.0f);

			// перезарядка
			int ReloadShow = (int)(56.0f - (56.0f*(sharedPlayerFighter->TimeLastUpdate-sharedWeapon->LastFireTime))/sharedWeapon->NextFireTime);
			// особый случай, рой ракет
			if (sharedWeapon->InternalType == 17 && sharedWeapon->SwarmNum > 0) ReloadShow = 56;
			if (ReloadShow<0) ReloadShow = 0;
			SrcRect(0,ReloadShow,8,56);
			DstRect(Xpos+12,Ypos+16+ReloadShow,Xpos+12+8,Ypos+56+16);
			vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("game/weapon_energy.tga"), true, 1.0f);
		}
	}
	if (GameConfig().GameWeaponInfoType == 2) {
		// выводим подложку меню - общую
		SrcRect(0,0,2,2);
		DstRect(Xpos,Ypos,Xpos+128+18+6,Ypos+64+4);
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/blackpoint.tga"), true, 0.2f);

		// выводим подложку меню - под иконку
		SrcRect(0,0,2,2);
		DstRect(Xpos+23,Ypos+1,Xpos+23+128,Ypos+64+3);
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/blackpoint.tga"), true, 0.5f);

		// выводим подложку меню - под боекомплект
		SrcRect(0,0,2,2);
		DstRect(Xpos+1,Ypos+1,Xpos+8+3,Ypos+64+3);
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/blackpoint.tga"), true, 0.5f);

		// выводим подложку меню - под перезарядка
		SrcRect(0,0,2,2);
		DstRect(Xpos+12,Ypos+1,Xpos+12+8+2,Ypos+64+3);
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/blackpoint.tga"), true, 0.5f);

		// подложка-состояния
		SrcRect(0,0,128,64);
		DstRect(Xpos+23,Ypos+2,Xpos+23+128,Ypos+64+2);

		// пушка работает или нет?
		if (sharedWeapon->Strength <= 0.0f) {
			vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/weapon_on_icon.tga"), true, CurrentAlert3, 0.0f, sRGBCOLOR{1.0f, 0.0f, 0.0f});

			// иконка оружия
			SrcRect(0,0,128,64);
			DstRect(Xpos+23,Ypos+2,Xpos+23+128,Ypos+64+2);
			vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(GetWeaponIconName(sharedWeapon->InternalType)), true, 1.0f);
		} else {
			if (sharedWeapon->CurrentEnergyAccumulated < sharedWeapon->EnergyUse)
				vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/weapon_on_icon.tga"), true, CurrentAlert3, 0.0f, sRGBCOLOR{0.0f, 1.0f, 1.0f});
			else {
				if (sharedWeapon->Ammo == 0)
					vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/weapon_on_icon.tga"), true, CurrentAlert3, 0.0f, sRGBCOLOR{1.0f, 0.5f, 0.2f});
				else
					vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/weapon_on_icon.tga"), true, 1.0f, 0.0f, sRGBCOLOR{0.0f, 1.0f, 0.0f});
			}
			// иконка оружия
			SrcRect(0,0,128,64);
			DstRect(Xpos+23,Ypos+2,Xpos+23+128,Ypos+64+2);
			vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(GetWeaponIconName(sharedWeapon->InternalType)), true, 1.0f);

			// боекомплект
			int AmmoShow = (int)((64.0f*(sharedWeapon->AmmoStart-sharedWeapon->Ammo))/sharedWeapon->AmmoStart);
			// если меняли боекомплект и сделали его меньше, чтобы не вылазила линия боекомплекта...
			if (AmmoShow < 0) AmmoShow = 0;

			SrcRect(0, AmmoShow, 8, 64);
			DstRect(Xpos+2, Ypos+2+AmmoShow, Xpos+8+2, Ypos+64+2);
			vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("game/ammo.tga"), true, 1.0f);

			// перезарядка
			int ReloadShow = (int)(64.0f - (64.0f*(sharedPlayerFighter->TimeLastUpdate-sharedWeapon->LastFireTime))/sharedWeapon->NextFireTime);
			// особый случай, рой ракет
			if (sharedWeapon->InternalType == 17 && sharedWeapon->SwarmNum > 0) ReloadShow = 64;
			if (ReloadShow<0) ReloadShow = 0;
			SrcRect(0,ReloadShow,8,64);
			DstRect(Xpos+12+1,Ypos+2+ReloadShow,Xpos+12+8+1,Ypos+64+2);
			vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("game/energy.tga"), true, 1.0f);
		}
	}
	if (GameConfig().GameWeaponInfoType == 3) {
		// выводим подложку меню - общую
		SrcRect(0,0,2,2);
		DstRect(Xpos,Ypos,Xpos+18+5,Ypos+64+4);
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/blackpoint.tga"), true, 0.2f);

		// выводим подложку меню - под боекомплект
		SrcRect(0,0,2,2);
		DstRect(Xpos+1,Ypos+1,Xpos+8+3,Ypos+64+3);
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/blackpoint.tga"), true, 0.5f);
		// выводим подложку меню - под перезарядка
		SrcRect(0,0,2,2);
		DstRect(Xpos+12,Ypos+1,Xpos+12+8+2,Ypos+64+3);
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/blackpoint.tga"), true, 0.5f);

		// пушка работает или нет?
		if (sharedWeapon->Strength <= 0.0f) {

		} else {
			// боекомплект
			int AmmoShow = (int)((64.0f*(sharedWeapon->AmmoStart-sharedWeapon->Ammo))/sharedWeapon->AmmoStart);
			// если меняли боекомплект и сделали его меньше, чтобы не вылазила линия боекомплекта...
			if (AmmoShow < 0) AmmoShow = 0;

			SrcRect(0,AmmoShow,8,64);
			DstRect(Xpos+2,Ypos+2+AmmoShow,Xpos+8+2,Ypos+64+2);
			vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("game/ammo.tga"), true, 1.0f);

			// перезарядка
			int ReloadShow = (int)(64.0f - (64.0f*(sharedPlayerFighter->TimeLastUpdate-sharedWeapon->LastFireTime))/sharedWeapon->NextFireTime);
			// особый случай, рой ракет
			if (sharedWeapon->InternalType == 17 && sharedWeapon->SwarmNum > 0) ReloadShow = 64;
			if (ReloadShow<0) ReloadShow = 0;
			SrcRect(0,ReloadShow,8,64);
			DstRect(Xpos+12+1,Ypos+2+ReloadShow,Xpos+12+8+1,Ypos+64+2);
			vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("game/energy.tga"), true, 1.0f);
		}
	}



	// этот уровень уже занят, переходим к следующему для нового оружия
	RightDrawLevelPos++;
}







//------------------------------------------------------------------------------------
// Прорисовка правого слота
//------------------------------------------------------------------------------------
void DrawGameWeaponRightSlot(int WeaponNum, int DrawLevelPos)
{
	auto sharedPlayerFighter = PlayerFighter.lock();
	if (!sharedPlayerFighter)
		return;

	auto sharedWeapon = sharedPlayerFighter->WeaponSlots[WeaponNum].Weapon.lock();
	if (!sharedWeapon)
		return;

	sRECT SrcRect, DstRect;

	int Xpos = GameConfig().InternalWidth - 152;
	int Ypos = 80;
	switch (DrawLevelPos) {
	case 1:
		Ypos = 80;
		break;
	case 2:
		Ypos = 150;
		break;
	case 3:
		Ypos = 220;
		break;
	}

	if (GameConfig().GameWeaponInfoType == 1) {
		Ypos += DrawLevelPos*15 - 25;
		Xpos = GameConfig().InternalWidth - 164;

		// выводим подложку меню - общую
		SrcRect(0,0,164,88);
		DstRect(Xpos,Ypos,Xpos+164,Ypos+88);
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("game/weapon_panel_right.tga"), true, 1.0f);

		// подложка-состояния
		SrcRect(0,0,128,64);
		DstRect(Xpos+12,Ypos+12,Xpos+12+128,Ypos+64+12);
		// пушка работает или нет?
		if (sharedWeapon->Strength <= 0.0f) {
			vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/weapon_on_icon.tga"), true, CurrentAlert3, 0.0f, sRGBCOLOR{1.0f, 0.0f, 0.0f});

			// иконка оружия
			SrcRect(0,0,128,64);
			DstRect(Xpos+12,Ypos+12,Xpos+12+128,Ypos+64+12);
			vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(GetWeaponIconName(sharedWeapon->InternalType)), true, 1.0f);
		} else {
			if (sharedWeapon->CurrentEnergyAccumulated < sharedWeapon->EnergyUse)
				vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/weapon_on_icon.tga"), true, CurrentAlert3, 0.0f, sRGBCOLOR{0.0f, 1.0f, 1.0f});
			else {
				if (sharedWeapon->Ammo == 0)
					vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/weapon_on_icon.tga"), true, CurrentAlert3, 0.0f, sRGBCOLOR{1.0f, 0.5f, 0.2f});
				else
					vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/weapon_on_icon.tga"), true, 1.0f, 0.0f, sRGBCOLOR{0.0f, 1.0f, 0.0f});
			}
			// иконка оружия
			SrcRect(0,0,128,64);
			DstRect(Xpos+12,Ypos+12,Xpos+12+128,Ypos+64+12);
			vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(GetWeaponIconName(sharedWeapon->InternalType)), true, 1.0f);

			// боекомплект
			int AmmoShow = (int)((56.0f*(sharedWeapon->AmmoStart-sharedWeapon->Ammo))/sharedWeapon->AmmoStart);
			// если меняли боекомплект и сделали его меньше, чтобы не вылазила линия боекомплекта...
			if (AmmoShow < 0) AmmoShow = 0;

			SrcRect(0,AmmoShow,8,56);
			DstRect(Xpos+154,Ypos+16+AmmoShow,Xpos+8+154,Ypos+56+16);
			vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("game/weapon_ammo.tga"), true, 1.0f);

			// перезарядка
			int ReloadShow = (int)(56.0f - (56.0f*(sharedPlayerFighter->TimeLastUpdate-sharedWeapon->LastFireTime))/sharedWeapon->NextFireTime);
			// особый случай, рой ракет
			if (sharedWeapon->InternalType == 17 && sharedWeapon->SwarmNum > 0) ReloadShow = 56;
			if (ReloadShow<0) ReloadShow = 0;
			SrcRect(0,ReloadShow,8,56);
			DstRect(Xpos+144,Ypos+16+ReloadShow,Xpos+144+8,Ypos+56+16);
			vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("game/weapon_energy.tga"), true, 1.0f);
		}
	}
	if (GameConfig().GameWeaponInfoType == 2) {
		// выводим подложку меню - общую
		SrcRect(0,0,2,2);
		DstRect(Xpos,Ypos,Xpos+128+18+6,Ypos+64+4);
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/blackpoint.tga"), true, 0.2f);

		// выводим подложку меню - под иконку
		SrcRect(0,0,2,2);
		DstRect(Xpos+1,Ypos+1,Xpos+1+128,Ypos+64+3);
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/blackpoint.tga"), true, 0.5f);

		// выводим подложку меню - под боекомплект
		SrcRect(0,0,2,2);
		DstRect(Xpos+13+128,Ypos+1,Xpos+13+8+2+128,Ypos+64+3);
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/blackpoint.tga"), true, 0.5f);

		// выводим подложку меню - под перезарядка
		SrcRect(0,0,2,2);
		DstRect(Xpos+2+128,Ypos+1,Xpos+8+4+128,Ypos+64+3);
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/blackpoint.tga"), true, 0.5f);

		// подложка-состояния
		SrcRect(0,0,128,64);
		DstRect(Xpos+1,Ypos+2,Xpos+1+128,Ypos+64+2);
		// пушка работает или нет?
		if (sharedWeapon->Strength <= 0.0f) {
			vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/weapon_on_icon.tga"), true, CurrentAlert3, 0.0f, sRGBCOLOR{1.0f, 0.0f, 0.0f});

			// иконка оружия
			SrcRect(0,0,128,64);
			DstRect(Xpos+1,Ypos+2,Xpos+1+128,Ypos+64+2);
			vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(GetWeaponIconName(sharedWeapon->InternalType)), true, 1.0f);
		} else {
			if (sharedWeapon->CurrentEnergyAccumulated < sharedWeapon->EnergyUse)
				vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/weapon_on_icon.tga"), true, CurrentAlert3, 0.0f, sRGBCOLOR{0.0f, 1.0f, 1.0f});
			else {
				if (sharedWeapon->Ammo == 0)
					vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/weapon_on_icon.tga"), true, CurrentAlert3, 0.0f, sRGBCOLOR{1.0f, 0.5f, 0.2f});
				else
					vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/weapon_on_icon.tga"), true, 1.0f, 0.0f, sRGBCOLOR{0.0f, 1.0f, 0.0f});
			}
			// иконка оружия
			SrcRect(0,0,128,64);
			DstRect(Xpos+1,Ypos+2,Xpos+1+128,Ypos+64+2);
			vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(GetWeaponIconName(sharedWeapon->InternalType)), true, 1.0f);

			// боекомплект
			int AmmoShow = (int)((64.0f*(sharedWeapon->AmmoStart-sharedWeapon->Ammo))/sharedWeapon->AmmoStart);
			// если меняли боекомплект и сделали его меньше, чтобы не вылазила линия боекомплекта...
			if (AmmoShow < 0) AmmoShow = 0;

			SrcRect(0,AmmoShow,8,64);
			DstRect(Xpos+12+2+128,Ypos+2+AmmoShow,Xpos+12+8+2+128,Ypos+64+2);
			vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("game/ammo.tga"), true, 1.0f);

			// перезарядка
			int ReloadShow = (int)(64.0f - (64.0f*(sharedPlayerFighter->TimeLastUpdate-sharedWeapon->LastFireTime))/sharedWeapon->NextFireTime);
			// особый случай, рой ракет
			if (sharedWeapon->InternalType == 17 && sharedWeapon->SwarmNum > 0) ReloadShow = 64;
			if (ReloadShow<0) ReloadShow = 0;
			SrcRect(0,ReloadShow,8,64);
			DstRect(Xpos+3+128,Ypos+2+ReloadShow,Xpos+8+3+128,Ypos+64+2);
			vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("game/energy.tga"), true, 1.0f);
		}
	}
	if (GameConfig().GameWeaponInfoType == 3) {
		// выводим подложку меню - общую
		SrcRect(0,0,2,2);
		DstRect(Xpos+128+1,Ypos,Xpos+128+18+6,Ypos+64+4);
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/blackpoint.tga"), true, 0.2f);

		// выводим подложку меню - под боекомплект
		SrcRect(0,0,2,2);
		DstRect(Xpos+13+128,Ypos+1,Xpos+13+8+2+128,Ypos+64+3);
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/blackpoint.tga"), true, 0.5f);

		// выводим подложку меню - под перезарядка
		SrcRect(0,0,2,2);
		DstRect(Xpos+2+128,Ypos+1,Xpos+8+4+128,Ypos+64+3);
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/blackpoint.tga"), true, 0.5f);

		// пушка работает или нет?
		if (sharedWeapon->Strength <= 0.0f) {

		} else {
			// боекомплект
			int AmmoShow = (int)((64.0f*(sharedWeapon->AmmoStart-sharedWeapon->Ammo))/sharedWeapon->AmmoStart);
			// если меняли боекомплект и сделали его меньше, чтобы не вылазила линия боекомплекта...
			if (AmmoShow < 0) AmmoShow = 0;

			SrcRect(0,AmmoShow,8,64);
			DstRect(Xpos+12+2+128,Ypos+2+AmmoShow,Xpos+12+8+2+128,Ypos+64+2);
			vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("game/ammo.tga"), true, 1.0f);

			// перезарядка
			int ReloadShow = (int)(64.0f - (64.0f*(sharedPlayerFighter->TimeLastUpdate-sharedWeapon->LastFireTime))/sharedWeapon->NextFireTime);
			// особый случай, рой ракет
			if (sharedWeapon->InternalType == 17 && sharedWeapon->SwarmNum > 0) ReloadShow = 64;
			if (ReloadShow<0) ReloadShow = 0;
			SrcRect(0,ReloadShow,8,64);
			DstRect(Xpos+3+128,Ypos+2+ReloadShow,Xpos+8+3+128,Ypos+64+2);
			vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("game/energy.tga"), true, 1.0f);
		}
	}

	// этот уровень уже занят, переходим к следующему для нового оружия
	LeftDrawLevelPos++;
}










//------------------------------------------------------------------------------------
// Прорисовка состояния всего оружия корабля игрока
//------------------------------------------------------------------------------------
void DrawGameWeaponSlots()
{
	// если корабля нет - нам тут делать нечего
	if (PlayerFighter.expired())
		return;

	// сбрасываем в первоначальное состояние счетчики уровня
	RightDrawLevelPos = 1;
	LeftDrawLevelPos = 1;


	switch (GameConfig().Profile[CurrentProfile].Ship) {
	case 1:
		DrawGameWeaponLeftSlot(2, RightDrawLevelPos);
		DrawGameWeaponRightSlot(3, LeftDrawLevelPos);
		DrawGameWeaponLeftSlot(4, RightDrawLevelPos);
		DrawGameWeaponLeftSlot(0, RightDrawLevelPos);
		DrawGameWeaponRightSlot(1, LeftDrawLevelPos);
		break;
	case 2:
		DrawGameWeaponLeftSlot(2, RightDrawLevelPos);
		DrawGameWeaponRightSlot(3, LeftDrawLevelPos);
		DrawGameWeaponLeftSlot(0, RightDrawLevelPos);
		DrawGameWeaponRightSlot(1, LeftDrawLevelPos);
		break;
	case 3:
		DrawGameWeaponLeftSlot(0, RightDrawLevelPos);
		DrawGameWeaponLeftSlot(1, RightDrawLevelPos);
		DrawGameWeaponRightSlot(2, LeftDrawLevelPos);
		break;
	case 4:
		DrawGameWeaponLeftSlot(4, RightDrawLevelPos);
		DrawGameWeaponLeftSlot(2, RightDrawLevelPos);
		DrawGameWeaponRightSlot(3, LeftDrawLevelPos);
		DrawGameWeaponLeftSlot(0, RightDrawLevelPos);
		DrawGameWeaponRightSlot(1, LeftDrawLevelPos);
		break;
	case 5:
		DrawGameWeaponLeftSlot(0, RightDrawLevelPos);
		DrawGameWeaponRightSlot(1, LeftDrawLevelPos);
		break;
	case 6:
		DrawGameWeaponLeftSlot(4, RightDrawLevelPos);
		DrawGameWeaponLeftSlot(2, RightDrawLevelPos);
		DrawGameWeaponRightSlot(3, LeftDrawLevelPos);
		DrawGameWeaponLeftSlot(0, RightDrawLevelPos);
		DrawGameWeaponRightSlot(1, LeftDrawLevelPos);
		break;
	case 7:
		DrawGameWeaponLeftSlot(0, RightDrawLevelPos);
		DrawGameWeaponLeftSlot(1, RightDrawLevelPos);
		DrawGameWeaponRightSlot(2, LeftDrawLevelPos);
		DrawGameWeaponLeftSlot(3, RightDrawLevelPos);
		DrawGameWeaponRightSlot(4, LeftDrawLevelPos);
		break;
	case 8:
		DrawGameWeaponLeftSlot(0, RightDrawLevelPos);
		DrawGameWeaponRightSlot(1, LeftDrawLevelPos);
		break;
	case 9:
		DrawGameWeaponLeftSlot(0, RightDrawLevelPos);
		DrawGameWeaponRightSlot(1, LeftDrawLevelPos);
		break;
	case 10:
		DrawGameWeaponLeftSlot(4, RightDrawLevelPos);
		DrawGameWeaponLeftSlot(2, RightDrawLevelPos);
		DrawGameWeaponRightSlot(3, LeftDrawLevelPos);
		DrawGameWeaponLeftSlot(0, RightDrawLevelPos);
		DrawGameWeaponRightSlot(1, LeftDrawLevelPos);
		break;
	case 11:
		DrawGameWeaponLeftSlot(2, RightDrawLevelPos);
		DrawGameWeaponLeftSlot(0, RightDrawLevelPos);
		DrawGameWeaponRightSlot(1, LeftDrawLevelPos);
		break;
	case 12:
		DrawGameWeaponLeftSlot(2, RightDrawLevelPos);
		DrawGameWeaponLeftSlot(0, RightDrawLevelPos);
		DrawGameWeaponRightSlot(1, LeftDrawLevelPos);
		break;
	case 13:
		DrawGameWeaponLeftSlot(3, RightDrawLevelPos);
		DrawGameWeaponRightSlot(4, LeftDrawLevelPos);
		DrawGameWeaponLeftSlot(1, RightDrawLevelPos);
		DrawGameWeaponRightSlot(2, LeftDrawLevelPos);
		DrawGameWeaponLeftSlot(0, RightDrawLevelPos);
		break;
	case 14:
		DrawGameWeaponLeftSlot(0, RightDrawLevelPos);
		DrawGameWeaponRightSlot(1, LeftDrawLevelPos);
		DrawGameWeaponLeftSlot(4, RightDrawLevelPos);
		DrawGameWeaponLeftSlot(2, RightDrawLevelPos);
		DrawGameWeaponRightSlot(3, LeftDrawLevelPos);
		break;
	case 15:
		DrawGameWeaponLeftSlot(0, RightDrawLevelPos);
		DrawGameWeaponLeftSlot(1, RightDrawLevelPos);
		DrawGameWeaponRightSlot(2, LeftDrawLevelPos);
		break;
	case 16:
		DrawGameWeaponLeftSlot(0, RightDrawLevelPos);
		DrawGameWeaponLeftSlot(1, RightDrawLevelPos);
		DrawGameWeaponRightSlot(2, LeftDrawLevelPos);
		break;
	case 17:
		DrawGameWeaponLeftSlot(4, RightDrawLevelPos);
		DrawGameWeaponLeftSlot(0, RightDrawLevelPos);
		DrawGameWeaponRightSlot(1, LeftDrawLevelPos);
		DrawGameWeaponLeftSlot(2, RightDrawLevelPos);
		DrawGameWeaponRightSlot(3, LeftDrawLevelPos);
		break;
	case 18:
		DrawGameWeaponLeftSlot(0, RightDrawLevelPos);
		DrawGameWeaponRightSlot(1, LeftDrawLevelPos);
		DrawGameWeaponLeftSlot(2, RightDrawLevelPos);
		DrawGameWeaponRightSlot(3, LeftDrawLevelPos);
		break;
	case 19:
		DrawGameWeaponLeftSlot(0, RightDrawLevelPos);
		DrawGameWeaponLeftSlot(1, RightDrawLevelPos);
		DrawGameWeaponRightSlot(2, LeftDrawLevelPos);
		break;
	case 20:
		DrawGameWeaponLeftSlot(0, RightDrawLevelPos);
		DrawGameWeaponRightSlot(1, LeftDrawLevelPos);
		DrawGameWeaponLeftSlot(2, RightDrawLevelPos);
		DrawGameWeaponRightSlot(3, LeftDrawLevelPos);
		break;
	case 21:
		DrawGameWeaponLeftSlot(2, RightDrawLevelPos);
		DrawGameWeaponLeftSlot(0, RightDrawLevelPos);
		DrawGameWeaponRightSlot(1, LeftDrawLevelPos);
		break;
	case 22:
		DrawGameWeaponLeftSlot(0, RightDrawLevelPos);
		break;
	default:
		std::cerr << __func__ << "(): " << "wrong Ship.\n";
		break;
	}


}

} // astromenace namespace
} // viewizard namespace
