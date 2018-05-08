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


const char *OnOff[2] = {
	"On",
	"Off"
};


const char *ArcadeSim[2] = {
	"Sim",
	"Arcade"
};


void DifficultyMenu()
{
	sRECT SrcRect(2, 2, 861, 482);
	sRECT DstRect(GameConfig().InternalWidth / 2 - 427, 160, GameConfig().InternalWidth / 2 + 432, 160 + 480);
	vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/panel800_444_back.tga"), true, 0.9f * MenuContentTransp);

	int X1 = GameConfig().InternalWidth / 2 - 372;
	int Y1 = 217;
	int Prir1 = 50;
	int Size;
	int SizeI;



	vw_DrawText(X1, Y1, -340, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Enemy Weapon Penalty"));
	bool ButOff = false;
	if (GameConfig().Profile[CurrentProfile].NPCWeaponPenalty == 1)
		ButOff = true;
	if (DrawButton128_2(X1+360, Y1-6, vw_GetText("Decrease"), MenuContentTransp, ButOff)) {
		ChangeGameConfig().Profile[CurrentProfile].NPCWeaponPenalty--;
		if (GameConfig().Profile[CurrentProfile].NPCWeaponPenalty < 1)
			ChangeGameConfig().Profile[CurrentProfile].NPCWeaponPenalty = 1;
	}
	ButOff = false;
	if (GameConfig().Profile[CurrentProfile].NPCWeaponPenalty == 3)
		ButOff = true;
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Increase"), MenuContentTransp, ButOff)) {
		ChangeGameConfig().Profile[CurrentProfile].NPCWeaponPenalty++;
		if (GameConfig().Profile[CurrentProfile].NPCWeaponPenalty > 3)
				ChangeGameConfig().Profile[CurrentProfile].NPCWeaponPenalty = 3;
	}
	if (GameConfig().Profile[CurrentProfile].NPCWeaponPenalty == 1) {
		Size = vw_FontSize(vw_GetText("None"));
		SizeI = (110-Size)/2;
		vw_DrawText(X1+498+SizeI, Y1, 0, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("None"));
	} else {
		Size = vw_FontSize("x%i", GameConfig().Profile[CurrentProfile].NPCWeaponPenalty);
		SizeI = (110-Size)/2;
		vw_DrawText(X1+498+SizeI, Y1, 0, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "x%i", GameConfig().Profile[CurrentProfile].NPCWeaponPenalty);
	}




	Y1 += Prir1;
	vw_DrawText(X1, Y1, -340, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Enemy Armor Penalty"));
	ButOff = false;
	if (GameConfig().Profile[CurrentProfile].NPCArmorPenalty == 1)
		ButOff = true;
	if (DrawButton128_2(X1+360, Y1-6, vw_GetText("Decrease"), MenuContentTransp, ButOff)) {
		ChangeGameConfig().Profile[CurrentProfile].NPCArmorPenalty--;
		if (GameConfig().Profile[CurrentProfile].NPCArmorPenalty < 1)
			ChangeGameConfig().Profile[CurrentProfile].NPCArmorPenalty = 1;
	}
	ButOff = false;
	if (GameConfig().Profile[CurrentProfile].NPCArmorPenalty == 4)
		ButOff = true;
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Increase"), MenuContentTransp, ButOff)) {
		ChangeGameConfig().Profile[CurrentProfile].NPCArmorPenalty++;
		if (GameConfig().Profile[CurrentProfile].NPCArmorPenalty > 4)
			ChangeGameConfig().Profile[CurrentProfile].NPCArmorPenalty = 4;
	}
	if (GameConfig().Profile[CurrentProfile].NPCArmorPenalty == 1) {
		Size = vw_FontSize(vw_GetText("None"));
		SizeI = (110-Size)/2;
		vw_DrawText(X1+498+SizeI, Y1, 0, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("None"));
	} else {
		Size = vw_FontSize("x%i", GameConfig().Profile[CurrentProfile].NPCArmorPenalty);
		SizeI = (110-Size)/2;
		vw_DrawText(X1+498+SizeI, Y1, 0, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "x%i", GameConfig().Profile[CurrentProfile].NPCArmorPenalty);
	}




	Y1 += Prir1;
	vw_DrawText(X1, Y1, -340, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Enemy Targeting Penalty"));
	ButOff = false;
	if (GameConfig().Profile[CurrentProfile].NPCTargetingSpeedPenalty == 1)
		ButOff = true;
	if (DrawButton128_2(X1+360, Y1-6, vw_GetText("Decrease"), MenuContentTransp, ButOff)) {
		ChangeGameConfig().Profile[CurrentProfile].NPCTargetingSpeedPenalty--;
		if (GameConfig().Profile[CurrentProfile].NPCTargetingSpeedPenalty < 1)
			ChangeGameConfig().Profile[CurrentProfile].NPCTargetingSpeedPenalty = 1;
	}
	ButOff = false;
	if (GameConfig().Profile[CurrentProfile].NPCTargetingSpeedPenalty == 4)
		ButOff = true;
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Increase"), MenuContentTransp, ButOff)) {
		ChangeGameConfig().Profile[CurrentProfile].NPCTargetingSpeedPenalty++;
		if (GameConfig().Profile[CurrentProfile].NPCTargetingSpeedPenalty > 4)
			ChangeGameConfig().Profile[CurrentProfile].NPCTargetingSpeedPenalty = 4;
	}
	if (GameConfig().Profile[CurrentProfile].NPCTargetingSpeedPenalty == 1) {
		Size = vw_FontSize(vw_GetText("None"));
		SizeI = (110-Size)/2;
		vw_DrawText(X1+498+SizeI, Y1, 0, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("None"));
	} else {
		Size = vw_FontSize("x%i", GameConfig().Profile[CurrentProfile].NPCTargetingSpeedPenalty);
		SizeI = (110-Size)/2;
		vw_DrawText(X1+498+SizeI, Y1, 0, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "x%i", GameConfig().Profile[CurrentProfile].NPCTargetingSpeedPenalty);
	}




	Y1 += Prir1;
	vw_DrawText(X1, Y1, -340, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Limited Ammo"));
	if (DrawButton128_2(X1+360+128, Y1-6, vw_GetText(OnOff[GameConfig().Profile[CurrentProfile].LimitedAmmo]), MenuContentTransp, false)) {
		ChangeGameConfig().Profile[CurrentProfile].LimitedAmmo++;
		if (GameConfig().Profile[CurrentProfile].LimitedAmmo > 1)
			ChangeGameConfig().Profile[CurrentProfile].LimitedAmmo = 0;
	}




	Y1 += Prir1;
	vw_DrawText(X1, Y1, -340, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Destroyable Weapon"));
	if (DrawButton128_2(X1+360+128, Y1-6, vw_GetText(OnOff[GameConfig().Profile[CurrentProfile].DestroyableWeapon]), MenuContentTransp, false)) {
		ChangeGameConfig().Profile[CurrentProfile].DestroyableWeapon++;
		if (GameConfig().Profile[CurrentProfile].DestroyableWeapon > 1)
			ChangeGameConfig().Profile[CurrentProfile].DestroyableWeapon = 0;
	}





	Y1 += Prir1;
	vw_DrawText(X1, Y1, -340, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Weapon Targeting Mode"));
	if (DrawButton128_2(X1+360+128, Y1-6, vw_GetText(ArcadeSim[GameConfig().Profile[CurrentProfile].WeaponTargetingMode]), MenuContentTransp, false)) {
		ChangeGameConfig().Profile[CurrentProfile].WeaponTargetingMode++;
		if (GameConfig().Profile[CurrentProfile].WeaponTargetingMode > 1)
			ChangeGameConfig().Profile[CurrentProfile].WeaponTargetingMode = 0;
	}




	Y1 += Prir1;
	vw_DrawText(X1, Y1, -340, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("SpaceShip Control Mode"));
	if (DrawButton128_2(X1+360+128, Y1-6, vw_GetText(ArcadeSim[GameConfig().Profile[CurrentProfile].SpaceShipControlMode]), MenuContentTransp, false)) {
		ChangeGameConfig().Profile[CurrentProfile].SpaceShipControlMode++;
		if (GameConfig().Profile[CurrentProfile].SpaceShipControlMode > 1)
			ChangeGameConfig().Profile[CurrentProfile].SpaceShipControlMode = 0;
	}





	// считаем общую сложность
	ChangeGameConfig().Profile[CurrentProfile].Difficulty = 100 - (
			(GameConfig().Profile[CurrentProfile].NPCWeaponPenalty - 1) * 6 +
			(GameConfig().Profile[CurrentProfile].NPCArmorPenalty - 1) * 6 +
			(GameConfig().Profile[CurrentProfile].NPCTargetingSpeedPenalty - 1) * 6 +
			GameConfig().Profile[CurrentProfile].LimitedAmmo * 14 +
			GameConfig().Profile[CurrentProfile].DestroyableWeapon * 11 +
			GameConfig().Profile[CurrentProfile].WeaponTargetingMode * 12 +
			GameConfig().Profile[CurrentProfile].SpaceShipControlMode * 15);



	Y1 += Prir1;
	Size = vw_FontSize("%s: %i%%", vw_GetText("Current Profile Difficulty"), GameConfig().Profile[CurrentProfile].Difficulty);
	SizeI = (GameConfig().InternalWidth - Size) / 2;
	vw_DrawText(SizeI, Y1, 0, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, "%s: %i%%", vw_GetText("Current Profile Difficulty"), GameConfig().Profile[CurrentProfile].Difficulty);








	int X = GameConfig().InternalWidth / 2 - 192;
	int Y = 175+95*5;
	if (DrawButton384(X,Y, vw_GetText("BACK"), MenuContentTransp, &Button1Transp, &LastButton1UpdateTime))
		ComBuffer = eCommand::SWITCH_TO_PROFILE;




}
