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


	Website: https://viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

// TODO translate comments

#include "../core/core.h"
#include "../config/config.h"
#include "../assets/texture.h"
#include "../command.h"
#include "../game.h" // FIXME "game.h" should be replaced by individual headers
#include <sstream>
#include <iomanip>

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

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
	vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/panel800_444_back.tga"), true, 0.9f * MenuContentTransp);

	int X1 = GameConfig().InternalWidth / 2 - 372;
	int Y1 = 217;
	int Prir1 = 50;
	int Size;
	int SizeI;



	vw_DrawText(X1, Y1, -340, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetText("Enemy Weapon Penalty"));
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
		vw_DrawText(X1+498+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetText("None"));
	} else {
		std::ostringstream tmpStream;
		tmpStream << std::fixed << std::setprecision(0)
			  << "x" << static_cast<int>(GameConfig().Profile[CurrentProfile].EnemyWeaponPenalty);
		Size = vw_TextWidth(tmpStream.str().c_str());
		SizeI = (110-Size)/2;
		vw_DrawText(X1+498+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str().c_str());
	}




	Y1 += Prir1;
	vw_DrawText(X1, Y1, -340, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetText("Enemy Armor Penalty"));
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
		vw_DrawText(X1+498+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetText("None"));
	} else {
		std::ostringstream tmpStream;
		tmpStream << std::fixed << std::setprecision(0)
			  << "x" << static_cast<int>(GameConfig().Profile[CurrentProfile].EnemyArmorPenalty);
		Size = vw_TextWidth(tmpStream.str().c_str());
		SizeI = (110-Size)/2;
		vw_DrawText(X1+498+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str().c_str());
	}




	Y1 += Prir1;
	vw_DrawText(X1, Y1, -340, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetText("Enemy Targeting Penalty"));
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
		vw_DrawText(X1+498+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetText("None"));
	} else {
		std::ostringstream tmpStream;
		tmpStream << std::fixed << std::setprecision(0)
			  << "x" << static_cast<int>(GameConfig().Profile[CurrentProfile].EnemyTargetingSpeedPenalty);
		Size = vw_TextWidth(tmpStream.str().c_str());
		SizeI = (110-Size)/2;
		vw_DrawText(X1+498+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str().c_str());
	}




	// note, we use "Limited Ammo" option in menu, but UnlimitedAmmo variable
	Y1 += Prir1;
	vw_DrawText(X1, Y1, -340, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetText("Limited Ammo"));
	ButOff = !ChangeGameConfig().Profile[CurrentProfile].UnlimitedAmmo;
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Harder"), MenuContentTransp, ButOff)) {
		ChangeGameConfig().Profile[CurrentProfile].UnlimitedAmmo = 0;
		ProfileDifficulty(CurrentProfile, eDifficultyAction::Update);
	}
	ButOff = ChangeGameConfig().Profile[CurrentProfile].UnlimitedAmmo;
	if (DrawButton128_2(X1+360, Y1-6, vw_GetText("Easier"), MenuContentTransp, ButOff)) {
		ChangeGameConfig().Profile[CurrentProfile].UnlimitedAmmo = 1;
		ProfileDifficulty(CurrentProfile, eDifficultyAction::Update);
	}
	Size = vw_TextWidth(vw_GetText(OnOff[GameConfig().Profile[CurrentProfile].UnlimitedAmmo]));
	SizeI = (110-Size)/2;
	// OnOff[] = 0 - on (limited ammo), 1 - off (unlimited ammo)
	vw_DrawText(X1+498+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetText(OnOff[GameConfig().Profile[CurrentProfile].UnlimitedAmmo]));




	// note, we use "Destroyable Weapon" option in menu, but UndestroyableWeapon variable
	Y1 += Prir1;
	vw_DrawText(X1, Y1, -340, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetText("Destroyable Weapon"));
	ButOff = !ChangeGameConfig().Profile[CurrentProfile].UndestroyableWeapon;
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Harder"), MenuContentTransp, ButOff)) {
		ChangeGameConfig().Profile[CurrentProfile].UndestroyableWeapon = 0;
		ProfileDifficulty(CurrentProfile, eDifficultyAction::Update);
	}
	ButOff = ChangeGameConfig().Profile[CurrentProfile].UndestroyableWeapon;
	if (DrawButton128_2(X1+360, Y1-6, vw_GetText("Easier"), MenuContentTransp, ButOff)) {
		ChangeGameConfig().Profile[CurrentProfile].UndestroyableWeapon = 1;
		ProfileDifficulty(CurrentProfile, eDifficultyAction::Update);
	}
	Size = vw_TextWidth(vw_GetText(OnOff[GameConfig().Profile[CurrentProfile].UndestroyableWeapon]));
	SizeI = (110-Size)/2;
	// OnOff[] = 0 - on (destroyable weapon), 1 - off (undestroyable weapon)
	vw_DrawText(X1+498+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetText(OnOff[GameConfig().Profile[CurrentProfile].UndestroyableWeapon]));




	Y1 += Prir1;
	vw_DrawText(X1, Y1, -340, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetText("Weapon Targeting Mode"));
	ButOff = !ChangeGameConfig().Profile[CurrentProfile].WeaponTargetingMode;
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Harder"), MenuContentTransp, ButOff)) {
		ChangeGameConfig().Profile[CurrentProfile].WeaponTargetingMode = 0;
		ProfileDifficulty(CurrentProfile, eDifficultyAction::Update);
	}
	ButOff = ChangeGameConfig().Profile[CurrentProfile].WeaponTargetingMode;
	if (DrawButton128_2(X1+360, Y1-6, vw_GetText("Easier"), MenuContentTransp, ButOff)) {
		ChangeGameConfig().Profile[CurrentProfile].WeaponTargetingMode = 1;
		ProfileDifficulty(CurrentProfile, eDifficultyAction::Update);
	}
	Size = vw_TextWidth(vw_GetText(ArcadeSim[GameConfig().Profile[CurrentProfile].WeaponTargetingMode]));
	float WScale = 0;
	if (Size > 110) {
		Size = 110;
		WScale = -110;
	}
	SizeI = (110 - Size) / 2;
	vw_DrawText(X1+498+SizeI, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetText(ArcadeSim[GameConfig().Profile[CurrentProfile].WeaponTargetingMode]));




	Y1 += Prir1;
	vw_DrawText(X1, Y1, -340, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetText("SpaceShip Control Mode"));
	ButOff = !ChangeGameConfig().Profile[CurrentProfile].SpaceShipControlMode;
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Harder"), MenuContentTransp, ButOff)) {
		ChangeGameConfig().Profile[CurrentProfile].SpaceShipControlMode = 0;
		ProfileDifficulty(CurrentProfile, eDifficultyAction::Update);
	}
	ButOff = ChangeGameConfig().Profile[CurrentProfile].SpaceShipControlMode;
	if (DrawButton128_2(X1+360, Y1-6, vw_GetText("Easier"), MenuContentTransp, ButOff)) {
		ChangeGameConfig().Profile[CurrentProfile].SpaceShipControlMode = 1;
		ProfileDifficulty(CurrentProfile, eDifficultyAction::Update);
	}
	Size = vw_TextWidth(vw_GetText(ArcadeSim[GameConfig().Profile[CurrentProfile].SpaceShipControlMode]));
	WScale = 0;
	if (Size > 110) {
		Size = 110;
		WScale = -110;
	}
	SizeI = (110 - Size) / 2;
	vw_DrawText(X1+498+SizeI, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetText(ArcadeSim[GameConfig().Profile[CurrentProfile].SpaceShipControlMode]));




	Y1 += Prir1;
	std::ostringstream tmpStream;
	tmpStream << std::fixed << std::setprecision(0)
		  << vw_GetText("Current Profile Difficulty")
		  << ": " << ProfileDifficulty(CurrentProfile) << "%%"; // FIXME change to single %, after vw_DrawText() revise (variadic args)
	Size = vw_TextWidth(tmpStream.str().c_str());
	SizeI = (GameConfig().InternalWidth - Size) / 2;
	vw_DrawText(SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, tmpStream.str().c_str());


	int X = GameConfig().InternalWidth / 2 - 192;
	int Y = 175+95*5;
	if (DrawButton384(X,Y, vw_GetText("BACK"), MenuContentTransp, &Button1Transp, &LastButton1UpdateTime))
		cCommand::GetInstance().Set(eCommand::SWITCH_TO_PROFILE);
}

} // astromenace namespace
} // viewizard namespace
