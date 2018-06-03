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


	Web Site: https://www.viewizard.com/
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
	if (GameConfig().Profile[CurrentProfile].EnemyWeaponPenalty == 1)
		ButOff = true;
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Harder"), MenuContentTransp, ButOff)) {
		ChangeGameConfig().Profile[CurrentProfile].EnemyWeaponPenalty--;
		if (GameConfig().Profile[CurrentProfile].EnemyWeaponPenalty < 1)
			ChangeGameConfig().Profile[CurrentProfile].EnemyWeaponPenalty = 1;
		ProfileDifficulty(CurrentProfile, eDifficultyAction::Update);
	}
	ButOff = false;
	if (GameConfig().Profile[CurrentProfile].EnemyWeaponPenalty == 3)
		ButOff = true;
	if (DrawButton128_2(X1+360, Y1-6, vw_GetText("Easier"), MenuContentTransp, ButOff)) {
		ChangeGameConfig().Profile[CurrentProfile].EnemyWeaponPenalty++;
		if (GameConfig().Profile[CurrentProfile].EnemyWeaponPenalty > 3)
				ChangeGameConfig().Profile[CurrentProfile].EnemyWeaponPenalty = 3;
		ProfileDifficulty(CurrentProfile, eDifficultyAction::Update);
	}
	if (GameConfig().Profile[CurrentProfile].EnemyWeaponPenalty == 1) {
		Size = vw_TextWidth(vw_GetText("None"));
		SizeI = (110-Size)/2;
		vw_DrawText(X1+498+SizeI, Y1, 0, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("None"));
	} else {
		Size = vw_TextWidth("x%i", GameConfig().Profile[CurrentProfile].EnemyWeaponPenalty);
		SizeI = (110-Size)/2;
		vw_DrawText(X1+498+SizeI, Y1, 0, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "x%i", GameConfig().Profile[CurrentProfile].EnemyWeaponPenalty);
	}




	Y1 += Prir1;
	vw_DrawText(X1, Y1, -340, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Enemy Armor Penalty"));
	ButOff = false;
	if (GameConfig().Profile[CurrentProfile].EnemyArmorPenalty == 1)
		ButOff = true;
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Harder"), MenuContentTransp, ButOff)) {
		ChangeGameConfig().Profile[CurrentProfile].EnemyArmorPenalty--;
		if (GameConfig().Profile[CurrentProfile].EnemyArmorPenalty < 1)
			ChangeGameConfig().Profile[CurrentProfile].EnemyArmorPenalty = 1;
		ProfileDifficulty(CurrentProfile, eDifficultyAction::Update);
	}
	ButOff = false;
	if (GameConfig().Profile[CurrentProfile].EnemyArmorPenalty == 4)
		ButOff = true;
	if (DrawButton128_2(X1+360, Y1-6, vw_GetText("Easier"), MenuContentTransp, ButOff)) {
		ChangeGameConfig().Profile[CurrentProfile].EnemyArmorPenalty++;
		if (GameConfig().Profile[CurrentProfile].EnemyArmorPenalty > 4)
			ChangeGameConfig().Profile[CurrentProfile].EnemyArmorPenalty = 4;
		ProfileDifficulty(CurrentProfile, eDifficultyAction::Update);
	}
	if (GameConfig().Profile[CurrentProfile].EnemyArmorPenalty == 1) {
		Size = vw_TextWidth(vw_GetText("None"));
		SizeI = (110-Size)/2;
		vw_DrawText(X1+498+SizeI, Y1, 0, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("None"));
	} else {
		Size = vw_TextWidth("x%i", GameConfig().Profile[CurrentProfile].EnemyArmorPenalty);
		SizeI = (110-Size)/2;
		vw_DrawText(X1+498+SizeI, Y1, 0, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "x%i", GameConfig().Profile[CurrentProfile].EnemyArmorPenalty);
	}




	Y1 += Prir1;
	vw_DrawText(X1, Y1, -340, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Enemy Targeting Penalty"));
	ButOff = false;
	if (GameConfig().Profile[CurrentProfile].EnemyTargetingSpeedPenalty == 1)
		ButOff = true;
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Harder"), MenuContentTransp, ButOff)) {
		ChangeGameConfig().Profile[CurrentProfile].EnemyTargetingSpeedPenalty--;
		if (GameConfig().Profile[CurrentProfile].EnemyTargetingSpeedPenalty < 1)
			ChangeGameConfig().Profile[CurrentProfile].EnemyTargetingSpeedPenalty = 1;
		ProfileDifficulty(CurrentProfile, eDifficultyAction::Update);
	}
	ButOff = false;
	if (GameConfig().Profile[CurrentProfile].EnemyTargetingSpeedPenalty == 4)
		ButOff = true;
	if (DrawButton128_2(X1+360, Y1-6, vw_GetText("Easier"), MenuContentTransp, ButOff)) {
		ChangeGameConfig().Profile[CurrentProfile].EnemyTargetingSpeedPenalty++;
		if (GameConfig().Profile[CurrentProfile].EnemyTargetingSpeedPenalty > 4)
			ChangeGameConfig().Profile[CurrentProfile].EnemyTargetingSpeedPenalty = 4;
		ProfileDifficulty(CurrentProfile, eDifficultyAction::Update);
	}
	if (GameConfig().Profile[CurrentProfile].EnemyTargetingSpeedPenalty == 1) {
		Size = vw_TextWidth(vw_GetText("None"));
		SizeI = (110-Size)/2;
		vw_DrawText(X1+498+SizeI, Y1, 0, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("None"));
	} else {
		Size = vw_TextWidth("x%i", GameConfig().Profile[CurrentProfile].EnemyTargetingSpeedPenalty);
		SizeI = (110-Size)/2;
		vw_DrawText(X1+498+SizeI, Y1, 0, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "x%i", GameConfig().Profile[CurrentProfile].EnemyTargetingSpeedPenalty);
	}




	Y1 += Prir1;
	vw_DrawText(X1, Y1, -340, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Limited Ammo"));
	if (DrawButton128_2(X1+360+128, Y1-6, vw_GetText(OnOff[GameConfig().Profile[CurrentProfile].LimitedAmmo]), MenuContentTransp, false)) {
		ChangeGameConfig().Profile[CurrentProfile].LimitedAmmo++;
		if (GameConfig().Profile[CurrentProfile].LimitedAmmo > 1)
			ChangeGameConfig().Profile[CurrentProfile].LimitedAmmo = 0;
		ProfileDifficulty(CurrentProfile, eDifficultyAction::Update);
	}




	Y1 += Prir1;
	vw_DrawText(X1, Y1, -340, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Destroyable Weapon"));
	if (DrawButton128_2(X1+360+128, Y1-6, vw_GetText(OnOff[GameConfig().Profile[CurrentProfile].DestroyableWeapon]), MenuContentTransp, false)) {
		ChangeGameConfig().Profile[CurrentProfile].DestroyableWeapon++;
		if (GameConfig().Profile[CurrentProfile].DestroyableWeapon > 1)
			ChangeGameConfig().Profile[CurrentProfile].DestroyableWeapon = 0;
		ProfileDifficulty(CurrentProfile, eDifficultyAction::Update);
	}





	Y1 += Prir1;
	vw_DrawText(X1, Y1, -340, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("Weapon Targeting Mode"));
	if (DrawButton128_2(X1+360+128, Y1-6, vw_GetText(ArcadeSim[GameConfig().Profile[CurrentProfile].WeaponTargetingMode]), MenuContentTransp, false)) {
		ChangeGameConfig().Profile[CurrentProfile].WeaponTargetingMode++;
		if (GameConfig().Profile[CurrentProfile].WeaponTargetingMode > 1)
			ChangeGameConfig().Profile[CurrentProfile].WeaponTargetingMode = 0;
		ProfileDifficulty(CurrentProfile, eDifficultyAction::Update);
	}




	Y1 += Prir1;
	vw_DrawText(X1, Y1, -340, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, vw_GetText("SpaceShip Control Mode"));
	if (DrawButton128_2(X1+360+128, Y1-6, vw_GetText(ArcadeSim[GameConfig().Profile[CurrentProfile].SpaceShipControlMode]), MenuContentTransp, false)) {
		ChangeGameConfig().Profile[CurrentProfile].SpaceShipControlMode++;
		if (GameConfig().Profile[CurrentProfile].SpaceShipControlMode > 1)
			ChangeGameConfig().Profile[CurrentProfile].SpaceShipControlMode = 0;
		ProfileDifficulty(CurrentProfile, eDifficultyAction::Update);
	}







	Y1 += Prir1;
	Size = vw_TextWidth("%s: %i%%", vw_GetText("Current Profile Difficulty"), ProfileDifficulty(CurrentProfile));
	SizeI = (GameConfig().InternalWidth - Size) / 2;
	vw_DrawText(SizeI, Y1, 0, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, "%s: %i%%", vw_GetText("Current Profile Difficulty"), ProfileDifficulty(CurrentProfile));








	int X = GameConfig().InternalWidth / 2 - 192;
	int Y = 175+95*5;
	if (DrawButton384(X,Y, vw_GetText("BACK"), MenuContentTransp, &Button1Transp, &LastButton1UpdateTime))
		ComBuffer = eCommand::SWITCH_TO_PROFILE;




}
