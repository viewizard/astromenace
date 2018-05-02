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
	sRECT DstRect(Setup.InternalWidth / 2 - 427, 160, Setup.InternalWidth / 2 + 432, 160 + 480);
	vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/panel800_444_back.tga"), true, 0.9f*MenuContentTransp);

	int X1 = Setup.InternalWidth/2 - 372;
	int Y1 = 217;
	int Prir1 = 50;
	int Size;
	int SizeI;



	vw_DrawFont(X1, Y1, -340, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("Enemy Weapon Penalty"));
	bool ButOff = false;
	if (Setup.Profile[CurrentProfile].NPCWeaponPenalty == 1) ButOff = true;
	if (DrawButton128_2(X1+360, Y1-6, vw_GetText("Decrease"), MenuContentTransp, ButOff)) {
		Setup.Profile[CurrentProfile].NPCWeaponPenalty--;
		if (Setup.Profile[CurrentProfile].NPCWeaponPenalty < 1) Setup.Profile[CurrentProfile].NPCWeaponPenalty = 1;
	}
	ButOff = false;
	if (Setup.Profile[CurrentProfile].NPCWeaponPenalty == 3) ButOff = true;
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Increase"), MenuContentTransp, ButOff)) {
		Setup.Profile[CurrentProfile].NPCWeaponPenalty++;
		if (Setup.Profile[CurrentProfile].NPCWeaponPenalty > 3) Setup.Profile[CurrentProfile].NPCWeaponPenalty = 3;
	}
	if (Setup.Profile[CurrentProfile].NPCWeaponPenalty == 1) {
		Size = vw_FontSize(vw_GetText("None"));
		SizeI = (110-Size)/2;
		vw_DrawFont(X1+498+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("None"));
	} else {
		Size = vw_FontSize("x%i", Setup.Profile[CurrentProfile].NPCWeaponPenalty);
		SizeI = (110-Size)/2;
		vw_DrawFont(X1+498+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "x%i", Setup.Profile[CurrentProfile].NPCWeaponPenalty);
	}




	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -340, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("Enemy Armor Penalty"));
	ButOff = false;
	if (Setup.Profile[CurrentProfile].NPCArmorPenalty == 1) ButOff = true;
	if (DrawButton128_2(X1+360, Y1-6, vw_GetText("Decrease"), MenuContentTransp, ButOff)) {
		Setup.Profile[CurrentProfile].NPCArmorPenalty--;
		if (Setup.Profile[CurrentProfile].NPCArmorPenalty < 1) Setup.Profile[CurrentProfile].NPCArmorPenalty = 1;
	}
	ButOff = false;
	if (Setup.Profile[CurrentProfile].NPCArmorPenalty == 4) ButOff = true;
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Increase"), MenuContentTransp, ButOff)) {
		Setup.Profile[CurrentProfile].NPCArmorPenalty++;
		if (Setup.Profile[CurrentProfile].NPCArmorPenalty > 4) Setup.Profile[CurrentProfile].NPCArmorPenalty = 4;
	}
	if (Setup.Profile[CurrentProfile].NPCArmorPenalty == 1) {
		Size = vw_FontSize(vw_GetText("None"));
		SizeI = (110-Size)/2;
		vw_DrawFont(X1+498+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("None"));
	} else {
		Size = vw_FontSize("x%i", Setup.Profile[CurrentProfile].NPCArmorPenalty);
		SizeI = (110-Size)/2;
		vw_DrawFont(X1+498+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "x%i", Setup.Profile[CurrentProfile].NPCArmorPenalty);
	}




	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -340, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("Enemy Targeting Penalty"));
	ButOff = false;
	if (Setup.Profile[CurrentProfile].NPCTargetingSpeedPenalty == 1) ButOff = true;
	if (DrawButton128_2(X1+360, Y1-6, vw_GetText("Decrease"), MenuContentTransp, ButOff)) {
		Setup.Profile[CurrentProfile].NPCTargetingSpeedPenalty--;
		if (Setup.Profile[CurrentProfile].NPCTargetingSpeedPenalty < 1) Setup.Profile[CurrentProfile].NPCTargetingSpeedPenalty = 1;
	}
	ButOff = false;
	if (Setup.Profile[CurrentProfile].NPCTargetingSpeedPenalty == 4) ButOff = true;
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Increase"), MenuContentTransp, ButOff)) {
		Setup.Profile[CurrentProfile].NPCTargetingSpeedPenalty++;
		if (Setup.Profile[CurrentProfile].NPCTargetingSpeedPenalty > 4) Setup.Profile[CurrentProfile].NPCTargetingSpeedPenalty = 4;
	}
	if (Setup.Profile[CurrentProfile].NPCTargetingSpeedPenalty == 1) {
		Size = vw_FontSize(vw_GetText("None"));
		SizeI = (110-Size)/2;
		vw_DrawFont(X1+498+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("None"));
	} else {
		Size = vw_FontSize("x%i", Setup.Profile[CurrentProfile].NPCTargetingSpeedPenalty);
		SizeI = (110-Size)/2;
		vw_DrawFont(X1+498+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "x%i", Setup.Profile[CurrentProfile].NPCTargetingSpeedPenalty);
	}




	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -340, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("Limited Ammo"));
	if (DrawButton128_2(X1+360+128, Y1-6, vw_GetText(OnOff[Setup.Profile[CurrentProfile].LimitedAmmo]), MenuContentTransp, false)) {
		Setup.Profile[CurrentProfile].LimitedAmmo++;
		if (Setup.Profile[CurrentProfile].LimitedAmmo > 1) Setup.Profile[CurrentProfile].LimitedAmmo = 0;
	}




	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -340, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("Destroyable Weapon"));
	if (DrawButton128_2(X1+360+128, Y1-6, vw_GetText(OnOff[Setup.Profile[CurrentProfile].DestroyableWeapon]), MenuContentTransp, false)) {
		Setup.Profile[CurrentProfile].DestroyableWeapon++;
		if (Setup.Profile[CurrentProfile].DestroyableWeapon > 1) Setup.Profile[CurrentProfile].DestroyableWeapon = 0;
	}





	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -340, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("Weapon Targeting Mode"));
	if (DrawButton128_2(X1+360+128, Y1-6, vw_GetText(ArcadeSim[Setup.Profile[CurrentProfile].WeaponTargetingMode]), MenuContentTransp, false)) {
		Setup.Profile[CurrentProfile].WeaponTargetingMode++;
		if (Setup.Profile[CurrentProfile].WeaponTargetingMode > 1) Setup.Profile[CurrentProfile].WeaponTargetingMode = 0;
	}




	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -340, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("SpaceShip Control Mode"));
	if (DrawButton128_2(X1+360+128, Y1-6, vw_GetText(ArcadeSim[Setup.Profile[CurrentProfile].SpaceShipControlMode]), MenuContentTransp, false)) {
		Setup.Profile[CurrentProfile].SpaceShipControlMode++;
		if (Setup.Profile[CurrentProfile].SpaceShipControlMode > 1) Setup.Profile[CurrentProfile].SpaceShipControlMode = 0;
	}





	// считаем общую сложность
	Setup.Profile[CurrentProfile].Difficulty = 100-( (Setup.Profile[CurrentProfile].NPCWeaponPenalty-1)*6+
			(Setup.Profile[CurrentProfile].NPCArmorPenalty-1)*6+
			(Setup.Profile[CurrentProfile].NPCTargetingSpeedPenalty-1)*6+
			Setup.Profile[CurrentProfile].LimitedAmmo*14+
			Setup.Profile[CurrentProfile].DestroyableWeapon*11+
			Setup.Profile[CurrentProfile].WeaponTargetingMode*12+
			Setup.Profile[CurrentProfile].SpaceShipControlMode*15);



	Y1 += Prir1;
	Size = vw_FontSize("%s: %i%%", vw_GetText("Current Profile Difficulty"), Setup.Profile[CurrentProfile].Difficulty);
	SizeI = (Setup.InternalWidth-Size)/2;
	vw_DrawFont(SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, "%s: %i%%", vw_GetText("Current Profile Difficulty"), Setup.Profile[CurrentProfile].Difficulty);








	int X = Setup.InternalWidth/2 - 192;
	int Y = 175+95*5;
	if (DrawButton384(X,Y, vw_GetText("BACK"), MenuContentTransp, &Button1Transp, &LastButton1UpdateTime))
		ComBuffer = eCommand::SWITCH_TO_PROFILE;




}
